#include "Physics.h"
#include "Collider.h"
#include "ColliderData.h"
#include "ColliderDataSphere.h"
#include "ColliderDataCapsule.h"
#include "ColliderDataPolygon.h"
#include "Rigidbody.h"
#include "Library/System/DebugDraw.h"
#include "Collision.h"
#include "Library/Objects/GameObject.h"
#include "Library/Objects/Character/CharacterBase.h"
#include "Library/Geometry/Vector2.h"
#include "Library/System/Statistics.h"

#include <cassert>
#include <vector>
#include <algorithm>

namespace {
	constexpr float kCheckUnder = -800.0f;
	constexpr float kCheckTop = 800.0f;

	// fps許容最低値
	// デバッグログで使用
	constexpr int kLowestFPS = Statistics::kFPS * 0.5f;
}

Physics::Physics() :
	_colliders()
{
}

void Physics::Entry(std::shared_ptr<Collider> collider)
{
	// 見つからなければ(登録されていなければ)
	if (!IsRegistered(collider)) {
		_colliders.emplace_front(collider);	// 登録
	}
	// 既に登録されていたらassert
	else {
		assert(false && "指定のcolliderは登録済");
	}
}

void Physics::Release(std::shared_ptr<Collider> collider)
{
	// 登録解除(eraseif 要C++20)
	size_t count = std::erase_if(
		_colliders,
		[collider](std::shared_ptr<Collider> target) { return target == collider; });
	// 登録されてなかったらassert
	if (count <= 0) {
		assert(false && "指定のcolliderは未登録");
	}
}

void Physics::Update()
{
	LONGLONG startTime = GetNowHiPerformanceCount();

	// 重力適用
	ApplyGravity();

	for (std::shared_ptr<Collider>& collider : _colliders) {
		Position3 pos = collider->GetPos();
		Vector3 vel = collider->GetVel();
		// 移動量切り捨て処理
		Vector2 velXZ = vel.XZ();
		// XZのみを見て閾値よりも小さければ
		if (velXZ.Magnitude() < PhysicsData::kSleepThreshold) {
			vel.x = vel.z = 0.0f;
		}

		// もともとの情報、予定情報をデバッグ表示
#ifdef _DEBUG
		DebugCollisionDraw(collider, pos);
#endif

		// 移動量再設定
		collider->SetVel(vel);
		//collider->_nextPos = collider->GetPos() + vel * timeScale;

		// 床と壁のフラグを初期化
		collider->SetIsFloor(false);
		collider->SetIsWall(false);
	}

	// 当たり判定チェック（nextPos指定）
	std::forward_list<OnCollideInfo> onCollideInfo = CheckCollide();

	// 位置確定
	FixPosition();

	// 当たり通知
	for (OnCollideInfo& info : onCollideInfo) {
		info.owner->OnCollide(info.colider);
	}

	int diffTime = static_cast<int>(GetNowHiPerformanceCount() - startTime);
	// FPSが想定値より低ければ
	if (GetFPS() <= kLowestFPS) {
		// Physics内で生じたラグを表示
		printf("FPS:%.01f PhysicsDiff:%d\n", GetFPS(), diffTime);
	}
}

void Physics::DebugCollisionDraw(std::shared_ptr<Collider>& collider, Position3& pos)
{
	// もともとの情報、予定情報をデバッグ表示
#ifdef _DEBUG
	int color = 0xff00ff;
	// 当たらない場合は色を変える
	if (!(collider->_colliderData->_isCollision)) {
		color = 0xffffff;
	}
	// 球
	if (collider->_colliderData->GetKind() == PhysicsData::ColliderKind::Sphere) {
		std::shared_ptr<ColliderDataSphere> sphereData = std::static_pointer_cast<ColliderDataSphere>(collider->_colliderData);
		float radius = sphereData->GetRad();
		DebugDraw::GetInstance().DrawSphere(pos, radius, color);
	}
	// カプセル
	if (collider->_colliderData->GetKind() == PhysicsData::ColliderKind::Capsule) {
		std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(collider->_colliderData);
		Position3 pos = collider->_rigidbody->GetPos();
		float radius = capsuleData->GetRad();
		Position3 start = capsuleData->GetStartPos(pos);
		Position3 end = capsuleData->GetEndPos(pos);
		DebugDraw::GetInstance().DrawSphere(start, radius, color);
		DebugDraw::GetInstance().DrawSphere(end, radius, color);
		DebugDraw::GetInstance().DrawCapsule(start, end, radius, color);
	}
#endif
}

