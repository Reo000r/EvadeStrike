#include "Weapon.h"
#include "Library/Physics/Rigidbody.h"
#include "Library/Physics/ColliderData.h"
#include "Library/Physics/ColliderDataCapsule.h"
#include "library/Objects/Character/CharacterBase.h"
#include <cassert>

#include <DxLib.h>

Weapon::Weapon(std::weak_ptr<Physics> physics,
    PhysicsData::GameObjectTag tag) :
    AttackableGameObject(physics, PhysicsData::Priority::Static, tag, 
        PhysicsData::ColliderKind::Capsule,
        true, false, false),
    _modelHandle(-1)
{
}

Weapon::~Weapon()
{
    // モデル解放
    if (_modelHandle != -1) {
        MV1DeleteModel(_modelHandle);
    }
}

void Weapon::Init()
{
}

void Weapon::Update()
{
}

void Weapon::Draw() const
{
    // 描画
    MV1DrawModel(_modelHandle);
}

void Weapon::SetData(int modelHandle, float colRad, float colHeight, 
    Vector3 transOffset, Vector3 scale, Vector3 angle)
{
    assert(modelHandle >= 0 && "モデルハンドルが正しくない");
    _modelHandle = modelHandle;
    _scale = scale;

    _transOffset = transOffset;
    _rotAngle = angle;
    _scale = scale;

    // 当たり判定データ設定
    _collider->CreateColliderDataCapsule(
        colRad,	// 種別
        Vector3Up() * colHeight,	// StartToEnd
        true,	// isTrigger
        false	// isCollision
    );
}

void Weapon::PositionUpdate(Matrix4x4 parentWorldMatrix)
{
    // 各オフセットの行列を生成
    Matrix4x4 scaleMatrix = MatGetScale(_scale);
    Matrix4x4 rotY = MatRotateY(_rotAngle.y);
    Matrix4x4 rotX = MatRotateX(_rotAngle.x);
    Matrix4x4 rotZ = MatRotateZ(_rotAngle.z);
    Matrix4x4 rotationMatrix = MatMultiple(MatMultiple(rotZ, rotX), rotY);
    Matrix4x4 translationMatrix = MatTranslate(_transOffset);

    // 親の行列に対して補正値を合成する
    // 親 -> 平行移動 -> 回転 -> 拡縮の順
    Matrix4x4 worldMatrix = MatMultiple(
        scaleMatrix, MatMultiple(rotationMatrix,
            MatMultiple(translationMatrix, parentWorldMatrix)));

    // モデルに最終的なワールド行列を適用
    MV1SetMatrix(_modelHandle, worldMatrix);

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
    // 武器の向き(Y軸方向)をワールド座標系で取得
    Vector3 dir = Vector3(
        worldMatrix.m[1][0],
        worldMatrix.m[1][1],
        worldMatrix.m[1][2]
    ).Normalize();

    std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(_collider->GetColData());
    float dist = capsuleData->GetDist();
    float rad = capsuleData->GetRad();

    // 武器の向きと距離から、新しいoffsetベクトルを計算
    Vector3 newOffset = dir * dist;

    // 当たり判定のデータを更新
    capsuleData->SetStartToEnd(newOffset);
}

void Weapon::Enable(float attackPower, float breakPower, Vector3 knockbackVector)
{
    // 攻撃情報を設定
    EntryAttackStats(attackPower, breakPower, knockbackVector);
    // Colliderと自身(GameObject)を紐づける
    _collider->SetParent(shared_from_this());
    // 攻撃判定を有効化
    SetCollisionState(true);
}

void Weapon::Disable()
{
    // 攻撃判定の無効化
    SetCollisionState(false);
    // 攻撃情報の削除
    ClearAttackState();
    ReleaseAttackStats();
}

