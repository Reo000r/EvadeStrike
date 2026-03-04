#include "PlayerComboHolder.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <DxLib.h>

namespace {
    const std::wstring kFilePath = L"Data/PlayerComboData.csv";
    constexpr wchar_t kSplitMark = L',';
    const std::wstring kAttackAnimHeader = L"Combo1,Combo2,Combo3,Combo4,Combo5";
}

PlayerComboHolder::PlayerComboHolder() :
    _actions(),
    _comboList()
{
}

void PlayerComboHolder::LoadCombo()
{
    _comboList.clear();
    printf("\n---プレイヤーのコンボデータ読み込み開始---\n");

    std::wifstream file(kFilePath);

    // ファイルが開けなければ
    if (!file.is_open()) {
        printf("＊＊＊ファイルが開けませんでした＊＊＊\n");
        printf("---プレイヤーのコンボデータ読み込み終了---\n\n");
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
        return;
    }

    // データを一行ずつ読み込む
    while (std::getline(file, line)) {
        // 空行はスキップ
        if (line.empty()) continue;

        ComboData_t data = DataSplit(line);

        printf("データを読み込みました\n");
        _comboList.emplace_back(data);
    }
    printf("プレイヤーのコンボデータ読み込み終了\n\n");
    file.close();
}

void PlayerComboHolder::AddAction(PlayerActionKind name)
{
    _actions.emplace_back(name);
    std::wstring kind = L"";
    if (name == PlayerActionKind::Punch) {
        kind = L"Punch";
    }
    else {
        kind = L"Kick";
    }
    printf("アクションを追加：%d段目(%ls)\n", 
        static_cast<int>(_actions.size()), kind.c_str());
}

bool PlayerComboHolder::CheckComboConsist()
{
    // 保存されている入力がコンボになっているか調べる
    for (const auto& combo : _comboList) {
        // 長さが違ったら同一でない
        if (_actions.size() != combo.size()) continue;
        // 要素が完全に一致しているならtrue
        if (_actions == combo) return true;
    }
    return false;
}

bool PlayerComboHolder::CheckComboConsist(PlayerActionKind action)
{
    ComboData_t tempCombo = _actions;
    tempCombo.emplace_back(action);
    // 保存されている入力がコンボになっているか調べる
    for (const auto& combo : _comboList) {
        // 長さが違ったら同一でない
        if (tempCombo.size() != combo.size()) continue;
        // 要素が完全に一致しているならtrue
        if (tempCombo == combo) return true;
    }
    return false;
}

void PlayerComboHolder::ResetActionData()
{
    _actions.clear();
    printf("コンボが途切れた\n");
}

PlayerComboHolder::ComboData_t PlayerComboHolder::GetActionList() const
{
    return _actions;
}

PlayerComboHolder::ComboData_t PlayerComboHolder::DataSplit(std::wstring str)
{
    // 文字列を分割し各要素を保存
    ComboData_t data;
    std::wstringstream ss(str);
    std::wstring item;

    // 要素がなくなるまで実行
    while (std::getline(ss, item, kSplitMark)) {
        if (item.empty()) continue;
        // 文字をenumに変換
        int actNum = std::stoi(item);
        printf("%d,", actNum);
        PlayerActionKind act = static_cast<PlayerActionKind>(actNum);
        // アクション種と一致しなかった場合
        if (act != PlayerActionKind::Punch &&
            act != PlayerActionKind::Kick) {
            printf("\n不正なデータが読み込まれた：%ls\n", item.c_str());
            assert(false && "不正なデータが読み込まれた");
            return ComboData_t();
        }
        // 保存
        data.emplace_back(act);
    }
    printf("\n");
    return data;
}