bool Physics::IsRegistered(std::shared_ptr<Collider> collider)
{
	// (見つからなかった場合はend)
	auto it = (
		std::find(
			_colliders.begin(),
			_colliders.end(),
			collider));
	return (it != _colliders.end());
}

std::forward_list<Physics::OnCollideInfo> Physics::CheckCollide()
{
	std::forward_list<OnCollideInfo> onCollideInfo;
	// 衝突通知、ポジション補正
	bool doCheck = true;
	int	checkCount = 0;	// チェック回数
	while (doCheck) {
		doCheck = false;
		++checkCount;

		// 全オブジェクト当たり判定
		for (const std::shared_ptr<Collider>& objA : _colliders) {
			// 当たり判定を行わないなら
			if (!objA->GetColData()->IsCollision()) {
				continue;	// 次へ
			}

			for (const std::shared_ptr<Collider>& objB : _colliders) {

				// 同じオブジェクトであればスキップ
				if (objA == objB) continue;
				// 当たり判定を行わないなら
				if (!objB->GetColData()->IsCollision()) {
					continue;	// 次へ
				}
				
				PhysicsData::Priority priorityA = objA->GetPriority();
				PhysicsData::Priority priorityB = objB->GetPriority();

				// ぶつかっていれば
				if (IsCollide(objA, objB)) {
					
					// 衝突通知情報の更新
					// 登録されていなければ登録
					OnCollideInfo infoA = { objA, objB };
					if (std::find(onCollideInfo.begin(), onCollideInfo.end(), infoA) == onCollideInfo.end()) {
						onCollideInfo.emplace_front(infoA);
					}
					OnCollideInfo infoB = { objB, objA };
					if (std::find(onCollideInfo.begin(), onCollideInfo.end(), infoB) == onCollideInfo.end()) {
						onCollideInfo.emplace_front(infoB);
					}
					
					std::shared_ptr<Collider> primary = objA;
					std::shared_ptr<Collider> secondary = objB;

					// どちらもトリガーでなければ次目標位置修正
					// どちらかがトリガーなら補正処理を飛ばす
					bool isTriggerAorB = (
						objA->_colliderData->IsTrigger() ||
						objB->_colliderData->IsTrigger());
					if (!isTriggerAorB) {
						// 移動優先度を数字に直したときに高い方を移動
						if (priorityA > priorityB) {
							primary = objB;
							secondary = objA;
						}
						// どちらも動かないものでなければ
						if (priorityA != PhysicsData::Priority::Static ||
							priorityB != PhysicsData::Priority::Static) {
							// 位置補正を行う
							// priorityが同じだった場合は両方押し戻す
							FixNextPosition(primary, secondary, (priorityA == priorityB));
						}

						// 一度でも衝突+補正したら衝突判定と補正やりなおし
						// 片方がトリガーなら衝突判定は行わない
						doCheck = true;
						break;
					}
				}
			}
			if (doCheck) {
				break;
			}
		}

		// 無限ループ避け
		if (checkCount > PhysicsData::kCheckCollideMaxCount && doCheck) {
#if _DEBUG
			//printfDx("当たり判定チェック回数が最大数(%d)を超えた\n",
			//	PhysicsData::kCheckCollideMaxCount);
#endif
			break;
		}
	}
	return onCollideInfo;
}

