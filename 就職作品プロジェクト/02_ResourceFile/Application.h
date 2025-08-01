#pragma once

#include    <Windows.h>
#include    <cstdint>

class Application
{
public:
    Application(uint32_t width, uint32_t height);
    ~Application();
    void Run();

    // 幅を取得
    static uint32_t GetWidth() {
        return m_Width;
    }

    // 高さを取得
    static uint32_t GetHeight() {
        return m_Height;
    }

    // ウインドウハンドルを返す
    static HWND GetWindow() {
        return m_hWnd;
    }

private:
    static HINSTANCE   m_hInst;     // インスタンスハンドル
    static HWND        m_hWnd;      // ウィンドウハンドル
    static uint32_t    m_Width;     // ウィンドウの横幅
    static uint32_t    m_Height;    // ウィンドウの縦幅

    // 初期化処理
	static bool InitApp();          
    // 終了処理
	static void TermApp();          
    // ウィンドウの初期化処理
	static bool InitWindow();       
    // ウィンドウの終了処理
	static void TermWindow();       
    // メインループ
	static void MainLoop();         
	// ウィンドウプロシージャ
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

