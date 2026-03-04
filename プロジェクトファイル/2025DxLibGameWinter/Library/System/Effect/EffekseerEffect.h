#pragma once
#include "Library/Geometry/Vector3.h"

/// <summary>
/// エフェクトを構成する情報をまとめたデータ
/// (エフェクトハンドル以外)
/// </summary>
struct EffectData {
	struct DynamicParam {
		float x = 0.0f; float y = 0.0f; float z = 0.0f; float w = 0.0f;
	};
	
	Position3 pos = Position3();		// 位置
	Vector3 rot = Vector3();			// 回転量
	Vector3 scale = Vector3(1,1,1);		// 大きさ
	float playSpeed = 1.0f;				// 再生速度
	DynamicParam dynamicParam;			// 動的パラメータ

};

/// <summary>
/// 生成されたエフェクト
/// </summary>
class EffekseerEffect final
{
public:

	EffekseerEffect();
	~EffekseerEffect();

	void Init(const int handle, const Vector3& initPos);
	void Update();

	// 各種パラメータ設定

	/// <summary>
	/// 位置設定
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Position3& pos) const;
	/// <summary>
	/// 角度(ラジアン)設定
	/// </summary>
	/// <param name="rot"></param>
	void SetRot(const Vector3& rot) const;
	/// <summary>
	/// 拡縮設定
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) const;
	/// <summary>
	/// 再生速度設定
	/// </summary>
	/// <param name="playSpeed"></param>
	void SetPlaySpeed(float playSpeed) const;
	/// <summary>
	/// 動的パラメータ設定
	/// </summary>
	/// <param name="dynamicParam"></param>
	void SetDynamicParam(const EffectData::DynamicParam& dynamicParam) const;

	/// <summary>
	/// エフェクトデータ設定
	/// </summary>
	/// <param name="data"></param>
	void SetData(const EffectData& data) const;

	/// <summary>
	/// 再生中かどうか
	/// </summary>
	/// <returns></returns>
	bool IsPlaying() const;

	/// <summary>
	/// エフェクトを削除(停止)
	/// </summary>
	void DeleteEffect();

	int GetEffectHandle() { return _effectHandle; }

private:
	// エフェクトハンドル
	int _effectHandle;

};

