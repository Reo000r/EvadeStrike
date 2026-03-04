#pragma once
#include "Library/Objects/GameObject.h"

/// <summary>
/// ジャスト回避判定を行う当たり判定
/// </summary>
class JustDodgeCollider : public GameObject
{
public:
	JustDodgeCollider(std::weak_ptr<Physics> physics);
	~JustDodgeCollider();

	void Init() override;
	void Update() override;
	void Draw() const override;

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	void UpdatePosition(Position3 pos);

	void Enable();
	void Disable();

	bool GetCollisionState() const;

	void SetData(float rad, Vector3 offset);
	void SetPosOffset(Vector3 offset);
	bool IsHit() const;

private:
	bool _isHit;
	Vector3 _posOffset;
};

