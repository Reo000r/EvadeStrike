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

#include <cassert>
#include <vector>
#include <algorithm>

namespace {
	constexpr float kCheckUnder = -800.0f;
	constexpr float kCheckTop = 800.0f;
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
	_wall.clear();
	_floorAndRoof.clear();

	// 移動
	for (std::shared_ptr<Collider>& collider : _colliders) {
		// オブジェクトのタイムスケール
		float timeScale = 1.0f;
		// 正常な親が要ればタイムスケールを取得する
		if (collider->GetParent() != nullptr) {
			timeScale = collider->GetParent()->GetCurrentTimeScale();
		}
	}

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
		// MEMO:描画判定のみ半径分だけ上がっている為要修正
		// プレイヤーの見た目判定を違和感なくしようとした
		// 実際に判定を取るとモデルが半径分だけ浮く
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

				_wall.clear();
				_floorAndRoof.clear();
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
		// primaryとsecondaryがそれぞれ球かカプセルかを判別
		if (primary->_colliderData->GetKind() == PhysicsData::ColliderKind::Capsule) {
			capsuleObj = primary;
			polygonObj = secondary;
		}
		else {
			capsuleObj = secondary;
			polygonObj = primary;
		}

		// それぞれのColliderからデータを取得
		std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->_colliderData);
		std::shared_ptr<ColliderDataPolygon> polygonData = std::static_pointer_cast<ColliderDataPolygon>(polygonObj->_colliderData);

		// 連続で当たっているなら
		if (polygonData->IsContinuousCollision())
		{
			// 当たったポリゴンの情報
			auto lineHit = polygonData->GetLineHit();

			// 壁なのか床なのかを見る
			// 法線のY成分が大きければ床、小さければ壁
			if (abs(lineHit.Normal.y) < PhysicsData::kWallThreshold)
			{
				// 壁

				// 高さ
				float nextY = capsuleObj->CalcNextPos().y;

				// 次の位置
				Vector3 nextPos = lineHit.HitPosition;
				nextPos += Vector3(lineHit.Normal) * (capsuleData->GetRad() + PhysicsData::kFixPositionOffset);

				// 始点から終点へのベクトル
				Vector3 sToE = 
					capsuleData->GetEndPos(capsuleData->GetStartPos(capsuleObj->GetPos())) 
					- capsuleObj->GetPos();

				// 座標確定
				nextPos.y = nextY;
				capsuleObj->SetPos(nextPos);
				capsuleData->SetStartToEnd(sToE);
				// xzの移動量リセット
				capsuleObj->SetVel(Vector3(0,capsuleObj->GetVel().y, 0));
			}
			else
			{
				// 床

				if (lineHit.Normal.y > 0.0f)
				{
					// 次の位置
					Vector3 nextPos = lineHit.HitPosition;
					nextPos.x = 0.0f;
					nextPos.z = 0.0f;
					nextPos.y += capsuleData->GetRad() + PhysicsData::kFixPositionOffset;

					// 始点から終点へのベクトル
					Vector3 sToE = 
						capsuleData->GetEndPos(capsuleData->GetStartPos(capsuleObj->GetPos())) 
						- capsuleObj->GetPos();

					// Y座標確定
					nextPos.x = capsuleObj->GetPos().x;
					nextPos.z = capsuleObj->GetPos().z;
					capsuleObj->SetPos(nextPos);
					capsuleData->SetStartToEnd(sToE);
					// Y移動量リセット
					capsuleObj->SetVel(
						Vector3(
							capsuleObj->GetVel().x, 
							0.0f, 
							capsuleObj->GetVel().z));
				}
			}

			// 連続判定状態リセット
			polygonData->SetContinuousCollision(false);
		}
		else
		{
			// 当たったポリゴンの情報
			auto& hitDim = polygonData->GetHitDim();
			// お互い動かないオブジェクトなら衝突しない(ポリゴンはスタティックなので片方がスタティックなら)
			if (capsuleObj->GetPriority() == PhysicsData::Priority::Static)
			{
				// 検出したプレイヤーの周囲のポリゴン情報を開放する
				DxLib::MV1CollResultPolyDimTerminate(hitDim);
				return;
			}

			// カプセルの頭座標と足座標
			Position3 headPos = capsuleData->GetEndPos(capsuleObj->CalcNextPos());	// 移動後
			Position3 legPos = capsuleObj->CalcNextPos();	// 移動後
			// 頭より足のほうが低い位置にあるなら入れ替える
			if (headPos.y < legPos.y)
			{
				Position3 temp = legPos;
				legPos = headPos;
				headPos = temp;
			}
			float radius = capsuleData->GetRad();

			// 床ポリゴンと壁ポリゴンに分ける
			AnalyzeWallAndFloor(hitDim, legPos);

			// 床か天井に当たったか
			bool isFloorAndRoof = !_floorAndRoof.empty();
			// 壁に当たったか
			bool isWall = !_wall.empty();

			// 壁と当たっているなら
			if (isWall) {
				// 壁に当たっているので
				capsuleObj->SetIsWall(true);

				// 補正するベクトルを返す
				Vector3 overlapVec = HitWallCP(headPos, legPos, radius);

				// ベクトルを補正
				capsuleObj->SetPos(capsuleObj->GetPos() + overlapVec);

				// 座標が変わったので判定用の座標も更新
				legPos += overlapVec;
				headPos += overlapVec;

				// 速度に補正値を追加
				capsuleObj->AddVel(overlapVec);
			}
			// 床と当たったら
			if (isFloorAndRoof) {
				// 上昇中なら
				if (capsuleObj->GetVel().y > 0.0f) {
					//天井に当たった処理
					HitRoofCP(capsuleObj, headPos, radius);
				}
				else {
					// 床の高さに合わせる
					HitFloorCP(capsuleObj, legPos, headPos, radius);
				}
			}
			


			// 検出したプレイヤーの周囲のポリゴン情報を開放する
			DxLib::MV1CollResultPolyDimTerminate(hitDim);
		}

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

		//collider->_rigidbody->SetVel(toFixedPos);

		// 位置確定
		// MEMO:本来はここでCollider側に位置更新処理を行わせる関数を呼び、
		// モデルなどの位置更新なども一緒に行わせたい
		collider->_rigidbody->SetPos(collider->CalcNextPos());


		// 
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
			tag == PhysicsData::GameObjectTag::Enemy) {
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

void Physics::AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, Vector3 nextPos)
{
	// 検出されたポリゴンの数だけ繰り返す
	for (int i = 0; i < hitDim.HitNum; ++i) {
		// 法線のY成分が大きければ床、小さければ壁
		if (abs(hitDim.Dim[i].Normal.y) < PhysicsData::kWallThreshold) {
			// 壁ポリゴンと判断された場合でも、プレイヤーのY座標＋1.0fより高いポリゴンのみ当たり判定を行う
			// 段さで突っかかるのを防ぐため
			if (hitDim.Dim[i].Position[0].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[1].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[2].y > nextPos.y + 1.0f) {
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に保存する
				if (_wall.size() < PhysicsData::kMaxHitPolygon) {
					// ポリゴンの構造体のアドレスを壁ポリゴン配列に保存
					_wall.emplace_back(hitDim.Dim[i]);
				}
			}
		}
		// 床ポリゴン
		else {
			// ポリゴンの数が列挙できる限界数に達していなかったらポリゴン配列に保存
			if (_floorAndRoof.size() < PhysicsData::kMaxHitPolygon) {
				// ポリゴンの構造体のアドレスを床ポリゴン配列に保存
				_floorAndRoof.emplace_back(hitDim.Dim[i]);
			}
		}
	}
}

