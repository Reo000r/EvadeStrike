#pragma once
#include <string>
#include <DxLib.h>

namespace StringUtility {
	/// <summary>
	/// stringからwstringへ変換する
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::wstring GetWStringFromString(const std::string& str);

	/// <summary>
	/// wstringからstringへ変換する
	/// 動作未確認
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string GetStringFromWString(const std::wstring& wstr);
}