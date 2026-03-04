#include "SoundManager.h"
#include <string>
#include <cassert>
#include <DxLib.h>

namespace {
	// サウンドファイルのパスをここで管理
	const std::unordered_map<SEType, std::wstring> kSEPaths = {
		{ SEType::Enter,			L"Data/Sound/SE/SEEnter.mp3" },
		{ SEType::AttackPunch,		L"Data/Sound/SE/SEPunch.mp3" },
		{ SEType::AttackKick,		L"Data/Sound/SE/SEKick.mp3" },
		{ SEType::AttackSP,			L"Data/Sound/SE/SESpecialAttack.mp3" },
		{ SEType::AttackSwing,		L"Data/Sound/SE/SESwing.mp3" },
		{ SEType::StageUnlock,		L"Data/Sound/SE/SEStageUnlock.mp3" },
		{ SEType::JustDodge,		L"Data/Sound/SE/SEJustDodge.mp3" },
	};
	const std::unordered_map<BGMType, std::wstring> kBGMPaths = {
		{ BGMType::Title,		L"Data/Sound/BGM/BGMTitle.mp3" },
		{ BGMType::GamePlay,	L"Data/Sound/BGM/BGMGamePlay.mp3" },
		{ BGMType::Result,		L"Data/Sound/BGM/BGMResult.mp3" },
	};

	constexpr float kMasterVolumeRate = 0.7f;
	constexpr int kSEVolume = static_cast<int>(255 * 1.0f * kMasterVolumeRate);
	constexpr int kBGMVolume = static_cast<int>(255 * 0.6f * kMasterVolumeRate);
}

SoundManager& SoundManager::GetInstance()
{
	// 初実行時にメモリ確保
	static SoundManager manager;
	return manager;
}

void SoundManager::Init()
{
	// 読み込み
	LoadResources();
}

void SoundManager::Terminate()
{
	// 解放
	ReleaseResources();
}

void SoundManager::LoadResources()
{
	// 全てのサウンドを読み込み、ハンドルを保存する
	for (const auto& pair : kSEPaths) {
		const SEType& type = pair.first;
		const std::wstring& path = pair.second;
		int handle = LoadSoundMem(path.c_str());
		assert(handle != -1 && "サウンドの読み込みに失敗");
		_seList[type] = handle;
		ChangeVolumeSoundMem(kSEVolume, handle);
	}
	for (const auto& pair : kBGMPaths) {
		const BGMType& type = pair.first;
		const std::wstring& path = pair.second;
		int handle = LoadSoundMem(path.c_str());
		assert(handle != -1 && "サウンドの読み込みに失敗");
		_bgmList[type] = handle;
		ChangeVolumeSoundMem(kBGMVolume, handle);
	}
}

void SoundManager::ReleaseResources()
{
	// 保存されている全てのサウンドを解放する
	for (const auto& pair : _seList) {
		DeleteSoundMem(pair.second);
	}
	_seList.clear();
	for (const auto& pair : _bgmList) {
		DeleteSoundMem(pair.second);
	}
	_bgmList.clear();
}

void SoundManager::PlaySoundType(SEType type)
{
	// 複製元のサウンドが存在するかチェック
	auto it = _seList.find(type);
	assert(it != _seList.end() && "要求されたタイプのサウンドが読み込まれていない");

	// 効果音再生
	PlaySoundMem(it->second, DX_PLAYTYPE_BACK, true);
}

void SoundManager::PlaySoundType(BGMType type, bool isLoop, bool isPlayFromStart)
{
	// 複製元のサウンドが存在するかチェック
	auto it = _bgmList.find(type);
	assert(it != _bgmList.end() && "要求されたタイプのサウンドが読み込まれていない");

	for (const std::pair<BGMType, int>& pair : _bgmList) {
		// 同じBGMかつ途中からの再生を許可している場合はcontinue
		if (!isPlayFromStart && type == pair.first) continue;
		// 既存のBGMが鳴っていたら停止
		if (CheckSoundMem(pair.second) == 1) {
			StopSoundMem(pair.second);
		}
	}

	// 指定された曲がなっていたかつ
	// 最初からの再生を希望されていないならreturn
	if (CheckSoundMem(_bgmList[type]) == 1 &&
		!isPlayFromStart) return;

	// BGM再生
	int playType = DX_PLAYTYPE_LOOP;
	if (!isLoop) playType = DX_PLAYTYPE_BACK;
	PlaySoundMem(it->second, playType, true);
}