Vector3 Physics::HitWallCP(const Vector3& headPos, const Vector3& legPos, float shortDis)
{
	// 垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;	// 最短距離

	Vector3 top = headPos;
	top.y += shortDis;
	Vector3 bot = legPos;
	bot.y -= shortDis;

	// 法線
	Vector3 nom = {};
	for (auto& wall : _wall) {
		// 壁かチェック
		if (abs(wall.Normal.y) >= PhysicsData::kWallThreshold)	continue;
		VECTOR pos1 = wall.Position[0];
		VECTOR pos2 = wall.Position[1];
		VECTOR pos3 = wall.Position[2];
		// 最短距離の2乗を返す
		float dis = Segment_Triangle_MinLength_Square(top, bot, pos1, pos2, pos3);
		// 平方根を返す
		dis = sqrtf(dis);
		// 初回または前回より距離が短いなら
		if (hitShortDis > dis) {
			// 現状の最短
			hitShortDis = dis;
			// 法線
			nom = wall.Normal;
		}
	}
	// 押し戻し
	// どれくらい押し戻すか
	float overlap = shortDis - hitShortDis;
	overlap = std::clamp<float>(overlap, 0, shortDis);
	overlap += PhysicsData::kFixPositionOffset;
	// 正規化
	if (nom.Magnitude() != 0.0f) {
		nom = nom.Normalize();
	}
	return nom * overlap;
}

