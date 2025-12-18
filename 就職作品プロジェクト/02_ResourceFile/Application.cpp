#include <chrono>
#include <thread>
#include <algorithm>
#include "Application.h"
#include "Game.h"
#include "imgui_impl_win32.h"

const auto ClassName  = TEXT("就職作品");              //!< ウィンドウクラス名.
const auto WindowName = TEXT("就職作品 初期化終わり");    //!< ウィンドウ名.

HINSTANCE  Application::m_hInst;        // インスタンスハンドル
HWND       Application::m_hWnd;         // ウィンドウハンドル
uint32_t   Application::m_Width;        // ウィンドウの横幅
uint32_t   Application::m_Height;       // ウィンドウの縦幅
uint32_t   Application::m_GameWidth;    // ゲーム画面の横幅
uint32_t   Application::m_GameHeight;   // ゲーム画面の縦幅

// ImGuiのWin32プロシージャハンドラ(マウス対応)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//==============================
//		コンストラクタ
//==============================
Application::Application(uint32_t width, uint32_t height)
{
    m_Height = height;
    m_Width  = width;
    m_GameWidth  = width;
    m_GameHeight = height;

    timeBeginPeriod(1);
}

//==============================
//		デストラクタ
//==============================
Application::~Application()
{
    timeEndPeriod(1);
}

//==============================
//		アプリケーションの実行
//==============================
void Application::Run()
{
	//==============================
	// アプリケーションの初期化処理
	//==============================
	if (InitApp()) { 

        //==============================
        //        メインループ
        //==============================
        MainLoop();
    
    }

	//==============================
    // アプリケーションの終了処理
	//==============================
	TermApp();
}

// Application.cppに追加するdeltaTime管理用変数
namespace {
    float g_deltaTime = 0.0f;
    LARGE_INTEGER g_prevTime;
    LARGE_INTEGER g_frequency;
}

// deltaTime取得関数
float Application::GetDeltaTime()
{
    return g_deltaTime;
}

//==============================
//		初期化処理
//==============================
bool Application::InitApp()
{

    QueryPerformanceFrequency(&g_frequency);
    QueryPerformanceCounter(&g_prevTime);
    return InitWindow();
}

//==============================
//		アプリケーションの終了処理
//==============================
void Application::TermApp()
{
	// ウィンドウの終了処理
	TermWindow();
}

//==============================
//		ウィンドウの初期化処理
//==============================
bool Application::InitWindow()
{
	// インスタンスハンドルを取得.
	auto hInst = GetModuleHandle(nullptr);      //リソースのロードや他のモジュール関連の操作に使用する変数を用意
	if (hInst == nullptr)                       //関数が失敗したか確認
	{
		return false;
	}

	WNDCLASSEX wc = {};
	
	wc.cbSize        = sizeof(WNDCLASSEX);                   //構造体のサイズを設定
	wc.style         = CS_HREDRAW | CS_VREDRAW;              //ウィンドウの再描画スタイルを設定
	wc.lpfnWndProc   = WndProc;                              //ウィンドウプロシージャの関数ポインタ設定
	wc.hIcon         = LoadIcon(hInst, IDI_APPLICATION);     //大きいアイコンをロード
	wc.hCursor       = LoadCursor(hInst, IDC_ARROW);         //カーソルをロード
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);   //背景色を設定
	wc.lpszMenuName  = nullptr;                              //メニュー名を設定
	wc.lpszClassName = ClassName;                            //ウィンドウクラス名を設定
	wc.hIconSm       = LoadIcon(hInst, IDI_APPLICATION);     //小さいアイコンをロード

    // ウィンドウの登録
    // ウィンドウクラスの登録して成功か失敗の結果を返す
    if (!RegisterClassEx(&wc)){
        return false; 
    }

    // インスタンスハンドル設定.
    m_hInst = hInst;

    // ウィンドウのサイズを設定.
    RECT rc = {};
    rc.right = static_cast<LONG>(m_Width);
    rc.bottom = static_cast<LONG>(m_Height);

    // ウィンドウサイズを調整.
    auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRect(&rc, style, FALSE);

    // ウィンドウを生成.
    m_hWnd = CreateWindowEx(    
        0,                      //拡張ウィンドウスタイル
        //WS_EX_TOPMOST,        //ウィンドウを常に最前面に表示するスタイル
        ClassName,              //ウィンドウクラス名
        WindowName,             //ウィンドウ名（タイトル）
        style,                  //ウィンドウスタイル
        CW_USEDEFAULT,          //初期X座標
        CW_USEDEFAULT,          //初期Y座標
        rc.right - rc.left,     //ウィンドウの幅
        rc.bottom - rc.top,     //ウィンドウの高さ
        nullptr,                //親ウィンドウのハンドル
        nullptr,                //メニューのハンドル
        m_hInst,                //インスタンスハンドル
        nullptr                 //追加の作成パラメータ
    );

    if (m_hWnd == nullptr){
        return false;
    }

    // ウィンドウを表示.
    ShowWindow(m_hWnd, SW_NORMAL);  //第２引数

    // ウィンドウを更新.
    UpdateWindow(m_hWnd);             //再描画するウィンドウ

    // ウィンドウにフォーカスを設定.
    SetFocus(m_hWnd);                 //キー入力を受けつけるようにする

    // 正常終了.
    return true;
}

