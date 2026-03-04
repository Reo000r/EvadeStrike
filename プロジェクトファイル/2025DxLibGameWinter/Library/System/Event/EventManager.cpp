#include "EventManager.h"
#include "EventCollider.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Physics/Physics.h"
#include "Library/System/ObjectHandleHolder.h"
#include "Library/System/SoundManager.h"
#include <cassert>
#include <vector>

namespace {
    const std::string kEventTypeEnemySpawn = "SpawnEnemy";
    const std::string kEventTypeDefeatEnemy = "DefeatEnemy";
}

EventManager::EventManager() :
    _requiredEventCount(-1),
    _isClearable(false)
{
}

void EventManager::Init(
    std::weak_ptr<Physics> physics,
    std::weak_ptr<EnemyManager> enemyManager,
    std::vector<ObjectData> enemySpawnDataList) {
    _physics = physics;
    _enemyManager = enemyManager;
    _enemySpawnDataList = enemySpawnDataList;

    // クリアに必要なイベントID数の算出
    std::vector<int> uniqueIds;
    for (const auto& data : _enemySpawnDataList) {
        // カウント対象であれば
        if (data.eventData.eventType == kEventTypeEnemySpawn.c_str()) {
            // 既にuniqueIdsに同じIDが存在するか検索
            auto it = std::find(uniqueIds.begin(), uniqueIds.end(), data.eventData.thisEventId);
            // 見つからなかった場合のみ追加
            if (it == uniqueIds.end()) {
                uniqueIds.emplace_back(data.eventData.thisEventId);
            }
        }
    }
    // 満たしていたイベント数を記録
    _requiredEventCount = static_cast<int>(uniqueIds.size());
    _executedEventList.clear();
}

void EventManager::Update() {
    // 更新
    for (auto& eventCol : _eventColList) {
        eventCol->Update();
    }

    // Physicsから解放させる
    for (auto& eventCol : _eventColList) {
        // 削除されていない場合は次へ
        if (!eventCol->IsExpired()) continue;

        // Physicsから削除
        eventCol->ReleasePhysics();
    }

    // 削除対象になったイベントをコンテナから一括削除
    std::erase_if(_eventColList, [](const std::shared_ptr<EventCollider>& col) {
        return col->IsExpired();
        });
}

void EventManager::Draw()
{
    // 描画
    for (auto& eventCol : _eventColList) {
        eventCol->Draw();
    }
}

void EventManager::CallEvent(int eventId, const std::string& eventType)
{
    printf("イベントが呼ばれた(ID:%d, Type:%s)\n", eventId, eventType.c_str());

    // 実行済みリストへの登録とクリア判定
    // 重複登録を避ける（同じイベントが複数回呼ばれる設計の場合）
    auto it = std::find_if(_executedEventList.begin(), _executedEventList.end(),
        [eventId, &eventType](const std::pair<std::string, int>& pair) {
            return pair.first == eventType && pair.second == eventId;
        });
    // 登録された場合
    if (it == _executedEventList.end()) {
        _executedEventList.push_back({ eventType, eventId });

        // 特定のタイプの完了数をカウント
        int completedCount = 0;
        for (const auto& log : _executedEventList) {
            if (log.first == kEventTypeDefeatEnemy) {
                completedCount++;
            }
        }

        // 必要数に達したらフラグを立てる
        if (completedCount >= _requiredEventCount) {
            _isClearable = true;
            printf("クリア可能状態\n");
        }
    }

    // 敵の出現イベント判定
    if (eventType == "SpawnEnemy") {
        if (!_enemyManager.lock()) {
            assert(false && "不正なenemyManager");
            return;
        }

        // 生成を行ったか
        bool isSpawned = false;
        // 指定されたIDに紐づく敵を生成
        // _enemySpawnDataListを利用して指定されたID
        for (const auto data : _enemySpawnDataList) {
            // データが一致していないイベントの場合はcontinue
            if (data.eventData.thisEventId != eventId) continue;
            if (data.eventData.eventType != eventType) continue;

            // 敵のタイプをgroupNameから判定して生成
            EnemyType type;
            if (data.statusData.groupName == "EnemyWeak") {
                type = EnemyType::Weak;
            }
            else {
                // 未対応の敵種ならcontinue
                printf("未対応の敵種:%s\n", eventType.c_str());
                continue;
            }

            // EnemyManagerから生成
            _enemyManager.lock()->SpawnEnemy(type, data.transData.pos);
            printf("敵生成\n");
            isSpawned = true;
        }

        if (isSpawned) {
            _enemyManager.lock()->SetActiveEventId(eventId);
        }
    }
    // 敵の消滅イベント判定
    else if (eventType == "DefeatEnemy") {
        // 対応したイベント壁を無効化
        printf("敵全滅イベント\n");
        SoundManager::GetInstance().PlaySoundType(SEType::StageUnlock);
        for (const auto col : _eventColList) {
            // データが一致していないイベントの場合はcontinue
            if (col->GetObjectData().eventData.thisEventId != eventId) continue;
            if (col->GetObjectData().eventData.eventType != eventType) continue;
            // 無効化する
            col->Expire();
            printf("イベント壁無効化\n");
        }
    }
    // 未対応イベント判定
    else {
        printf("未対応のイベントタイプ:%s\n", eventType.c_str());
    }

    // 登録されている全EventCollider（壁やトリガー）へ通知
    for (auto& eventCol : _eventColList) {
        // 各オブジェクトが自身の ID と一致するか判断し、反応する
        eventCol->OnCall(eventId, eventType);
    }

    printf("イベント処理終了\n");
}

void EventManager::RegisterEventCollider(std::shared_ptr<EventCollider> eventCollider)
{
    // 見つからなかった場合はend
    auto it = (
        std::find(
            _eventColList.begin(),
            _eventColList.end(),
            eventCollider));

    // 見つからなければ(登録されていなければ)
    if (it == _eventColList.end()) {
        _eventColList.push_back(eventCollider);	// 登録
    }
    // 既に登録されていたらassert
    else {
        assert(false && "指定のcolliderは登録済");
    }

}
