#pragma once
#include "UIBase.h"
#include <memory>

class Player;

class UIPlayerHP : public UIBase
{
public:

	UIPlayerHP(int baseGraphHandle, 
		int gaugeGraphHandle,
		int decGaugeGraphHandle);
	~UIPlayerHP();

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

	void SetPlayer(std::weak_ptr<Player> player) { _player = player; }
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

	std::weak_ptr<Player> _player;
	float _maxHp;
	float _lastHp;
	float _animStartHp;
	// アニメーションの残りフレーム
	int _shakeAnimFrame;
	int _decAnimFrame;
	float _decAnimProgress;
};

