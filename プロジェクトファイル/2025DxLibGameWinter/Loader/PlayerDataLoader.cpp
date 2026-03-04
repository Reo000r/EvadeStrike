#include "PlayerDataLoader.h"
#include "Library/Geometry/Calculation.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <DxLib.h>

namespace {
	const std::wstring kAttackAnimFilePath = L"Data/PlayerAttackAnimData.csv";
	const std::wstring kAttackStatsFilePath = L"Data/PlayerAttackStats.csv";
    constexpr wchar_t kSplitMark = L',';
    const std::wstring kAttackAnimHeader = 
        L"ID,DefaultID,PunchID,KickID,StartAttackActiveRate,EndAttackActiveRate,StartTransRate,EndTransRate,AnimSpeed,AnimName";
    const std::wstring kAttackStatsHeader = 
        L"Kind,Rad,Height,TransX,Y,Z,ScaleX,Y,Z,AngleX,Y,Z,KnockbackX,Y,Z,Attack,Break";

    constexpr int kPunchAttackID = 00;
    constexpr int kKickAttackID = 10;
    constexpr int kSpecialAttackID = 20;
}

PlayerDataLoader::PlayerDataLoader()
{
}

void PlayerDataLoader::Load()
{
    AttackAnimDataLoad();

    AttackStatsLoad();
}

PlayerAttackAnimData PlayerDataLoader::GetAttackAnimData(int id)
{
	// 検索をかけて要素が存在していたら
	auto it = _attackAnimDataMap.find(id);
	if (it != _attackAnimDataMap.end()) {
        // データを返す
        return it->second;
	}
    // 存在しなかった場合は
    assert(false && "存在しないIdが指定された");
    // 不正なデータを返す
    return PlayerAttackAnimData();
}

std::unordered_map<int, PlayerAttackAnimData> PlayerDataLoader::GetAttackAnimDataList()
{
	return _attackAnimDataMap;
}

PlayerAttackAnimData PlayerDataLoader::GetBeforeAtackAnimData() const
{
    PlayerAttackAnimData data;
    data.punchId = kPunchAttackID;
    data.kickId = kKickAttackID;
    return data;
}

AttackColStats PlayerDataLoader::GetAttackStats(PlayerActionKind kind)
{
    // 検索をかけて要素が存在していたら
    auto it = _attackStatsMap.find(kind);
    if (it != _attackStatsMap.end()) {
        // データを返す
        return it->second;
    }
    // 存在しなかった場合は
    assert(false && "不正な攻撃種別が指定された");
    // 不正なデータを返す
    return AttackColStats();
}

std::unordered_map<PlayerActionKind, AttackColStats> PlayerDataLoader::GetAttackStatsList()
{
    return _attackStatsMap;
}

int PlayerDataLoader::GetSpecialAttackID() const
{
    return kSpecialAttackID;
}

void PlayerDataLoader::AttackAnimDataLoad()
{
    _attackAnimDataMap.clear();
    printf("\n---プレイヤーの攻撃アニメーションデータ読み込み開始---\n");

    std::wifstream file(kAttackAnimFilePath);

    // ファイルが開けなければ
    if (!file.is_open()) {
        printf("＊＊＊ファイルが開けませんでした＊＊＊\n");
        printf("---プレイヤーの攻撃アニメーションデータ読み込み終了---\n\n");
        assert(false && "ファイルが開けなかった");
        return;
    }

    std::wstring line;
    // ヘッダが一致していなければ
    if (!std::getline(file, line) || line != kAttackAnimHeader) {
        printf("＊＊＊ヘッダが一致しませんでした＊＊＊\n");
        printf("%ls\n", line.c_str());
        printf("%ls\n", kAttackAnimHeader.c_str());
        printf("---プレイヤーの攻撃データ読み込み終了---\n\n");
        assert(false && "ヘッダが一致していなかった");
        file.close();
        return;
    }

    // データを一行ずつ読み込む
    while (std::getline(file, line)) {
        // 空白行はスキップ
        if (line.empty()) continue;
        PlayerAttackAnimData data = AttackAnimDataSplit(line);
        if (data.id != -1) {
            printf("データを読み込みました\n");
            printf("　ID　　　　：%d\n", data.id);
            printf("　AnimName　：%ls\n", data.animName.c_str());
            printf("　AnimSpeed ：%.02f\n", data.animSpeed);
            _attackAnimDataMap[data.id] = data;
        }
    }
    printf("---プレイヤーの攻撃アニメーションデータ読み込み終了---\n\n");
    file.close();
}

