#include "EffectManager.h"
#include "EffekseerEffect.h"
#include "EffectFactory.h"
#include <EffekseerForDXLib.h>

namespace {
	// パーティクル数上限
	constexpr int kEffectPerticleLimit = 32768;
}

EffectManager& EffectManager::GetInstance()
{
	static EffectManager manager;
	return manager;
}

bool EffectManager::Init()
{
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	// 初期化処理に失敗したら終了
	if (Effekseer_Init(kEffectPerticleLimit) == -1) {
#ifdef _DEBUG
		printf("EffekseerForDXLib初期化失敗\n");
#endif	// _DEBUG
		return false;
	}

#ifdef _DEBUG
	printf("EffekseerForDXLib初期化成功\n");
#endif	// _DEBUG

	// フルスクリーン対応
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	// デバイスロスト対応
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// 歪み機能を有効
	Effekseer_InitDistortion();

	// Zバッファの設定はほかの場所で実行している

	// エフェクトの読み込み
	EffectFactory::LoadResources();

	return true;
}

void EffectManager::Update()
{
	// エフェクト更新
	UpdateEffekseer3D();

	// 再生が終了していないエフェクトを引き継ぐ
	std::forward_list<std::shared_ptr<EffekseerEffect>> playingEffects;
	for (std::shared_ptr<EffekseerEffect>& effect : _effects) {
		// 再生終了または削除されたエフェクトは
		// 引き継がない
		if (!effect->IsPlaying()) continue;

		// 再生中の場合は引き継ぐ
		playingEffects.emplace_front(effect);
	}
	// 再生中のエフェクトのみ保存
	_effects = playingEffects;
}

void EffectManager::Draw() const
{
	// DxLibのカメラ行列をEffekseerに同期
	Effekseer_Sync3DSetting();
	// エフェクト描画
	DrawEffekseer3D_Begin();
	for (auto& effect : _effects) {
		DrawEffekseer3D_Draw(effect->GetEffectHandle());
	}
	DrawEffekseer3D_End();
}

void EffectManager::Terminate()
{
	// 再生中のエフェクト解放
	DeleteAllEffect();
	// エフェクトハンドル解放
	EffectFactory::ReleaseResources();
}

std::weak_ptr<EffekseerEffect> EffectManager::GenerateEffect(
	const std::string& fileName, 
	const Position3& initPos)
{
	// エフェクトの生成
	std::shared_ptr<EffekseerEffect> effect = EffectFactory::GenerateEffect(fileName, initPos);
	// 正常に生成できなかった場合はnullptrを返す
	if (!effect) {
		effect = nullptr;
	}
	// 正常だった場合は登録
	else {
		_effects.emplace_front(effect);
	}
	printf("エフェクト生成:%s (%.03f,%.03f,%.03f)\n",
		fileName.c_str(), initPos.x, initPos.y, initPos.z);
	return effect;
}

void EffectManager::DeleteAllEffect()
{
	// 各インスタンス削除の際にエフェクトも消える
	_effects.clear();
}
