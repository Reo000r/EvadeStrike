#pragma once
#include "UIBase.h"
#include <memory>

class EnemyBoss;

class UIBossHP : public UIBase
{
public:

	UIBossHP(int baseGraphHandle,
		int gaugeGraphHandle,
		int decGaugeGraphHandle);
	~UIBossHP();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Vector2 shakeOffset) override;

	void SetBoss(std::weak_ptr<EnemyBoss> boss) { _boss = boss; }
	void SetMaxHP(float maxHp) { _maxHp = maxHp; }

private:

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnim();
	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimDec();

	void DrawHPBase(Position2 drawPos) const;
	void DrawHPGauge(Position2 drawPos, float ratio) const;
	void DrawHPGaugeDec(Position2 drawPos, float ratio);

	int _baseGraphHandle;
	int _gaugeGraphHandle;
	int _decGaugeGraphHandle;

	float _scale;

	std::weak_ptr<EnemyBoss> _boss;
	float _maxHp;
	float _lastHp;
	float _animStartHp;
	// アニメーションの残りフレーム
	int _shakeAnimFrame;
	int _decAnimFrame;
	float _decAnimProgress;
};

