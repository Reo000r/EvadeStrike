#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
//#include "Library/Geometry/Vector3.h"

struct CollisionData {
    bool isCollision = true;    // 当たり判定可否
    bool isTrigger = false;     // 押し戻し可否
};

/// <summary>
/// オブジェクト情報
/// </summary>
struct ObjectTransformData {
    std::string name = "None";  // オブジェクト名
    VECTOR pos = {};            // 位置
    VECTOR rot = {};            // 回転(ラジアン)
    VECTOR scale = {};          // 拡縮
};

/// <summary>
/// キャラクターのステータス情報
/// </summary>
struct CharacterStatusData {
    std::string groupName = "None"; // グループ名
    float hitPoint = 0.0f;          // 体力                      
    float attackPower = 0.0f;       // 攻撃力
    float moveSpeed = 0.0f;         // 移動速度
    float searchDistance = 0.0f;    // 索敵範囲
    float modelRotSpeed = 0.0f;     // 旋回速度
};

/// <summary>
/// <para> イベント情報 </para>
/// <para> thisEventId: </para>
/// <para> 　このオブジェクトのイベントID </para>
/// <para> 　これが指定されActive/Disactive命令を受けたら従う </para>
/// <para> callEventId: </para>
/// <para> 　このオブジェクトが発するイベントID </para>
/// <para> 　このオブジェクトが触れられたり倒されたりする(イベント種別による)と </para>
/// <para> 　このイベントIDを種別とともにイベント管理者へ発する </para>
/// </summary>
struct EventData {
    std::string eventType = "None"; // イベント種別
    int thisEventId = -1;           // このオブジェクトのイベントID
    int callEventId = -1;           // 呼び出すイベントID
};

struct ObjectData {
    CollisionData colData;
    ObjectTransformData transData;
    CharacterStatusData statusData;
    EventData eventData;
};

class ObjectDataLoader
{
public:
    ObjectDataLoader();
	virtual ~ObjectDataLoader();

    /// <summary>
    /// 指定のファイルからオブジェクト情報を取得
    /// </summary>
    /// <param name="fileName">パス</param>
    /// <returns></returns>
    static std::vector<ObjectData> LoadData(std::string fileName);

};

