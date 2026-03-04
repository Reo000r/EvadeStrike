#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <string>
#include <unordered_map>

class EffekseerEffect;

/// <summary>
/// エフェクトの生成を行い、インスタンスを返すstaticクラス
/// </summary>
class EffectFactory final
{
public:

	/// <summary>
	/// 必要なエフェクトをすべて読み込む
	/// </summary>
	static void LoadResources();
	/// <summary>
	/// 読み込んだエフェクトをすべて解放する
	/// </summary>
	static void ReleaseResources();

	/// <summary>
	/// <para> 元となるエフェクトハンドルから </para>
	/// <para> エフェクトの生成、初期化を行う </para>
	/// <para> 生成が失敗した場合、不完全なエフェクトを返す </para>
	/// </summary>
	/// <param name="fileName">エフェクトファイル名</param>
	/// <param name="initPos">エフェクトの初期位置</param>
	/// <returns>エフェクトインスタンス</returns>
	static std::shared_ptr<EffekseerEffect> GenerateEffect(
		const std::string& fileName, const Position3& initPos);

private:
	// エフェクト名とハンドル
	static std::unordered_map<std::string, int> _effectSourceHandles;
};
