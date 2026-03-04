#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <forward_list>
#include <string>

class EffekseerEffect;

/// <summary>
/// エフェクトの生成、保持、更新などを行うシングルトンクラス
/// </summary>
class EffectManager final
{
private:
	// シングルトン化
	EffectManager() {};
	EffectManager(const EffectManager&) = delete;
	void operator=(const EffectManager&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static EffectManager& GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>正常に初期化完了した場合はtrue、でなければfalse</returns>
	bool Init();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;
	/// <summary>
	/// 終了処理
	/// </summary>
	void Terminate();

	/// <summary>
	/// エフェクトの生成を行う
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="initPos">初期化位置</param>
	/// <returns>生成されたエフェクト(正常に生成できなかった場合はnullptr)</returns>
	std::weak_ptr<EffekseerEffect> GenerateEffect(const std::string& fileName, const Position3& initPos);
	/// <summary>
	/// 生成されているエフェクトの全削除を行う
	/// </summary>
	void DeleteAllEffect();

private:
	// 生成されたエフェクト
	std::forward_list<std::shared_ptr<EffekseerEffect>> _effects;

};

