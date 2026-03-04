#pragma once
#include <string>

namespace Statistics
{
	//constexpr int kScreenWidth  = 1920+32;
	//constexpr int kScreenHeight = 1080+18;
	constexpr int kScreenWidth  = 1280;
	constexpr int kScreenHeight = 720;
	//constexpr int kScreenWidth  = 640;
	//constexpr int kScreenHeight = 360;
	constexpr float kScreenWidthFullHDRatio = kScreenWidth / 1920.0f;
	
	constexpr float kScreenCenterWidth  = kScreenWidth * 0.5f;
	constexpr float kScreenCenterHeight = kScreenHeight * 0.5f;

	constexpr float kFPS = 60.0f;
	constexpr int kStopTime = static_cast<int>(1000000 / kFPS);

	constexpr int kFadeInterval = 30;

	//const std::wstring kDefaultFontName = L"BIZ UDP明朝 Medium";
	const std::wstring kDefaultFontName = L"Impact";
}