PlayerAttackAnimData PlayerDataLoader::AttackAnimDataSplit(std::wstring str)
{
	// 文字列を分割し各要素を保存
    PlayerAttackAnimData data;
    std::wstringstream ss(str);
    std::wstring item;

    if (std::getline(ss, item, kSplitMark)) data.id = std::stoi(item);
    if (std::getline(ss, item, kSplitMark)) data.defaultId = std::stoi(item);
    if (std::getline(ss, item, kSplitMark)) data.punchId = std::stoi(item);
    if (std::getline(ss, item, kSplitMark)) data.kickId = std::stoi(item);
    if (std::getline(ss, item, kSplitMark)) data.startAttackColActiveFrameRate = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.endAttackColActiveFrameRate = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.startTransformFrameRate = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.endTransformFrameRate = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.animSpeed = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.animName = item;

    return data;
}

void PlayerDataLoader::AttackStatsLoad()
{
    _attackStatsMap.clear();
    printf("\n---プレイヤーの攻撃データ読み込み開始---\n");

    std::wifstream file(kAttackStatsFilePath);

    // ファイルが開けなければ
    if (!file.is_open()) {
        printf("＊＊＊ファイルが開けませんでした＊＊＊\n");
        printf("---プレイヤーの攻撃データ読み込み終了---\n\n");
        assert(false && "ファイルが開けなかった");
        printf("プレイヤーの攻撃データ読み込み終了\n\n");
        return;
    }

    std::wstring line;
    // ヘッダが一致していなければ
    if (!std::getline(file, line) || line != kAttackStatsHeader) {
        printf("＊＊＊ヘッダが一致しませんでした＊＊＊\n");
        printf("%ls\n", line.c_str());
        printf("%ls\n", kAttackStatsHeader.c_str());
        printf("---プレイヤーの攻撃データ読み込み終了---\n\n");
        assert(false && "ヘッダが一致していなかった");
        printf("プレイヤーの攻撃データ読み込み終了\n\n");
        file.close();
        return;
    }

    // データを一行ずつ読み込む
    while (std::getline(file, line)) {
        // 空白行はスキップ
        if (line.empty()) continue;
        AttackColStats data = AttackStatsSplit(line);
        if (data.kind != PlayerActionKind::None) {
            printf("データを読み込みました\n");
            printf("　Kind　　　：%d\n", data.kind);
            printf("　ColRad　　：%.02f\n", data.colRad);
            printf("　ColHeight ：%.02f\n", data.colHeight);
            _attackStatsMap[data.kind] = data;
        }
    }
    printf("プレイヤーの攻撃データ読み込み終了\n\n");
    file.close();
}

AttackColStats PlayerDataLoader::AttackStatsSplit(std::wstring str)
{
    // 文字列を分割し各要素を保存
    AttackColStats data;
    std::wstringstream ss(str);
    std::wstring item;

    if (std::getline(ss, item, kSplitMark)) data.kind = static_cast<PlayerActionKind>(std::stoi(item));
    if (std::getline(ss, item, kSplitMark)) data.colRad = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.colHeight = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.transOffset.x = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.transOffset.y = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.transOffset.z = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.scale.x = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.scale.y = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.scale.z = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.angle.x = Calc::ToRadian(std::stof(item));
    if (std::getline(ss, item, kSplitMark)) data.angle.y = Calc::ToRadian(std::stof(item));
    if (std::getline(ss, item, kSplitMark)) data.angle.z = Calc::ToRadian(std::stof(item));
    if (std::getline(ss, item, kSplitMark)) data.knockbackVel.x = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.knockbackVel.y = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.knockbackVel.z = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.attackPower = std::stof(item);
    if (std::getline(ss, item, kSplitMark)) data.breakPower = std::stof(item);

    return data;
}
