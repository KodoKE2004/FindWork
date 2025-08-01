#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include <functional>
#include <windows.h>
#include <assert.h>
#include <locale.h>

#pragma warning(push)				//警告設定をスタックに保存
#pragma warning(disable:4005)		//マクロの再定義の指摘 ( 警告4005 ) を無効化

#pragma warning(pop)				//元の警告設定の復元 無効にした警告を有効化

#pragma comment (lib,"winmm.lib")

constexpr uint32_t SCREEN_WIDTH = 1280;	// ウィンドウの横幅
constexpr uint32_t SCREEN_HEIGHT = 720; // ウィンドウの縦幅