//==============================
//		ウィンドウの終了処理
//==============================
void Application::TermWindow()
{
    // ウィンドウの登録を解除.
    if (m_hInst != nullptr)
    {
        UnregisterClass(ClassName, m_hInst);
    }

    m_hInst = nullptr;
    m_hWnd = nullptr;
}


//==============================
//		メインループ
//==============================
void Application::MainLoop()
{
    MSG   msg = {};
    auto& instance = Game::GetInstance();
    instance.Initialize();

    static HWND s_bound = nullptr;
    HWND now = GetWindow();
    if (now && IsWindow(now) && now != s_bound)
    {
        DirectX::Mouse::Get().SetWindow(now);
        s_bound = now;
    }

    const double targetFrameSec = 1.0 / 60.0;

    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) break;

        }
        else
        {
            auto& mouse = DirectX::Mouse::Get();
            mouse.SetWindow(GetWindow());

            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            double dt = static_cast<double>(currentTime.QuadPart - g_prevTime.QuadPart)
                      / static_cast<double>(g_frequency.QuadPart);

            if (dt < targetFrameSec)
            {
                double remain = targetFrameSec - dt;
                DWORD sleepMs = static_cast<DWORD>(remain * 1000.0);

                if (sleepMs > 0) {
                    Sleep(sleepMs);
                }

                QueryPerformanceCounter(&currentTime);
                dt = static_cast<double>(currentTime.QuadPart - g_prevTime.QuadPart)
                    / static_cast<double>(g_frequency.QuadPart);
            }

            g_deltaTime = static_cast<float>(dt);
            g_prevTime = currentTime;

            // ゲームの更新
            instance.Update(Application::GetDeltaTime());
            instance.Draw();
        }
    }
    instance.Finalize();
}

//==============================
//		ウィンドウプロシージャ
//==============================
LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
    DirectX::Mouse::   ProcessMessage(msg, wParam, lParam);

    switch (msg)
    {
    case WM_DESTROY:// ウィンドウ破棄のメッセージ
    {
        PostQuitMessage(0);// 「WM_QUIT」メッセージを送る　→　アプリ終了
        return 0;
    }
    break;

    case WM_CLOSE:  // 「x」ボタンが押されたら
    {
        int res = MessageBoxA(NULL, "終了しますか？", "確認", MB_OKCANCEL);
        if (res == IDOK) {
            DestroyWindow(hWnd);  // WM_DESTROY が呼ばれて PostQuitMessage が送られる
        }
        return 0;
    }
    break;

    case WM_KEYDOWN: //キー入力があったメッセージ
    {
        if (LOWORD(wParam) == VK_ESCAPE) { //入力されたキーがESCAPEなら
            PostMessage(hWnd, WM_CLOSE, wParam, lParam);    //「WM_CLOSE」を送る
        }
    }
    break;

    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
