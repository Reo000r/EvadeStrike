#include "Collision.h"
#include "ProjectSettings.h"
#include "Library/Physics/ColliderDataSphere.h"
#include "Library/Physics/ColliderDataCapsule.h"

#include <algorithm>

namespace {
	constexpr float kCheckUnder = -800.0f;
	constexpr float kCheckTop = 800.0f;
}

bool CheckHitSphereSphere(
	const std::shared_ptr<ColliderDataSphere> sphereA,
	const std::shared_ptr<ColliderDataSphere> sphereB,
	Position3 nextAPos, Position3 nextBPos)
{
	Vector3 atob = nextBPos - nextAPos;
	float atobLength = atob.Magnitude();

	// お互いの距離が、それぞれの半径を足したものより小さければ当たる
	return (atobLength < sphereA->GetRad() + sphereB->GetRad());
}

bool CheckHitCapsuleCapsule(
	const std::shared_ptr<ColliderDataCapsule> capsuleA, 
	const std::shared_ptr<ColliderDataCapsule> capsuleB, 
	Position3 nextAPos, Position3 nextBPos)
{
	// カプセルAの線分と半径
	Vector3 startA = capsuleA->GetStartPos(nextAPos);
	Vector3 endA = capsuleA->GetEndPos(nextAPos);
	float radiusA = capsuleA->GetRad();

	// カプセルBの線分と半径
	Vector3 startB = capsuleB->GetStartPos(nextBPos);
	Vector3 endB = capsuleB->GetEndPos(nextBPos);
	float radiusB = capsuleB->GetRad();

	// 2つの線分の最近接点を求める
	Vector3 pA, pB;
	ClosestPointSegments(startA, endA, startB, endB, pA, pB);

	// 最近接点間の距離の2乗を計算
	float distSq = (pA - pB).SqrMagnitude();
	float radSum = radiusA + radiusB;

	// 最近接点間の距離が、半径の合計より小さいかどうかで衝突を判定
	return (distSq < (radSum * radSum));
}

bool CheckHitSphereCapsule(
	const std::shared_ptr<ColliderDataSphere> sphere, 
	const std::shared_ptr<ColliderDataCapsule> capsule, 
	Position3 nextSpherePos, Position3 nextCapsulePos)
{
	// 球の情報を取得
	Vector3 sphereCenter = nextSpherePos;
	float sphereRadius = sphere->GetRad();

	// カプセルの情報を取得
	Vector3 capsuleStart = capsule->GetStartPos(nextCapsulePos);
	Vector3 capsuleEnd = capsule->GetEndPos(nextCapsulePos);
	float capsuleRadius = capsule->GetRad();

	// 点と線分の最近接点を求める
	Vector3 closestPointOnCapsuleAxis =
		ClosestPointPointAndSegment(
			sphereCenter,
			capsuleStart, capsuleEnd);

	// 最近接点間の距離の2乗を計算
	float distSq = (sphereCenter - closestPointOnCapsuleAxis).SqrMagnitude();
	// 半径の合計を計算
	float radSum = sphereRadius + capsuleRadius;

	// 距離が半径の合計より小さいか判定
	return (distSq < (radSum * radSum));
}

bool CheckHitCapsulePolygon(
	const std::shared_ptr<Collider> capsule, 
	const std::shared_ptr<Collider> polygon)
{
	// それぞれのColliderからデータを取得
	std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsule->GetColData());
	std::shared_ptr<ColliderDataPolygon> polygonData = std::static_pointer_cast<ColliderDataPolygon>(polygon->GetColData());

	//当たってるポリゴンの数
	auto hitDim = MV1CollCheck_Capsule(
		polygonData->GetModelHandle(),
		-1,
		capsule->CalcNextPos(),
		capsuleData->GetEndPos(capsule->CalcNextPos()),
		capsuleData->GetRad(),
		-1);

	// 当たっていないなら
	if (hitDim.HitNum <= 0)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);

		// 連続衝突判定
		auto startLineHitDim = MV1CollCheck_Line(
			polygonData->GetModelHandle(),
			-1,
			capsule->GetPos(),
			capsule->CalcNextPos()
		);
		// 当たったのであれば
		if (startLineHitDim.HitFlag) {
			// 連続衝突をした
			polygonData->SetContinuousCollision(true);

			// 当たり判定に使うので保存
			polygonData->SetLineHit(startLineHitDim);

			return true;
		}
		else {
			auto endLineHitDim = MV1CollCheck_Line(
				polygonData->GetModelHandle(),
				-1,
				capsuleData->GetEndPos(capsule->GetPos()),
				capsuleData->GetEndPos(capsule->CalcNextPos())
			);

			if (endLineHitDim.HitFlag) {
				// 連続衝突した
				polygonData->SetContinuousCollision(true);

				// 当たり判定に使うので保存
				polygonData->SetLineHit(endLineHitDim);

				return true;
			}
			else {
				// 当たってない
				return false;
			}
		}
	}
	if (capsuleData->IsTrigger()) {
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
	else {
		// 当たり判定用に保存
		polygonData->SetHitDim(hitDim);
	}

	return true;
}

