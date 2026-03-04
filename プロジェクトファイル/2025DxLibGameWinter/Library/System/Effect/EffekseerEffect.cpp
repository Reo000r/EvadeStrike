#include "EffekseerEffect.h"
#include <EffekseerForDXLib.h>

EffekseerEffect::EffekseerEffect() :
	_effectHandle(-1)
{
}

EffekseerEffect::~EffekseerEffect()
{
	// このインスタンスが消える時にエフェクトも削除
	if (_effectHandle != -1) DeleteEffect();
}

void EffekseerEffect::Init(const int handle, const Vector3& initPos)
{
	_effectHandle = handle;
	SetPos(initPos);
}

void EffekseerEffect::Update()
{
	// (エフェクトの更新はmanager側で行われる)
}

void EffekseerEffect::SetPos(const Position3& pos) const
{
	SetPosPlayingEffekseer3DEffect(_effectHandle,
		pos.x, pos.y, pos.z);			// 位置
}

void EffekseerEffect::SetRot(const Vector3& rot) const
{
	SetRotationPlayingEffekseer3DEffect(_effectHandle,
		rot.x, rot.y, rot.z);			// 角度
}

void EffekseerEffect::SetScale(const Vector3& scale) const
{
	SetScalePlayingEffekseer3DEffect(_effectHandle,
		scale.x, scale.y, scale.z);		// 拡縮
}

void EffekseerEffect::SetPlaySpeed(float playSpeed) const
{
	SetSpeedPlayingEffekseer3DEffect(_effectHandle,
		playSpeed);						// 再生速度
}

void EffekseerEffect::SetDynamicParam(const EffectData::DynamicParam& dynamicParam) const
{
	// 動的パラメータ
	SetDynamicInput3DEffect(_effectHandle, 0, dynamicParam.x);
	SetDynamicInput3DEffect(_effectHandle, 1, dynamicParam.y);
	SetDynamicInput3DEffect(_effectHandle, 2, dynamicParam.z);
	SetDynamicInput3DEffect(_effectHandle, 3, dynamicParam.w);
}

void EffekseerEffect::SetData(const EffectData& data) const
{
	// データ適用
	SetPos(data.pos);
	SetRot(data.rot);
	SetScale(data.scale);
	SetPlaySpeed(data.playSpeed);
	SetDynamicParam(data.dynamicParam);
}

bool EffekseerEffect::IsPlaying() const
{
	// 再生中なら0、再生停止/再生終了なら-1
	// MEMO:再生速度が0の場合は再生中判定になる？
	return IsEffekseer3DEffectPlaying(_effectHandle) == 0;
}

void EffekseerEffect::DeleteEffect()
{
	// エフェクトの削除(停止)を行い、
	// managerに自身を削除してもらう
	if (_effectHandle != -1) {
		StopEffekseer3DEffect(_effectHandle);
		_effectHandle = -1;
	}
}

