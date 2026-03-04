#include "StagePlacer.h"
#include "Loader/ObjectDataLoader.h"
#include "Library/System/ObjectHandleHolder.h"
#include "Library/Geometry/Vector3.h"
#include "Library/Geometry/Calculation.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ColliderData.h"
#include "Library/Physics/ColliderDataPolygon.h"
#include "Library/Physics/Physics.h"
#include "Library/System/Event/EventManager.h"
#include "Library/System/Event/EventCollider.h"
#include "Library/System/Event/EventTrigger.h"
#include "Library/System/Event/EventWall.h"
#include <DxLib.h>
#include <string>

namespace {
	// モデルの位置の補正値
	constexpr float kModelTransformOffset = 100.0f;

	// ステージの拡縮倍率
	constexpr float kStageScale = 4.0f;
	// モデル自体の拡縮補正
	constexpr float kModelScaleOffset = 0.01f;

	const Position3 kStagePosOffset = 
		Position3(0, -0.0f, 0) *
		//Position3(0, -10.4f, -10.0f) * 
		kModelTransformOffset * kStageScale;

	const std::string kStageDataPath = "Data/StagePlaceData.csv";
}

StagePlacer::StagePlacer()
{
}

StagePlacer::~StagePlacer()
{
	// モデルのハンドルを解放
	for (auto& col : _colliders) {
		
		auto colDataPolygon = std::static_pointer_cast<ColliderDataPolygon>(col->GetColData());
		int modelHandle = colDataPolygon->GetModelHandle();
		if (modelHandle >= 0) {
			MV1DeleteModel(modelHandle);
		}
	}
}

void StagePlacer::SetObjectHandleHolder(std::shared_ptr<ObjectHandleHolder> holder)
{
	_holder = holder;
}

std::vector<ObjectData> StagePlacer::Place(
	std::weak_ptr<Physics> physics, 
	std::weak_ptr<EventManager> eventManager)
{
	printf("---ステージ配置開始---\n");
	std::vector<ObjectData> enemySpawnData;
	// kStageDataPathの情報をObjectDataLoaderを用いて取得
	std::vector<ObjectData> objData =
		ObjectDataLoader::LoadData(kStageDataPath.c_str());
	for (const auto& data : objData) {
		
		// 壁や床であれば
		if (data.transData.name == "Ground" ||
			data.transData.name == "Wall") {
			int handle = -1;
			// 名前からハンドルを取得
			handle = _holder.lock()->GetModelHandle(data.transData.name);

			// 異常値であれば
			if (handle < 0) {
				printf("ハンドルが異常値(%s)\n", data.transData.name.c_str());
			}
			// オブジェクトを配置
			// 行列作成
			MATRIX mZ = MGetRotZ(data.transData.rot.z);
			MATRIX mX = MGetRotX(data.transData.rot.x);
			MATRIX mY = MGetRotY(data.transData.rot.y);
			MATRIX rotMat = MMult(MMult(mZ, mX), mY);

			MATRIX scaleMat = MGetScale(data.transData.scale);
			MATRIX transMat = MGetTranslate(data.transData.pos);
			MATRIX finalMat = MMult(MMult(scaleMat, rotMat), transMat);
			// 適用
			MV1SetMatrix(handle, finalMat);

			// 当たり判定作成
			std::shared_ptr<Collider> col = 
				std::make_shared<Collider>(
				PhysicsData::Priority::Static,
				PhysicsData::GameObjectTag::SystemWall,
				PhysicsData::ColliderKind::Polygon,
				false, true, false
			);
			// 当たり判定データ設定
			col->CreateColliderDataPolygon(
				handle,
				false,			// isTrigger
				true			// isCollision
			);
			// 壁や床とは当たり判定を行わない
			col->AddThroughTag(PhysicsData::GameObjectTag::SystemWall);
			col->AddThroughTag(PhysicsData::GameObjectTag::StepGround);

			// col->SetPosで正確な位置を入れるか
			// 当たり判定時にGetModelPosをしないと当たり判定がおかしくなるかも

			// physicsに登録する
			physics.lock()->Entry(col);

			// 保持
			_colliders.emplace_back(col);

			printf("配置(%s)\n", data.transData.name.c_str());
		}
		// イベント判定であれば
		else if (data.transData.name == "EventWall") {
			// isTriggerフラグによって生成する派生クラスを切り分ける
			std::shared_ptr<EventCollider> eventCol;
			// 判定壁だった場合
			if (data.colData.isTrigger) {
				// 判定壁生成
				eventCol = std::make_shared<EventTrigger>(physics, data, _holder); 
			}
			// 通常壁だった場合
			else {
				// 壁生成
				eventCol = std::make_shared<EventWall>(physics, data, _holder);
			}
			// EventManagerをセット
			eventCol->SetEventManager(eventManager);

			// 正しく生成されていれば
			if (eventCol) {
				eventCol->Init();
				// EventManagerに登録して一元管理させる
				eventManager.lock()->RegisterEventCollider(eventCol);
			}
			printf("イベント配置(%s, Trigger:%d)\n", data.transData.name.c_str(), data.colData.isTrigger);
		}
		// 敵の出現位置であれば
		else if (data.transData.name == "EnemySpawn") {
			// EnemyManagerへ渡すデータ
			enemySpawnData.push_back(data);
			printf("敵出現位置記録(%s, Group:%s)\n", data.transData.name.c_str(), data.statusData.groupName.c_str());
		}
		else {
			printf("未対応(%s)\n", data.transData.name.c_str());
		}
	}

	printf("---ステージ配置完了---\n");

	return enemySpawnData;
}

void StagePlacer::Draw()
{
	// モデル描画
	for (auto& col : _colliders) {
		auto colDataPolygon = std::static_pointer_cast<ColliderDataPolygon>(col->GetColData());
		MV1DrawModel(colDataPolygon->GetModelHandle());
	}
}