bool Physics::HitFloorCP(const std::shared_ptr<Collider> other, const Vector3& legPos, const Vector3& headPos, float shortDis)
{
	// 足の真下にある床の中で最も高いY
	float finalFloorY = -FLT_MAX;
	bool hitFloor = false;

	// 足の真下レイ(下方向)
	VECTOR rayStart = legPos;
	VECTOR rayEnd = VAdd(rayStart, VGet(0.0f, kCheckUnder, 0.0f));

	for (auto& floor : _floorAndRoof) {
		// 下向き法線なら床ではない
		if (floor.Normal.y < 0.0f) continue;

		VECTOR pos1 = floor.Position[0];
		VECTOR pos2 = floor.Position[1];
		VECTOR pos3 = floor.Position[2];

		// 足の真下にあるポリゴンと交差チェック
		HITRESULT_LINE res = HitCheck_Line_Triangle(rayStart, rayEnd, pos1, pos2, pos3);

		if (res.HitFlag) {
			float hitPosY = res.Position.y;

			// より高い床を優先する(坂道で安定)
			if (hitPosY > finalFloorY) {
				finalFloorY = hitPosY;
				hitFloor = true;
			}
		}
	}

	// 足の真下で床が見つかった
	if (hitFloor) {
		float newY = finalFloorY + shortDis + PhysicsData::kFixPositionOffset;

		other->SetPosY(newY);
		other->SetVelY(0.0f);
		other->SetIsFloor(true);

		return true;
	}

	// 足の真下に床が無かった場合キャラが坂の腹に刺さっている可能性あり
	// headからleg のラインが床を貫通していないかチェック
	float betweenY = -FLT_MAX;
	bool hitBetween = false;

	VECTOR head = headPos;

	for (auto& floor : _floorAndRoof) {
		if (floor.Normal.y < 0.0f)	continue;

		VECTOR pos1 = floor.Position[0];
		VECTOR pos2 = floor.Position[1];
		VECTOR pos3 = floor.Position[2];

		HITRESULT_LINE res = 
			HitCheck_Line_Triangle(head, rayStart, pos1, pos2, pos3);

		if (res.HitFlag) {
			if (res.Position.y > betweenY) {
				// 間の床のY座標を保存
				betweenY = res.Position.y;
				hitBetween = true;
			}
		}
	}

	// headとlegの間で床が見つかった
	if (hitBetween) {
		float newY = betweenY + shortDis + PhysicsData::kFixPositionOffset;

		other->SetPosY(newY);
		other->SetVelY(0.0f);
		other->SetIsFloor(true);
		return true;
	}

	return false;
}

void Physics::HitRoofCP(const std::shared_ptr<Collider> other, const Vector3& headPos, float shortDis)
{
	// 垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;	// 最短距離
	// 当たった中で足元に一番近いY座標に合わせる
	float lowHitPosY = other->GetPos().y;
	// 天井と当たったか
	bool isHitRoof = false;
	for (auto& roof : _floorAndRoof) {
		// 上向きの法線ベクトルなら飛ばす
		if (roof.Normal.y > 0.0f)	continue;
		// 頭の上にポリゴンがあるかをチェック
		HITRESULT_LINE lineResult = 
			HitCheck_Line_Triangle(headPos, 
				headPos + Vector3(0.0f, kCheckTop, 0.0f),
			roof.Position[0], roof.Position[1], roof.Position[2]);

		if (lineResult.HitFlag) {
			// 距離
			float dis = VSize(VSub(lineResult.Position, headPos));
			// 初回または前回より距離が短いなら
			if (hitShortDis > dis) {
				isHitRoof = true;
				// 現状の最短
				hitShortDis = dis;
			}
		}
	}
	// 当たったいるなら
	if (isHitRoof) {
		// 押し戻し
		// どれくらい押し戻すか
		float overlap = std::abs(shortDis - hitShortDis);
		overlap = std::clamp<float>(overlap, 0, shortDis);
		overlap += PhysicsData::kFixPositionOffset;
		// 法線
		Vector3 nom = Vector3(0.0f, -1.0f, 0.0f);
		// 力を与える
		other->AddVel(nom * overlap);
	}
}
