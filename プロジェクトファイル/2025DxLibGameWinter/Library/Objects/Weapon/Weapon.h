#pragma once
#include "Library/Physics/Collider.h"
#include "Library/Objects/AttackableGameObject.h"
#include "Library/Geometry/Vector3.h"
#include "Library/Geometry/Matrix4x4.h"
#include <list>

class CharacterBase;

/// <summary>
/// 武器の基底クラス
/// </summary>
class Weapon abstract : public AttackableGameObject
{
public:
	Weapon(std::weak_ptr<Physics> physics,
		PhysicsData::GameObjectTag tag);
	virtual ~Weapon();

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
	void Draw() const override;

	/// <summary>
	/// 武器初期化
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	/// <param name="colRad">当たり判定半径</param>
	/// <param name="colHeight">当たり判定の高さ</param>
	/// <param name="transOffset">位置補正</param>
	/// <param name="scale">拡縮補正</param>
	/// <param name="angle">角度補正</param>
	void SetData(int modelHandle, float colRad,
		float colHeight,
		Vector3 transOffset = Vector3(),
		Vector3 scale = Vector3(1, 1, 1),
		Vector3 angle = Vector3());

	/// <summary>
	/// 武器更新
	/// </summary>
	/// <param name="parentWorldMatrix">ワールド行列とみなす行列</param>
	void PositionUpdate(Matrix4x4 parentWorldMatrix);

	/// <summary>
	/// 武器の有効化
	/// </summary>
	/// <param name="attackPower">攻撃力</param>
	/// <param name="breakPower">削り値</param>
	/// <param name="knockbackVector">ノックバックベクトル</param>
	void Enable(float attackPower, float breakPower, Vector3 knockbackVector);
	/// <summary>
	/// 武器の無効化
	/// </summary>
	void Disable();

protected:
	// モデルハンドル
	int _modelHandle;

	Vector3 _transOffset;
	Vector3 _rotAngle;
	Vector3 _scale;
};

