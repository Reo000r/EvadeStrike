#pragma once

/// <summary>
/// <para> ゲーム開始ボタンが起動中に初めて押されたかどうかを保持するシングルトン </para>
/// <para> セーブデータ等への永続化は行わない </para>
/// </summary>
class TutorialFlagHolder
{
private:
	TutorialFlagHolder();
	TutorialFlagHolder(const TutorialFlagHolder&) = delete;
	void operator=(const TutorialFlagHolder&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static TutorialFlagHolder& GetInstance();

	/// <summary>
	/// 起動中に既にゲーム開始ボタンが押されたか
	/// </summary>
	bool HasStartedGameOnce() const { return _hasStartedGameOnce; }

	/// <summary>
	/// ゲーム開始ボタンが押されたことを記録する
	/// </summary>
	void MarkGameStarted() { _hasStartedGameOnce = true; }

private:
	bool _hasStartedGameOnce;
};