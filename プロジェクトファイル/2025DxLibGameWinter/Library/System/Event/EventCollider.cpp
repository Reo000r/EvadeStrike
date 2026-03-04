#include "EventCollider.h"
#include "Library/System/ObjectHandleHolder.h"
#include "Library/Physics/Physics.h"

EventCollider::EventCollider(
	std::weak_ptr<Physics> physics, 
	const ObjectData& data, 
	std::weak_ptr<ObjectHandleHolder> holder, 
	bool isTrigger)
    : GameObject(physics,
        PhysicsData::Priority::Static,
        PhysicsData::GameObjectTag::EventWall,
        PhysicsData::ColliderKind::Polygon,
		isTrigger, true, false),
	_handleHolder(holder),
	_eventManager(),
    _data(data), 
	_isExpired(false),
	_modelHandle(-1),
	_isDraw(true)
{
}

EventCollider::~EventCollider()
{
}

void EventCollider::Init() {
    // 自身のポインタをColliderに設定
    _collider->SetParent(shared_from_this());

    // 位置の設定
    _collider->SetPos(_data.transData.pos);
	// 名前からハンドルを取得
	_modelHandle = _handleHolder.lock()->GetModelHandle(_data.transData.name);
	// 異常値であれば
	if (_modelHandle < 0) {
		printf("ハンドルが異常値(%s)\n", _data.transData.name.c_str());
		return;
	}

	// オブジェクトを配置
	// 行列作成
	MATRIX mZ = MGetRotZ(_data.transData.rot.z);
	MATRIX mX = MGetRotX(_data.transData.rot.x);
	MATRIX mY = MGetRotY(_data.transData.rot.y);
	MATRIX rotMat = MMult(MMult(mZ, mX), mY);

	MATRIX scaleMat = MGetScale(_data.transData.scale);
	MATRIX transMat = MGetTranslate(_data.transData.pos);
	MATRIX finalMat = MMult(MMult(scaleMat, rotMat), transMat);
	// 適用
	MV1SetMatrix(_modelHandle, finalMat);

	// 当たり判定データ設定
	_collider->CreateColliderDataPolygon(
		_modelHandle,
		_data.colData.isTrigger,	// isTrigger
		_data.colData.isCollision	// isCollision
	);
	// 壁や床とは当たり判定を行わない
	_collider->AddThroughTag(PhysicsData::GameObjectTag::SystemWall);
	_collider->AddThroughTag(PhysicsData::GameObjectTag::StepGround);

	// col->SetPosで正確な位置を入れるか
	// 当たり判定時にGetModelPosをしないと当たり判定がおかしくなるかも

	// physicsに登録する
	EntryPhysics(_physics);
}

void EventCollider::Update() {
    // 処理無し
	if (_eventManager.lock()) {}
}

void EventCollider::Draw() const {
	if (_isDraw) MV1DrawModel(_modelHandle);
}

void EventCollider::SetEventManager(std::weak_ptr<EventManager> eventManager)
{
	_eventManager = eventManager;
}

void EventCollider::Expire()
{
	// 削除フラグを立てる
	_isExpired = true;
}
