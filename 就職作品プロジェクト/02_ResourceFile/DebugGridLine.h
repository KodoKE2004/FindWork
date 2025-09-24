#pragma once
#include "VertexBuffer.h"
#include "ShaderManager.h"
#include <wrl/client.h>
using namespace Microsoft::WRL;

class DebugGridLine
{
public:
    DebugGridLine()  = default ;
    ~DebugGridLine() = default ;
        
    void Initialize();
    void Finalize();
    void Draw();

    void SetEnabled(bool enable) { m_Enabled = enable;    }
    bool IsEnabled()     const   { return m_Enabled;      }
    void DeInitialized()         { m_Initialized = false; }
    bool IsInitialized() const   { return m_Initialized;  }
private:
    struct GridParams
    {
        float start;
        float spacing;
        float axis;
        float padding;
    };
    ComPtr<ID3D11Buffer>      m_InstanceIDVB;
    ComPtr<ID3D11InputLayout> m_GridInputLayout;

    void UpdateGridParams(float axisFlag);
    void DrawAxis(VertexBuffer<VERTEX_3D>& buffer, float axisFlag);

    VertexBuffer<VERTEX_3D> m_LineX;
    VertexBuffer<VERTEX_3D> m_LineZ;
    ComPtr<ID3D11Buffer> m_GridParamBuffer;
    VertexShader* m_VertexShader = nullptr;
    PixelShader* m_PixelShader = nullptr;

    bool m_Initialized = false;
    bool m_Enabled = true;

    float m_Spacing    = 10.0f;
    float m_Start      = - 10000.0f;
    float m_HalfExtent =   10000.0f;
    unsigned int m_InstanceCount = 0;
    
};

