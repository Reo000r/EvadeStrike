#include "StringUtility.h"

std::wstring StringUtility::GetWStringFromString(const std::string& str)
{
	// まずは変換後のサイズを測っておく
		// (変換前と変換後でサイズが異なる場合があるため)
	int size = MultiByteToWideChar(
		CP_ACP,  // 現在のコードページ
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,  // 0でも通るらしい
		str.c_str(),
		static_cast<int>(str.size()),
		nullptr,
		0
	);

	// そのサイズを元に返すべきstringをリサイズ
	std::wstring wstr;
	wstr.resize(size);

	// サイズが確定したのでもう一度MultiByteToWideCharを呼び出す(実際に変換)
	MultiByteToWideChar(
		CP_ACP,  // 現在のコードページ
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		static_cast<int>(str.size()),
		wstr.data(),					// wstrのデータ本体の先頭アドレス
		static_cast<int>(wstr.size())	// wstrのデータサイズ
	);

	return wstr;
}

std::string StringUtility::GetStringFromWString(const std::wstring& wstr)
{
	// CP_UTF8 は変換先の文字コードをUTF-8に指定
	// 変換後の文字列長を取得
	int buffer_size = WideCharToMultiByte(
		CP_UTF8, 
		0, 
		wstr.c_str(), 
		-1, 
		NULL, 
		0, 
		NULL, 
		NULL);
	if (buffer_size == 0) {
	    return ""; // 変換失敗
	}
	
	// バッファを確保
	std::string str(buffer_size - 1, '\0'); // -1はヌル終端文字分
	
	// 変換実行
	WideCharToMultiByte(
		CP_UTF8, 
		0, 
		wstr.c_str(), 
		-1, 
		&str[0], 
		buffer_size, 
		NULL, 
		NULL);
	
	return str;
}