bool Physics::IsCollide(const std::shared_ptr<Collider> objA, const std::shared_ptr<Collider> objB) const
{
	bool isHit = false;

	// Colliderの種類によって、当たり判定を分ける
	PhysicsData::ColliderKind aKind = objA->_colliderData->GetKind();
	PhysicsData::ColliderKind bKind = objB->_colliderData->GetKind();

	PhysicsData::GameObjectTag aTag = objA->GetTag();
	PhysicsData::GameObjectTag bTag = objB->GetTag();

	// どちらかが相手のタグを無視する設定になっていたらreturn
	if (objA->_colliderData->IsThroughTarget(bTag) ||
		objB->_colliderData->IsThroughTarget(aTag)) return false;
	// どちらかが当たり判定を行わない設定になっていたらreturn
	if (!objA->_colliderData->_isCollision ||
		!objB->_colliderData->_isCollision) return false;

	// 球同士
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere) {
		std::shared_ptr<ColliderDataSphere> sphereA = std::static_pointer_cast<ColliderDataSphere>(objA->_colliderData);
		std::shared_ptr<ColliderDataSphere> sphereB = std::static_pointer_cast<ColliderDataSphere>(objB->_colliderData);
		isHit = CheckHitSphereSphere(sphereA, sphereB, objA->CalcNextPos(), objB->CalcNextPos());
	}
	// カプセル同士
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule) {
		std::shared_ptr<ColliderDataCapsule> capsuleA = std::static_pointer_cast<ColliderDataCapsule>(objA->_colliderData);
		std::shared_ptr<ColliderDataCapsule> capsuleB = std::static_pointer_cast<ColliderDataCapsule>(objB->_colliderData);
		isHit = CheckHitCapsuleCapsule(capsuleA, capsuleB, objA->CalcNextPos(), objB->CalcNextPos());
	}
	// 球とカプセル
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) ||
		(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere)) {
		// Aを球、Bをカプセルとする
		std::shared_ptr<Collider> sphereObj = objA;
		std::shared_ptr<Collider> capsuleObj = objB;
		// objAがカプセルであれば入れ替える
		if (aKind == PhysicsData::ColliderKind::Capsule) {
			sphereObj = objB;
			capsuleObj = objA;
		}
		// それぞれのコライダー情報を取得
		std::shared_ptr<ColliderDataSphere> sphereData = std::static_pointer_cast<ColliderDataSphere>(sphereObj->_colliderData);
		std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->_colliderData);
		isHit = CheckHitSphereCapsule(sphereData, capsuleData, sphereObj->CalcNextPos(), capsuleObj->CalcNextPos());
	}
	// 球とポリゴン
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Polygon) ||
		(aKind == PhysicsData::ColliderKind::Polygon && bKind == PhysicsData::ColliderKind::Sphere)) {

	}
	// カプセルとポリゴン
	else if ((aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Polygon) ||
		(aKind == PhysicsData::ColliderKind::Polygon && bKind == PhysicsData::ColliderKind::Capsule)) {

		////コライダーデータの取得
		//auto collDataA = std::static_pointer_cast<ColliderDataCapsule>(collA->m_collisionData);
		//auto collDataB = std::static_pointer_cast<ColliderDataPolygon>(collB->m_collisionData);
		// Aをカプセル、Bをポリゴンとする
		std::shared_ptr<Collider> capsuleObj = objA;
		std::shared_ptr<Collider> polygonObj = objB;
		// objAがポリゴンであれば入れ替える
		if (aKind == PhysicsData::ColliderKind::Polygon) {
			capsuleObj = objB;
			polygonObj = objA;
		}
		isHit = CheckHitCapsulePolygon(capsuleObj, polygonObj);
	}
	// ポリゴン同士
	else if (aKind == PhysicsData::ColliderKind::Polygon && bKind == PhysicsData::ColliderKind::Polygon) {
		// 未定義
	}
	else {
		//assert(false && "判定が定義されていないオブジェクトの組");
	}
	return isHit;
}

