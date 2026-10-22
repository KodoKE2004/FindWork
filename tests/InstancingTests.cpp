// Exercises the production Renderer/Shader on D3D11 WARP without opening a window.
#include "Renderer.h"
#include "Shader.h"
#include "Material.h"
#include <d3dcompiler.h>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <iostream>
#include <cmath>

HINSTANCE Application::m_hInst = GetModuleHandleW(nullptr);
HWND Application::m_hWnd = nullptr;
uint32_t Application::m_Width = 64;
uint32_t Application::m_Height = 64;

// A static member definition may access the private HWND for this test host.
Application::Application(uint32_t, uint32_t)
{
    m_hWnd = CreateWindowExW(0, L"STATIC", L"Instancing test", WS_OVERLAPPED,
        0, 0, 64, 64, nullptr, nullptr, m_hInst, nullptr);
}
Application::~Application() { DestroyWindow(m_hWnd); }

static void require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}
static void check(HRESULT result) { require(SUCCEEDED(result), "D3D operation failed"); }

int main()
{
    try
    {
        Application host(64, 64);
        Renderer::Initialize(D3D_DRIVER_TYPE_WARP);
        auto* device = Renderer::GetDevice();
        auto* context = Renderer::GetDeviceContext();
        require(device && context, "Renderer initialization failed");
        // Read back the actual b4 buffer after unchanged updates, overrides and recreation.
        {
            MATERIAL source{};
            source.Diffuse = Color(.2f, .3f, .4f, .5f);
            Material cached(source);
            auto verifyMaterial = [&](const Color& expected) {
                ComPtr<ID3D11Buffer> bound, readback;
                context->PSGetConstantBuffers(4, 1, bound.GetAddressOf());
                require(bound != nullptr, "Material not rebound after unchanged Update");
                D3D11_BUFFER_DESC desc{};
                bound->GetDesc(&desc);
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                check(device->CreateBuffer(&desc, nullptr, readback.GetAddressOf()));
                context->CopyResource(readback.Get(), bound.Get());
                D3D11_MAPPED_SUBRESOURCE mapped{};
                check(context->Map(readback.Get(), 0, D3D11_MAP_READ, 0, &mapped));
                const auto actual = static_cast<const MATERIAL*>(mapped.pData)->Diffuse;
                context->Unmap(readback.Get(), 0);
                require(actual == expected, "Cached material contents differ");
            };
            cached.Update();
            verifyMaterial(source.Diffuse);
            ID3D11Buffer* empty = nullptr;
            context->PSSetConstantBuffers(4, 1, &empty);
            cached.Update();
            verifyMaterial(source.Diffuse);
            const Color changed(.7f, .6f, .5f, .4f);
            cached.SetDiffuse(changed);
            cached.Update();
            verifyMaterial(changed);
            cached.SetMaterial(source);
            verifyMaterial(source.Diffuse);
            cached.Update();
            verifyMaterial(changed);
            require(cached.Create(source), "Material recreation failed");
            verifyMaterial(source.Diffuse);
            std::cout << "Material cache: unchanged, rebind, change, override, recreate OK\n";
        }
        ComPtr<ID3D11Texture2D> target, staging;
        ComPtr<ID3D11RenderTargetView> rtv;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = desc.Height = 64;
        desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        check(device->CreateTexture2D(&desc, nullptr, target.GetAddressOf()));
        check(device->CreateRenderTargetView(target.Get(), nullptr, rtv.GetAddressOf()));
        desc.BindFlags = 0;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        check(device->CreateTexture2D(&desc, nullptr, staging.GetAddressOf()));
        context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
        Renderer::SetBlendState(BS_NONE);
        D3D11_RASTERIZER_DESC rs{};
        rs.FillMode = D3D11_FILL_SOLID;
        rs.CullMode = D3D11_CULL_NONE;
        rs.DepthClipEnable = TRUE;
        ComPtr<ID3D11RasterizerState> rasterizer;
        check(device->CreateRasterizerState(&rs, rasterizer.GetAddressOf()));
        context->RSSetState(rasterizer.Get());
        Matrix identity = Matrix::Identity;
        Renderer::SetViewMatrix(&identity);
        Renderer::SetProjectionMatrix(&identity);
        // Prefixes exercise StartIndexLocation and BaseVertexLocation.
        VERTEX_3D vertices[] = {
            {},
            { NVector3(-.5f, .5f, .5f), NVector3(1, 1, 0), Color(1,1,1,1), Vector2(0,0) },
            { NVector3(.5f, .5f, .5f), NVector3(1, 1, 0), Color(1,1,1,1), Vector2(1,0) },
            { NVector3(-.5f,-.5f, .5f), NVector3(1, 1, 0), Color(1,1,1,1), Vector2(0,1) },
            { NVector3(.5f,-.5f, .5f), NVector3(1, 1, 0), Color(1,1,1,1), Vector2(1,1) }
        };
        UINT indices[] = { 0, 0, 0, 0, 1, 2, 2, 1, 3 };
        auto buffer = [&](const void* data, UINT size, UINT flags) {
            ComPtr<ID3D11Buffer> result;
            D3D11_BUFFER_DESC d{};
            d.ByteWidth = size; d.Usage = D3D11_USAGE_IMMUTABLE; d.BindFlags = flags;
            D3D11_SUBRESOURCE_DATA init{ data, 0, 0 };
            check(device->CreateBuffer(&d, &init, result.GetAddressOf()));
            return result;
        };
        auto vb = buffer(vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
        auto ib = buffer(indices, sizeof(indices), D3D11_BIND_INDEX_BUFFER);
        UINT stride = sizeof(VERTEX_3D), offset = 0;
        context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
        // Sentinel stream verifies restoration even when slot 1 was already used.
        context->IASetVertexBuffers(1, 1, vb.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        const char* pixelSource = R"(
            cbuffer Mat : register(b4) { float4 ambient; float4 diffuse; };
            float4 main(float4 p:SV_POSITION, float4 color:COLOR0, float2 uv:TEXCOORD0):SV_Target
            { return float4(uv, color.r * diffuse.r, color.a * diffuse.a); }
        )";
        ComPtr<ID3DBlob> psCode;
        check(D3DCompile(pixelSource, strlen(pixelSource), nullptr, nullptr, nullptr, "main", "ps_5_0",
            0, 0, psCode.GetAddressOf(), nullptr));
        ComPtr<ID3D11PixelShader> ps;
        check(device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, ps.GetAddressOf()));
        context->PSSetShader(ps.Get(), nullptr, 0);

        auto clear = [&]() { const float zero[4]{}; context->ClearRenderTargetView(rtv.Get(), zero); };
        auto pixels = [&]() {
            context->CopyResource(staging.Get(), target.Get());
            D3D11_MAPPED_SUBRESOURCE map{};
            check(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &map));
            std::vector<float> output(64 * 64 * 4);
            for (size_t y = 0; y < 64; ++y)
                memcpy(output.data() + y * 64 * 4, static_cast<char*>(map.pData) + y * map.RowPitch, 64 * 4 * sizeof(float));
            context->Unmap(staging.Get(), 0);
            return output;
        };
        // Model/Square own their b4 buffers; instancing must not replace these with Renderer defaults.
        ComPtr<ID3D11Buffer> materialCB;
        D3D11_BUFFER_DESC materialDesc{};
        materialDesc.ByteWidth = sizeof(MATERIAL);
        materialDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        check(device->CreateBuffer(&materialDesc, nullptr, materialCB.GetAddressOf()));
        auto material = [&](Color color) {
            MATERIAL m{}; m.Diffuse = color;
            context->UpdateSubresource(materialCB.Get(), 0, nullptr, &m, 0, 0);
            context->VSSetConstantBuffers(4, 1, materialCB.GetAddressOf());
            context->PSSetConstantBuffers(4, 1, materialCB.GetAddressOf());
        };
        D3D11_QUERY_DESC qd{ D3D11_QUERY_PIPELINE_STATISTICS, 0 };
        ComPtr<ID3D11Query> query;
        check(device->CreateQuery(&qd, query.GetAddressOf()));
        auto stats = [&]() {
            D3D11_QUERY_DATA_PIPELINE_STATISTICS value{};
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
            HRESULT hr;
            while ((hr = context->GetData(query.Get(), &value, sizeof(value), 0)) == S_FALSE)
            {
                require(std::chrono::steady_clock::now() < deadline, "GPU query timed out");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            check(hr);
            return value;
        };
        for (const char* name : { "VS_Alpha", "VS_Default", "VS_Unlit", "VS_Instansing2D" })
        {
            VertexShader shader;
            require(shader.Create(std::string("04_ShaderFile/") + name + ".hlsl"), "Shader variants failed");
            require(shader.SupportsInstancing(), "Missing instancing variant");
            shader.SetGPU();
            for (size_t count : { 0u, 1u, 512u, 513u, 1024u, 1025u, 2049u })
            {
                std::vector<InstanceData> instances(count);
                for (size_t i = 0; i < count; ++i)
                {
                    auto& v = instances[i];
                    v.world = Matrix::CreateScale(.35f, .55f, 1)
                        * Matrix::CreateRotationZ(.12f * static_cast<float>(i % 3))
                        * Matrix::CreateTranslation((i % 2) ? .4f : -.4f, 0, 0);
                    v.color = Color(.2f + .1f * static_cast<float>(i % 7), 1, 1, .7f);
                    v.uvRect = Vector4(.1f, .2f, .4f, .6f);
                }
                clear();
                for (auto& v : instances)
                {
                    Renderer::SetWorldMatrix(&v.world);
                    material(v.color);
                    // Unlit intentionally uses raw mesh UV in the original path.
                    Renderer::SetUV(v.uvRect.x, v.uvRect.y, v.uvRect.z, v.uvRect.w);
                    if (std::string(name) == "VS_Unlit") v.uvRect = Vector4(0,0,1,1);
                    Renderer::DrawIndexed(6, 3, 1);
                }
                auto expected = pixels();
                if (count) require(std::any_of(expected.begin(), expected.end(), [](float v) { return v > .1f; }), "Reference render is blank");
                clear();
                material(Color(1,1,1,1));
                Renderer::SetUV(0,0,1,1);
                // Must ignore a non-identity representative world matrix.
                Matrix wrong = Matrix::CreateTranslation(100,100,100);
                Renderer::SetWorldMatrix(&wrong);
                context->Begin(query.Get());
                require(Renderer::DrawIndexedInstanced(6, 3, 1, instances, shader), "Instanced submission failed");
                context->End(query.Get());
                require(stats().IAPrimitives == 2 * count, "Instance count truncated or duplicated");
                auto actual = pixels();
                for (size_t i = 0; i < actual.size(); ++i)
                    require(std::abs(actual[i] - expected[i]) < 2e-5f, "Individual/instanced image mismatch");
                ComPtr<ID3D11Buffer> restored;
                UINT restoredStride, restoredOffset;
                context->IAGetVertexBuffers(1, 1, restored.GetAddressOf(), &restoredStride, &restoredOffset);
                require(restored.Get() == vb.Get() && restoredStride == stride && restoredOffset == offset, "Slot 1 not restored");
                // Verify scope dispatch and automatic return to ordinary draws.
                clear();
                { Renderer::ScopedInstances scope(instances, shader); Renderer::DrawIndexed(6, 3, 1); }
                auto scoped = pixels();
                for (size_t i = 0; i < actual.size(); ++i) require(std::abs(actual[i]-scoped[i]) < 2e-5f, "Scoped draw mismatch");
                Renderer::SetWorldMatrix(&identity);
                context->Begin(query.Get()); Renderer::DrawIndexed(6, 3, 1); context->End(query.Get());
                require(stats().IAPrimitives == 2, "Instancing state leaked to ordinary draw");
                std::cout << name << ": " << count << " instances OK\n";
            }
            // Reload must recreate both variants and preserve the old pair on failure.
            require(shader.Create(std::string("04_ShaderFile/") + name + ".hlsl"), "Reload failed");
            require(!shader.Create("04_ShaderFile/does-not-exist.hlsl") && shader.SupportsInstancing(), "Failed reload destroyed shaders");
        }
        // Nonuniform scaling must use inverse-transpose normals, not world normals.
        VertexShader normalVS;
        require(normalVS.Create("04_ShaderFile/VS_Unlit.hlsl"), "Normal shader failed");
        normalVS.SetGPU();
        const char* normalSource = "float4 main(float4 p:SV_POSITION, float4 c:COLOR0, float2 uv:TEXCOORD0, float3 n:TEXCOORD1):SV_Target { return float4(n,1); }";
        ComPtr<ID3DBlob> normalCode;
        check(D3DCompile(normalSource, strlen(normalSource), nullptr, nullptr, nullptr, "main", "ps_5_0",
            0, 0, normalCode.GetAddressOf(), nullptr));
        ComPtr<ID3D11PixelShader> normalPS;
        check(device->CreatePixelShader(normalCode->GetBufferPointer(), normalCode->GetBufferSize(), nullptr, normalPS.GetAddressOf()));
        context->PSSetShader(normalPS.Get(), nullptr, 0);
        std::vector<InstanceData> normalInstances(1);
        normalInstances[0].world = Matrix::CreateScale(2,1,1);
        clear();
        require(Renderer::DrawIndexedInstanced(6,3,1,normalInstances,normalVS), "Normal draw failed");
        auto normals = pixels();
        const size_t center = (32*64+32)*4;
        std::cout << "Normal center: " << normals[center] << ", " << normals[center+1] << ", " << normals[center+2] << '\n';
        require(std::abs(normals[center] - 1/std::sqrt(5.f)) < 1e-5f
            && std::abs(normals[center+1] - 2/std::sqrt(5.f)) < 1e-5f, "Inverse transpose normal failed");
        std::cout << "Inverse-transpose normals OK\n";
        VertexShader grid, sky;
        require(grid.Create("04_ShaderFile/VS_DebugGrid.hlsl") && !grid.SupportsInstancing(), "Grid signature failed");
        require(sky.Create("04_ShaderFile/VS_Skydome.hlsl") && sky.SupportsInstancing(), "Sky signature failed");
        Renderer::Finalize();
        Renderer::Initialize(D3D_DRIVER_TYPE_WARP);
        require(Renderer::GetDevice() != nullptr, "Reinitialization failed");
        Renderer::Finalize();
        std::cout << "PASS: production renderer, image comparison, counts, subsets, state restoration, reload, reinitialize\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        Renderer::Finalize();
        return 1;
    }
}
