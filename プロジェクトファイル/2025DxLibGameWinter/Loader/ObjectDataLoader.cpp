#include "ObjectDataLoader.h"
#include "Library/Geometry/Calculation.h"
#include "Library/Geometry/Vector3.h"
#include <fstream>
#include <sstream>
#include <cassert>

namespace {
	std::string kHeader = 
		"Name,GroupName,IsCollision,IsTrigger,PosX,PosY,PosZ,RotX,RotY,RotZ,ScaleX,ScaleY,ScaleZ,HitPoint,AttackPower,MoveSpeed,SearchDistance,ModelRotSpeed,EventType,ThisEventId,CallEventId";

	// モデルの回転補正(度数法)
	const Vector3 kRotOffset = Vector3(0.0f, 0.0f, 0.0f);

	// モデルの位置の補正値
	constexpr float kModelTransformOffset = 100.0f;//100.0f;

	// ステージの拡縮倍率
	constexpr float kStageScale = 1.75f;
	// モデル自体の拡縮補正
	constexpr float kModelScaleOffset = 0.01f;

	const Position3 kStagePosOffset =
		Position3(0, -0.0f, 0) *
		//Position3(0, -10.4f, -10.0f) * 
		kModelTransformOffset * kStageScale;
}

ObjectDataLoader::ObjectDataLoader()
{
}

ObjectDataLoader::~ObjectDataLoader()
{
}

std::vector<ObjectData> ObjectDataLoader::LoadData(std::string fileName)
{
	// データ
	std::vector<ObjectData> objects;

	// ファイルを開く
	std::ifstream file(fileName.c_str());
	// もしもファイルを開けなかったら
	if (!file.is_open()) {
		printf("ファイルが開けなかった\n");
		printf("　path : %s\n", fileName.c_str());
		// ファイルを閉じて空のリストを返す
		file.close();
		return objects;
	}

	// 1行分のデータ
	std::string line;

	// ヘッダ照合
	std::getline(file, line);
	// ヘッダが一致していないなら
	if (line != kHeader) {
		printf("ヘッダが一致していない\n");
		printf("　path : %s\n", fileName.c_str());
		printf("　header : %s\n", kHeader.c_str());
	}

	// 1行ずつ読み取る
	while (std::getline(file,line)) {
		//行をカンマ区切りで1つずつ読み込むための準備
		std::stringstream ss(line);
		std::string part;
		ObjectData objData;
		constexpr char c = ',';

		// 代入

		// 名前、グループ名
		if (std::getline(ss, part, c)) objData.transData.name = part;
		if (std::getline(ss, part, c)) objData.statusData.groupName = part;

		// 当たり判定情報
		if (std::getline(ss, part, c)) objData.colData.isCollision = std::stoi(part);
		if (std::getline(ss, part, c)) objData.colData.isTrigger = std::stoi(part);


		// 位置、回転、拡縮
		// DxLib空間への変換
		// 位置
		float px, py, pz;
		if (std::getline(ss, part, c)) px = std::stof(part);
		if (std::getline(ss, part, c)) py = std::stof(part);
		if (std::getline(ss, part, c)) pz = std::stof(part);

		objData.transData.pos.x = px * kModelTransformOffset * kStageScale + kStagePosOffset.x;
		objData.transData.pos.y = py * kModelTransformOffset * kStageScale + kStagePosOffset.y;
		objData.transData.pos.z = pz * kModelTransformOffset * kStageScale + kStagePosOffset.z;

		// 回転
		if (std::getline(ss, part, c)) objData.transData.rot.x = Calc::ToRadian(std::stof(part));
		if (std::getline(ss, part, c)) objData.transData.rot.y = Calc::ToRadian(std::stof(part));
		if (std::getline(ss, part, c)) objData.transData.rot.z = Calc::ToRadian(std::stof(part));

		// 拡縮
		if (std::getline(ss, part, c)) objData.transData.scale.x = std::stof(part) * kStageScale * kModelScaleOffset;
		if (std::getline(ss, part, c)) objData.transData.scale.y = std::stof(part) * kStageScale * kModelScaleOffset;
		if (std::getline(ss, part, c)) objData.transData.scale.z = std::stof(part) * kStageScale * kModelScaleOffset;
		

		// キャラクター情報
		if (std::getline(ss, part, c)) objData.statusData.hitPoint = std::stof(part);
		if (std::getline(ss, part, c)) objData.statusData.attackPower = std::stof(part);
		if (std::getline(ss, part, c)) objData.statusData.moveSpeed = std::stof(part);
		if (std::getline(ss, part, c)) objData.statusData.searchDistance = std::stof(part);
		if (std::getline(ss, part, c)) objData.statusData.modelRotSpeed = std::stof(part);

		// イベント情報
		if (std::getline(ss, part, c)) objData.eventData.eventType = part;
		if (std::getline(ss, part, c)) objData.eventData.thisEventId = std::stoi(part);
		if (std::getline(ss, part, c)) objData.eventData.callEventId = std::stoi(part);

		//配列に追加
		objects.emplace_back(objData);
	}

	// ファイルを閉じて情報を返す
	file.close();
	return objects;
}
