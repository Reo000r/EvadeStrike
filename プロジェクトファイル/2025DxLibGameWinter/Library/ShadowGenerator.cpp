#include "ShadowGenerator.h"
#include "Library/Objects/GameObject.h"
#include <DxLib.h>
#include <cassert>

namespace {
	constexpr int kShadowMapSize = 8192;
	// 適応距離
	constexpr float kShadowMapRange = kShadowMapSize;
}

ShadowGenerator::ShadowGenerator() :
	_shadowMapHandle(-1)
{
}

ShadowGenerator::~ShadowGenerator()
{
	DeleteShadowMap(_shadowMapHandle);
}

void ShadowGenerator::Init(std::weak_ptr<GameObject> baseObject)
{
	if (baseObject.expired()) {
		assert(false && "不正なポインタ");
	}
	_baseObject = baseObject;
	_shadowMapHandle = MakeShadowMap(kShadowMapSize, kShadowMapSize);
}

void ShadowGenerator::Update()
{
}

void ShadowGenerator::ShadowDrawStart()
{
	// 影描画基準位置
	Position3 shadowBasePos = Position3(0,0,0);
	// 正常なオブジェクトであれば位置を参照する
	if (!_baseObject.expired()) {
		shadowBasePos = _baseObject.lock()->GetPos();
	}
	// シャドウマップ作成
	SetShadowMapLightDirection(_shadowMapHandle, VGet(0.5f, -1.0f, 0.5f));
	SetShadowMapDrawArea(
		_shadowMapHandle, 
		VSub(shadowBasePos, VGet(kShadowMapRange, 60.0f, kShadowMapRange)),
		VAdd(shadowBasePos, VGet(kShadowMapRange, 10.0f, kShadowMapRange)));
	// シャドウマップへの書き込み開始
	ShadowMap_DrawSetup(_shadowMapHandle);
}

void ShadowGenerator::ShadowDrawEnd()
{
	// シャドウマップ書き込み終了
	ShadowMap_DrawEnd();
	ClearDrawScreen();
}

void ShadowGenerator::NormalDrawStart()
{
	// シャドウマップを使用した描画
	SetUseShadowMap(0, _shadowMapHandle);
}

void ShadowGenerator::NormalDrawEnd()
{
	// シャドウマップの使用を終了
	SetUseShadowMap(0, -1);
}