void FixNextPosSphereSphere(const std::shared_ptr<Collider> priSphere, const std::shared_ptr<Collider> secSphere, bool isMutualPushback)
{
	// 当たり判定データ取得
	std::shared_ptr<ColliderDataSphere> priColliderData = std::static_pointer_cast<ColliderDataSphere>(priSphere->GetColData());
	std::shared_ptr<ColliderDataSphere> secColliderData = std::static_pointer_cast<ColliderDataSphere>(secSphere->GetColData());

	// 押し戻し方向の決定
	// secondaryからprimaryへ向かうベクトルを計算し、正規化する
	Vector3 pushBackVec = priSphere->CalcNextPos() - secSphere->CalcNextPos();
	// 距離がゼロに近い場合は、押し戻し方向が不定になるため処理をスキップ
	if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
		return;
	}
	pushBackVec.Normalized();

	// 押し戻し距離(貫通深度)の計算
	// 現在の中心間の距離を計算
	float currentDist = (priSphere->CalcNextPos() - secSphere->CalcNextPos()).Magnitude();
	// 2つの球の半径の合計
	float radiusSum = priColliderData->GetRad() + secColliderData->GetRad();
	// 貫通深度にオフセットを加えた、最終的な押し戻し距離を計算
	float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

	// 位置の修正
	// 計算した方向と距離から、押し戻しベクトルを生成
	Vector3 fixVec = pushBackVec * pushBackDist;

	// 優先度が同じでお互いに押し戻す場合
	if (isMutualPushback) {
		// 押し戻し量を半分ずつに分ける
		Vector3 halfFixVec = fixVec * 0.5f;
		priSphere->AddVel(halfFixVec);
		secSphere->AddVel(-halfFixVec);
	}
	// secondaryのみを押し戻す場合
	else {
		// secondaryを、primaryから離れる方向(pushBackVecの逆方向)に押し戻す
		secSphere->AddVel(-fixVec);
	}
}

void FixNextPosCapsuleCapsule(const std::shared_ptr<Collider> priCapsule, const std::shared_ptr<Collider> secCapsule, bool isMutualPushback)
{
	// 当たり判定データ取得
	std::shared_ptr<ColliderDataCapsule> priCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(priCapsule->GetColData());
	std::shared_ptr<ColliderDataCapsule> secCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(secCapsule->GetColData());
	// primaryカプセルの情報を取得
	Position3 priStart = priCapsuleData->GetStartPos(priCapsule->CalcNextPos());
	Position3 priEnd = priCapsuleData->GetEndPos(priCapsule->CalcNextPos());
	float priRadius = priCapsuleData->GetRad();

	// secondaryカプセルの情報を取得
	Position3 secStart = secCapsuleData->GetStartPos(secCapsule->CalcNextPos());
	Position3 secEnd = secCapsuleData->GetEndPos(secCapsule->CalcNextPos());
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
		pushBackVec = secCapsule->CalcNextPos() - priCapsule->CalcNextPos();
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
		priCapsule->AddVel(-halfFixVec);
		secCapsule->AddVel(halfFixVec);
	}
	// secondaryのみを押し戻す場合
	else {
		secCapsule->AddVel(fixVec);
	}
}

