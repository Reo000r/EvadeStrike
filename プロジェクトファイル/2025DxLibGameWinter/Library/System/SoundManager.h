#pragma once
#include <unordered_map>

enum class SEType {
	Enter,
	AttackPunch,
	AttackKick,
	AttackSP,
	AttackSwing,
	StageUnlock,
	JustDodge,
	typeNum,
};

enum class BGMType {
	Title,
	GamePlay,
	Result,
	typeNum,
};

/// <summary>
/// BGMや効果音を管理するシングルトンクラス
/// </summary>
class SoundManager
{
private:
	SoundManager() :
		_seList(),
		_bgmList()
	{
	}
	SoundManager(const SoundManager&) = delete;
	void operator=(const SoundManager&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static SoundManager& GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();
	/// <summary>
	/// 終了処理
	/// </summary>
	void Terminate();

	/// <summary>
	/// 必要なサウンドをすべて読み込む
	/// </summary>
	void LoadResources();

	/// <summary>
	/// 読み込んだサウンドをすべて解放する
	/// </summary>
	void ReleaseResources();

	/// <summary>
	/// 指定されたタイプの効果音を再生する
	/// </summary>
	/// <param name="type">効果音タイプ</param>
	void PlaySoundType(SEType type);
	/// <summary>
	/// 指定されたタイプのBGMを再生する
	/// </summary>
	/// <param name="type">BGMタイプ</param>
	/// <param name="isLoop">ループさせるか</param>
	/// <param name="isPlayFromStart">途中からの再生を許可するか</param>
	void PlaySoundType(BGMType type, bool isLoop = true, bool isPlayFromStart = true);

private:

	// SE種別、ハンドルを管理
	std::unordered_map<SEType, int> _seList;

	// BGM種別、ハンドルを管理
	std::unordered_map<BGMType, int> _bgmList;
};