void Physics::FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary, bool isMutualPushback)
{
	// collidableの種類によって、当たり判定を分ける
	PhysicsData::ColliderKind aKind = primary->_colliderData->GetKind();
	PhysicsData::ColliderKind bKind = secondary->_colliderData->GetKind();

	// 球同士
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere) {
		FixNextPosSphereSphere(primary, secondary, isMutualPushback);
	}
	// カプセル同士
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule) {
		// 当たり判定データ取得
		std::shared_ptr<ColliderDataCapsule> priCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(primary->_colliderData);
		std::shared_ptr<ColliderDataCapsule> secCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(secondary->_colliderData);
		// primaryカプセルの情報を取得
		Position3 priStart = priCapsuleData->GetStartPos(primary->CalcNextPos());
		Position3 priEnd = priCapsuleData->GetEndPos(primary->CalcNextPos());
		float priRadius = priCapsuleData->GetRad();

		// secondaryカプセルの情報を取得
		Position3 secStart = secCapsuleData->GetStartPos(secondary->CalcNextPos());
		Position3 secEnd = secCapsuleData->GetEndPos(secondary->CalcNextPos());
		float secRadius = secCapsuleData->GetRad();

		// 最近傍点の計算
		// 2つのカプセルの中心線上で最も近い点(pPri, pSec)を計算
		Position3 pPri, pSec;
		ClosestPointSegments(priStart, priEnd, secStart, secEnd, pPri, pSec);

		// 押し戻し方向の決定
		// 最近傍点間のベクトルを計算し、押し戻し方向を決定
		Vector3 pushBackVec = pSec - pPri;
		// 距離がゼロに近い場合は、カプセルの中心位置から方向を仮決めする（めり込みきっている場合など）
		if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
			pushBackVec = secondary->CalcNextPos() - primary->CalcNextPos();
		}
		pushBackVec.Normalized();

		// 押し戻し距離(貫通深度)の計算
		// 最近傍点間の現在の距離を計算
		float currentDist = (pSec - pPri).Magnitude();
		// 2つのカプセルの半径の合計
		float radiusSum = priRadius + secRadius;
		// 貫通深度にオフセットを加えた、最終的な押し戻し距離を計算
		float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

		// 位置の修正
		// 計算した方向と距離を使って、カプセルの位置を修正
		Vector3 fixVec = pushBackVec * pushBackDist;

		// 優先度が同じでお互いに押し戻す場合
		if (isMutualPushback) {
			// 押し戻し量を半分ずつに分ける
			Vector3 halfFixVec = fixVec * 0.5f;
			primary->AddVel(-halfFixVec);
			secondary->AddVel(halfFixVec);
		}
		// secondaryのみを押し戻す場合
		else {
			secondary->AddVel(fixVec);
		}
	}
	// 球とカプセル
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) ||
		(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere)) {
		// 当たり判定データ取得
		std::shared_ptr<Collider> sphereObj;
		std::shared_ptr<Collider> capsuleObj;
		// primaryとsecondaryがそれぞれ球かカプセルかを判別
		if (primary->_colliderData->GetKind() == PhysicsData::ColliderKind::Sphere) {
			sphereObj = primary;
			capsuleObj = secondary;
		}
		else {
			sphereObj = secondary;
			capsuleObj = primary;
		}

		// それぞれのColliderからデータを取得
		std::shared_ptr<ColliderDataSphere> sphereData = std::static_pointer_cast<ColliderDataSphere>(sphereObj->_colliderData);
		std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->_colliderData);

		// 球の情報を取得
		Vector3 sphereCenter = sphereObj->CalcNextPos();
		float sphereRadius = sphereData->GetRad();

		// カプセルの情報を取得
		Vector3 capsuleStart = capsuleData->GetStartPos(capsuleObj->CalcNextPos());
		Vector3 capsuleEnd = capsuleData->GetEndPos(capsuleObj->CalcNextPos());
		float capsuleRadius = capsuleData->GetRad();

		// 最近傍点の計算
		// 球の中心とカプセルの中心線との最近傍点を計算
		Vector3 closestPointOnCapsuleAxis = ClosestPointPointAndSegment(sphereCenter, capsuleStart, capsuleEnd);

		// 押し戻し方向の決定
		// カプセルの最近傍点から球の中心へ向かうベクトルを、押し戻し方向とする
		Vector3 pushBackVec = sphereCenter - closestPointOnCapsuleAxis;
		// 距離がゼロに近い場合は、オブジェクトの中心位置から方向を仮決めする
		if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
			pushBackVec = sphereObj->CalcNextPos() - capsuleObj->CalcNextPos();
		}
		pushBackVec.Normalized();

		// 押し戻し距離(貫通深度)の計算
		// 最近傍点間の現在の距離を計算
		float currentDist = (sphereCenter - closestPointOnCapsuleAxis).Magnitude();
		// 2つのオブジェクトの半径の合計
		float radiusSum = sphereRadius + capsuleRadius;
		// 貫通深度にオフセットを加えた、最終的な押し戻し距離を計算
		float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

		// 位置の修正
		// 計算した方向と距離を使って、オブジェクトの位置を修正
		Vector3 fixVec = pushBackVec * pushBackDist;

		// 優先度が同じでお互いに押し戻す場合
		if (isMutualPushback) {
			// 押し戻し量を半分ずつに分け、それぞれを押し戻す
			Vector3 halfFixVec = fixVec * 0.5f;
			sphereObj->AddVel(halfFixVec);	// 球を押し戻し
			capsuleObj->AddVel(-halfFixVec);	// カプセルを押し戻し
		}
		// 優先度に従って片方のみを押し戻す場合
		else {
			// 優先度の低い方(secondary)を押し戻す
			// (fixVecはカプセルから球へのベクトルのため、足し引きを使い分ける)
			if (secondary == sphereObj) {
				secondary->AddVel(fixVec);	// secondary(球)を押し戻す
			}
			else {	// secondary == capsuleObj
				secondary->AddVel(-fixVec);	// secondary(カプセル)を押し戻す
			}
		}
	}
	// 球とポリゴン
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) ||
		(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere)) {
		// 


	}
	// カプセルとポリゴン
	else if ((aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Polygon) ||
		(aKind == PhysicsData::ColliderKind::Polygon && bKind == PhysicsData::ColliderKind::Capsule)) {
		
		// 当たり判定データ取得
		std::shared_ptr<Collider> capsuleObj;
		std::shared_ptr<Collider> polygonObj;
		// primaryとsecondaryがそれぞれカプセルかポリゴンかを判別
		if (primary->_colliderData->GetKind() == PhysicsData::ColliderKind::Capsule) {
			capsuleObj = primary;
			polygonObj = secondary;
		}
		else {
			capsuleObj = secondary;
			polygonObj = primary;
		}

		FixNextPosCapsulePolygon(capsuleObj, polygonObj, isMutualPushback);

	}
	else {
		assert(false && "判定が定義されていないオブジェクトの組");
	}

	return;
}