void FixNextPosSphereCapsule(const std::shared_ptr<Collider> primary, const std::shared_ptr<Collider> secondary, bool isMutualPushback)
{
	// 当たり判定データ取得
	std::shared_ptr<Collider> sphereObj;
	std::shared_ptr<Collider> capsuleObj;
	// primaryとsecondaryがそれぞれ球かカプセルかを判別
	if (primary->GetColData()->GetKind() == PhysicsData::ColliderKind::Sphere) {
		sphereObj = primary;
		capsuleObj = secondary;
	}
	else {
		sphereObj = secondary;
		capsuleObj = primary;
	}

	// それぞれのColliderからデータを取得
	std::shared_ptr<ColliderDataSphere> sphereData = std::static_pointer_cast<ColliderDataSphere>(sphereObj->GetColData());
	std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->GetColData());

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

void FixNextPosCapsulePolygon(const std::shared_ptr<Collider> capsule, const std::shared_ptr<Collider> polygon, bool isMutualPushback)
{
	// それぞれのColliderからデータを取得
	std::shared_ptr<ColliderDataCapsule> capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsule->GetColData());
	std::shared_ptr<ColliderDataPolygon> polygonData = std::static_pointer_cast<ColliderDataPolygon>(polygon->GetColData());

	// 連続で当たっているなら
	if (polygonData->IsContinuousCollision()) {
		// 当たったポリゴンの情報
		auto lineHit = polygonData->GetLineHit();

		// 壁なのか床なのかを見る
		// 法線のY成分が大きければ床、小さければ壁
		if (abs(lineHit.Normal.y) < PhysicsData::kWallThreshold) {
			// 壁の補正ベクトルを計算 (現在の位置から目標位置への差分を AddVel する)
			Vector3 pushBackVec = (Vector3(lineHit.HitPosition) + Vector3(lineHit.Normal) * (capsuleData->GetRad() + PhysicsData::kFixPositionOffset)) - capsule->CalcNextPos();
			pushBackVec.y = 0; // Yは維持
			capsule->AddVel(pushBackVec);
		}
		else {
			// 床の補正
			float targetY = lineHit.HitPosition.y + capsuleData->GetRad() + PhysicsData::kFixPositionOffset;
			float overlapY = targetY - capsule->CalcNextPos().y;
			capsule->AddVel(Vector3(0, overlapY, 0));
		}

		// 連続判定状態リセット
		polygonData->SetContinuousCollision(false);
	}
	else {
		// 当たったポリゴンの情報
		auto& hitDim = polygonData->GetHitDim();
		// お互い動かないオブジェクトなら衝突しない(ポリゴンはスタティックなので片方がスタティックなら)
		if (capsule->GetPriority() == PhysicsData::Priority::Static) {
			// 検出したプレイヤーの周囲のポリゴン情報を開放する
			DxLib::MV1CollResultPolyDimTerminate(hitDim);
			return;
		}

		// カプセルの頭座標と足座標
		Position3 headPos = capsuleData->GetEndPos(capsule->CalcNextPos());	// 移動後
		Position3 legPos = capsule->CalcNextPos();	// 移動後
		// 頭より足のほうが低い位置にあるなら入れ替える
		if (headPos.y < legPos.y) {
			Position3 temp = legPos;
			legPos = headPos;
			headPos = temp;
		}
		float radius = capsuleData->GetRad();

		std::vector<MV1_COLL_RESULT_POLY> wall;
		std::vector<MV1_COLL_RESULT_POLY> floorAndRoof;

		// 床ポリゴンと壁ポリゴンに分ける
		AnalyzeWallAndFloor(hitDim, legPos, wall, floorAndRoof);

		// 床か天井に当たったか
		bool isFloorAndRoof = !floorAndRoof.empty();
		// 壁に当たったか
		bool isWall = !wall.empty();

		// 床と当たったら
		if (isFloorAndRoof) {
			// 上昇中なら
			if (capsule->GetVel().y > 0.0f) {
				//天井に当たった処理
				HitRoofCP(capsule, headPos, radius, floorAndRoof);
			}
			else {
				// 床の高さに合わせる
				HitFloorCP(capsule, legPos, headPos, radius, floorAndRoof);
			}
		}
		// 壁と当たっているなら
		if (isWall) {
			// 壁に当たっている
			capsule->SetIsWall(true);

			// 補正するベクトルを返す
			Vector3 overlapVec = HitWallCP(headPos, legPos, radius, wall);

			// ベクトルを補正
			capsule->AddVel(overlapVec);
		}


		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		DxLib::MV1CollResultPolyDimTerminate(hitDim);
	}
}

void AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, Vector3 nextPos,
	std::vector<MV1_COLL_RESULT_POLY>& wall, 
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof)
{
	// 初期化
	wall.clear();
	floorAndRoof.clear();

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
				if (wall.size() < PhysicsData::kMaxHitPolygon) {
					// ポリゴンの構造体のアドレスを壁ポリゴン配列に保存
					wall.emplace_back(hitDim.Dim[i]);
				}
			}
		}
		// 床ポリゴン
		else {
			// ポリゴンの数が列挙できる限界数に達していなかったらポリゴン配列に保存
			if (floorAndRoof.size() < PhysicsData::kMaxHitPolygon) {
				// ポリゴンの構造体のアドレスを床ポリゴン配列に保存
				floorAndRoof.emplace_back(hitDim.Dim[i]);
			}
		}
	}
}

Vector3 HitWallCP(const Vector3& headPos, const Vector3& legPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& wall)
{
	// 垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;	// 最短距離

	Vector3 top = headPos;
	top.y += shortDis;
	Vector3 bot = legPos;
	bot.y -= shortDis;

	// 法線
	Vector3 norm = {};
	for (auto& w : wall) {
		// 壁かチェック
		if (abs(w.Normal.y) >= PhysicsData::kWallThreshold)	continue;
		VECTOR pos1 = w.Position[0];
		VECTOR pos2 = w.Position[1];
		VECTOR pos3 = w.Position[2];
		// 最短距離の2乗を返す
		float dis = Segment_Triangle_MinLength_Square(top, bot, pos1, pos2, pos3);
		// 平方根を返す
		dis = sqrtf(dis);
		// 初回または前回より距離が短いなら
		if (hitShortDis > dis) {
			// 現状の最短
			hitShortDis = dis;
			// 法線
			norm = w.Normal;
		}
	}
	// 押し戻し
	// どれくらい押し戻すか
	float overlap = shortDis - hitShortDis;
	overlap = std::clamp<float>(overlap, 0, shortDis);
	overlap += PhysicsData::kFixPositionOffset;
	// 正規化
	if (norm.Magnitude() != 0.0f) {
		norm = norm.Normalize();
	}
	return norm * overlap;
}

