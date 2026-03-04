#pragma once
#include <string>
#include <unordered_map>

/// <summary>
/// 
/// </summary>
class ObjectHandleHolder
{
public:
	ObjectHandleHolder();
	virtual ~ObjectHandleHolder();

	/// <summary>
	/// 読み込み
	/// </summary>
	void Load();

	/// <summary>
	/// 指定の名前のオブジェクトのハンドルを複製して返す
	/// </summary>
	/// <returns></returns>
	int GetModelHandle(std::string name);

private:

	std::unordered_map<std::string, int> _modelHandleList;
};