void Physics::FixPosition()
{
	for (std::shared_ptr<Collider>& collider : _colliders) {
		Vector3 toFixedPos;
		// オブジェクトのタイムスケール
		float timeScale = 1.0f;		// デフォルト値
		// 正常な親がいればタイムスケール取得
		if (collider->GetParent() != nullptr) {
			timeScale = collider->GetParent()->GetCurrentTimeScale();
		}

		// Posを更新するので、velocityもそこに移動するvelocityに修正
		toFixedPos = collider->CalcNextPos() - collider->_rigidbody->GetPos();

		// 位置確定
		collider->_rigidbody->SetPos(collider->CalcNextPos());

		float y = collider->_rigidbody->GetVel().y;
		// 接地していたら
		if (collider->CalcNextPos().y <= 0.0f) {
			y = 0.0f;
		}
		collider->_rigidbody->SetVel(Vector3(0, y, 0));
	}
}

void Physics::ApplyGravity()
{
	for (auto& col : _colliders) {
		//重力を受けるか
		if (!col->UseGravity())continue;
		// 親がいるか
		if (col->GetParent() == nullptr) continue;
		auto tag = col->GetParent()->GetTag();

		Vector3 baseGravity = PhysicsData::Gravity;
		// キャラクターであれば
		if (tag == PhysicsData::GameObjectTag::Player ||
			tag == PhysicsData::GameObjectTag::EnemyMinion ||
			tag == PhysicsData::GameObjectTag::EnemyBoss) {
			std::shared_ptr<CharacterBase> character =
				std::static_pointer_cast<CharacterBase>(col->GetParent());
			// 接地しているかどうか
			if (character->IsGround()) {
				baseGravity *= 20.0f;
			}
		}

		Vector3 vel = col->GetVel();

		// オブジェクトのタイムスケール
		float timeScale = 1.0f;
		// 正常な親が要ればタイムスケールを取得する
		if (col->GetParent() != nullptr) {
			timeScale = col->GetParent()->GetCurrentTimeScale();
		}

		// タイムスケールを加味した重力を加える
		vel += baseGravity * col->GetGravityScale()* timeScale;

		// 最大重力加速度より小さかったら補正
		// (重力はマイナスのため)
		if (vel.y < PhysicsData::MaxGravityAccel.y) {
			vel.y = PhysicsData::MaxGravityAccel.y;
		}

		col->SetVel(vel);
	}
}
