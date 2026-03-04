#pragma once
class ResultDataHolder
{
private:
	ResultDataHolder();
	ResultDataHolder(const ResultDataHolder&) = delete;
	void operator=(const ResultDataHolder&) = delete;

public:

	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static ResultDataHolder& GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// コピーフラグが立っている場合リザルト用の画面を取得する
	/// </summary>
	void CopyResultScreen();

	/// <summary>
	/// リザルト用の画面を取得する予定を立てる
	/// </summary>
	void ReserveCopyResultScreen();

	void SetLastGraphHandle(int lastGraphHandle) { _lastGraphHandle = lastGraphHandle; }
	int GetLastGraphHandle() const { return _lastGraphHandle; }
	void AddScore(float score);
	float GetScore() const { return _score; }

private:

	float _score;
	int _lastGraphHandle;
	int _copyScreenProgress;

};

