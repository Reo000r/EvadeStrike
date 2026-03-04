#include "AttackCol.h"
#include "Library/Physics/Rigidbody.h"
#include "Library/Physics/ColliderData.h"
#include "Library/Physics/ColliderDataCapsule.h"
#include "library/Objects/Character/CharacterBase.h"
#include <cassert>
#include <DxLib.h>

AttackCol::AttackCol(std::weak_ptr<Physics> physics,
    PhysicsData::GameObjectTag attackTag) :
	AttackableGameObject(physics, PhysicsData::Priority::Static,
		attackTag,
		PhysicsData::ColliderKind::Capsule,
		true, false, false)
{
}

AttackCol::~AttackCol()
{
}

void AttackCol::Init()
{
}

void AttackCol::Update()
{
}

void AttackCol::Draw() const
{
}

void AttackCol::SetAttackData(AttackColStats stats)
{
    _attackData = stats;
    
    // 当たり判定データがないなら
    if (_collider->GetColData() == nullptr) {
        // 当たり判定データ設定
        _collider->CreateColliderDataCapsule(
            _attackData.colRad,	// 半径
            Vector3Up() * _attackData.colHeight,	// StartToEnd
            true,	// isTrigger
            false	// isCollision
        );
    }
}

void AttackCol::SetColliderData(std::shared_ptr<ColliderData> stats)
{
    _collider->SetColData(stats);
}

void AttackCol::PositionUpdate(Matrix4x4 parentWorldMatrix)
{
    // 各オフセットの行列を生成
    Matrix4x4 scaleMatrix = MatGetScale(_attackData.scale);
    Matrix4x4 rotY = MatRotateY(_attackData.angle.y);
    Matrix4x4 rotX = MatRotateX(_attackData.angle.x);
    Matrix4x4 rotZ = MatRotateZ(_attackData.angle.z);
    Matrix4x4 rotationMatrix = MatMultiple(MatMultiple(rotZ, rotX), rotY);
    Matrix4x4 translationMatrix = MatTranslate(_attackData.transOffset);

    // 親の行列に対して補正値を合成する
    // 親 -> 平行移動 -> 回転 -> 拡縮の順
    Matrix4x4 worldMatrix = MatMultiple(
        scaleMatrix, MatMultiple(rotationMatrix,
            MatMultiple(translationMatrix, parentWorldMatrix)));

    // Rigidbodyの位置と当たり判定の向きを更新
    // (回転とスケールが適用される前の行列から計算)
    auto m1 = MatMultiple(translationMatrix, parentWorldMatrix);
    Position3 modelWorldPos = Vector3(
        m1.m[3][0],
        m1.m[3][1],
        m1.m[3][2]
    );
    _collider->SetPos(modelWorldPos);

    // 当たり判定の向きは回転まで適用した行列から取得
    // 向き(Y軸方向)をワールド座標系で取得
    Vector3 dir = Vector3(
        worldMatrix.m[1][0],
        worldMatrix.m[1][1],
        worldMatrix.m[1][2]
    ).Normalize();

    std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(_collider->GetColData());
    float dist = capsuleData->GetDist();
    float rad = capsuleData->GetRad();

    // 向きと距離から、新しいoffsetベクトルを計算
    Vector3 newOffset = dir * dist;

    // 当たり判定のデータを更新
    capsuleData->SetStartToEnd(newOffset);
}

void AttackCol::Enable()
{
	// 攻撃情報を設定
	EntryAttackStats(
        _attackData.attackPower, _attackData.breakPower, 
        _attackData.knockbackVel);
	// Colliderと自身(GameObject)を紐づける
	_collider->SetParent(shared_from_this());
	// 攻撃判定を有効化
	SetCollisionState(true);
}

void AttackCol::Disable()
{
	// 攻撃判定の無効化
	SetCollisionState(false);
	// 攻撃情報の削除
	ClearAttackState();
	ReleaseAttackStats();
}