bool HitFloorCP(const std::shared_ptr<Collider> other, 
	const Vector3& legPos, const Vector3& headPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof)
{
	// 足の真下にある床の中で最も高いY
	float finalFloorY = -FLT_MAX;
	bool hitFloor = false;

	// 足の真下レイ(下方向)
	VECTOR rayStart = legPos;
	VECTOR rayEnd = VAdd(rayStart, VGet(0.0f, kCheckUnder, 0.0f));

	for (auto& floor : floorAndRoof) {
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

	for (auto& floor : floorAndRoof) {
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

void HitRoofCP(const std::shared_ptr<Collider> other, 
	const Vector3& headPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof)
{
	// 垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;	// 最短距離
	// 当たった中で足元に一番近いY座標に合わせる
	float lowHitPosY = other->GetPos().y;
	// 天井と当たったか
	bool isHitRoof = false;
	for (auto& roof : floorAndRoof) {
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
		Vector3 norm = Vector3(0.0f, -1.0f, 0.0f);
		// 力を与える
		other->AddVel(norm * overlap);
	}
}

Position3 ClosestPointPointAndSegment(const Position3& point, const Position3& start, const Position3& end)
{
	// 線分の始点から終点へのベクトル
	Vector3 segmentVec = end - start;
	// 線分の始点から指定された点へのベクトル
    Vector3 pointToStart = point - start;
    
    // 線分の長さの2乗
    float segmentLengthSq = segmentVec.SqrMagnitude();
    
    // 線分の長さがほぼ0の場合、始点が最近接点
	if (segmentLengthSq < PhysicsData::kZeroTolerance) {
		return start;
	}
	
	// 点を線分ベクトルに射影したときの t を計算
	// t = (point - start)・(end - start) / |end - start| ^ 2
	float t = Dot(pointToStart, segmentVec) / segmentLengthSq;
	
	// t を0.0～1.0の範囲にクランプする
	t = std::clamp(t, 0.0f, 1.0f);
	
	// 最近接点を計算して返す
	return (start + segmentVec * t);
}

void ClosestPointSegments(const Position3& startA, const Position3& endA, const Position3& startB, const Position3& endB, Position3& closestPointA, Position3& closestPointB)
{
	// 2つの線分の最近接点を求める処理
	Vector3 segAVec = endA - startA;	// 線分Aの方向ベクトル
	Vector3 segBVec = endB - startB;	// 線分Bの方向ベクトル
	Vector3 offsetVec = startA - startB;

	float segALenSq = segAVec.SqrMagnitude();	// 線分Aの長さの2乗
	float segBLenSq = segBVec.SqrMagnitude();	// 線分Bの長さの2乗
	float segBDotOffset = Dot(segBVec, offsetVec);	// 線分Bの方向ベクトルと両始点間ベクトルの内積

	// 両方の線分が点とみなせる場合
	if (segALenSq <= PhysicsData::kZeroTolerance && segBLenSq <= PhysicsData::kZeroTolerance) {
		// 両方が点の場合
		closestPointA = startA;
		closestPointB = startB;
		return;		// 処理終了
	}
	// 線分Aのみが点の場合
	else if (segALenSq <= PhysicsData::kZeroTolerance) {
		closestPointA = startA;
		closestPointB = ClosestPointPointAndSegment(startA, startB, endB);
		return;		// 処理終了
	}
	// 線分Bが点である場合
	else if (segBLenSq <= PhysicsData::kZeroTolerance) {
		closestPointA = ClosestPointPointAndSegment(startB, startA, endA);
		closestPointB = startB;
		return;		// 処理終了
	}

	// 両方が線分として扱える場合
	// 線分Aの方向ベクトルと両始点間ベクトルの内積
	float segADotOffset = Dot(segAVec, offsetVec);
	// 線分Aと線分Bの方向ベクトルの内積
	float segADotSegB = Dot(segAVec, segBVec);
	// 線分上の最近接点を求めるための変数 (0.0～1.0)
	float paramA, paramB;

	// 最近接点のパラメータを計算
	CalculateClosestSegmentParameters(
		segALenSq, segBLenSq, segADotSegB, segADotOffset, segBDotOffset, 
		paramA, paramB);

	// 求めた値を用いて、線分上の最近接点を求める
	closestPointA = startA + segAVec * paramA;
	closestPointB = startB + segBVec * paramB;
}

void CalculateClosestSegmentParameters(
	float segALenSq, float segBLenSq, float segADotSegB,
	float segADotOffset, float segBDotOffset,
	float& paramA, float& paramB)
{
	// 最近接点を求める式の分母
	float denom = segALenSq * segBLenSq - segADotSegB * segADotSegB;

	// 二直線が平行でない場合
	if (std::abs(denom) > PhysicsData::kZeroTolerance) {
		// 直線上の最近接点を計算
		paramA = (segADotSegB * segBDotOffset - segADotOffset * segBLenSq) / denom;
		paramB = (segALenSq * segBDotOffset - segADotSegB * segADotOffset) / denom;
	}
	// 線分が平行な場合
	else {
		// 計算を簡略化するため、片方の始点を基準に計算する
		paramA = 0.0f;
		paramB = std::clamp(segBDotOffset / segBLenSq, 0.0f, 1.0f); // クランプ
	}

	// 計算されたパラメータが両方とも 0-1 の範囲内にあるかチェック
	// 最近接点が両線分の内部にあるケース
	if (paramA >= 0.0f && paramA <= 1.0f && paramB >= 0.0f && paramB <= 1.0f) {
		// 範囲内なので何もしない
		return;
	}

	// それ以外の場合、最近接点は少なくとも片方の線分の端点にある
	// 最近接点が線分の外側にある場合、端点にクランプして再計算する

	// paramA を 0-1 にクランプ
	paramA = std::clamp(paramA, 0.0f, 1.0f);
	// クランプした paramA を元に paramB を再計算
	paramB = (segBDotOffset + segADotSegB * paramA) / segBLenSq;
	paramB = std::clamp(paramB, 0.0f, 1.0f);

	// さらに、クランプした paramB を元に paramA を再々計算
	// これにより、両方の範囲が互いに最適な状態に収束する
	paramA = (-segADotOffset + segADotSegB * paramB) / segALenSq;
	paramA = std::clamp(paramA, 0.0f, 1.0f);

	return;
}