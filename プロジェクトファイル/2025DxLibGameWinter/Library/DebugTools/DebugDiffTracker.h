#pragma once
#include <DxLib.h>
#include <string>
#include <map>

/// <summary>
/// 文字列と数字をペアで登録し、変更があれば表示するstaticクラス
/// </summary>
class DebugDiffTracker
{
public:
    static void Reset() {
        _valueMap.clear();
    }

    static void UpdateValue(const std::string& key, float newValue) {
        // 既存の値を検索
        auto it = _valueMap.find(key);

        if (it != _valueMap.end()) {
            // 前後の値を比較し変更されていれば
            if (it->second != newValue) {
#ifdef _DEBUG
                // デバッグ表示
                printf("DebugDiffTracker: [%s] 値が変更された (Before: %.2f, After: %.2f)\n",
                    key.c_str(), it->second, newValue);
#endif // _DEBUG
                // 値を更新
                it->second = newValue;
            }
        }
        else {
            // 初回登録
            _valueMap[key] = newValue;
        }
    }

private:
    static std::map<std::string, float> _valueMap;
};
