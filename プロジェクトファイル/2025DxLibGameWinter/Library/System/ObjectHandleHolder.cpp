#include "ObjectHandleHolder.h"
#include <vector>
#include <DxLib.h>

namespace {
    const std::wstring kModelPathBase = L"Data/Model/";
    const std::wstring kStageModelPath = kModelPathBase + L"Stage/";
    const std::wstring kCharacterModelPath = kModelPathBase + L"Character/";

    // 名前, パス
    const std::unordered_map<std::string, std::wstring>
        kModelPathList = {
        //{"House_A", kStageModelPath + L"House_A.mv1"},
        //{"House_B", kStageModelPath + L"House_B.mv1"},
        //{"House_C", kStageModelPath + L"House_C.mv1"},
        //{"House_D", kStageModelPath + L"House_D.mv1"},
        //{"House_Corner_A", kStageModelPath + L"House_Corner_A.mv1"},
        //{"House_Corner_B", kStageModelPath + L"House_Corner_B.mv1"},

        {"Ground", kStageModelPath + L"Ground_Tilling.mv1"},
        {"Wall", kStageModelPath + L"Wall.mv1"},
        {"EventWall", kStageModelPath + L"Wall.mv1"},

        {"Player", kCharacterModelPath + L"PlayerModel.mv1"},
        {"EnemyWeak", kCharacterModelPath + L"EnemyWeakModel.mv1"},
    };
}

ObjectHandleHolder::ObjectHandleHolder() :
    _modelHandleList()
{
}

ObjectHandleHolder::~ObjectHandleHolder()
{
}

void ObjectHandleHolder::Load()
{
    _modelHandleList.clear();

    // 予めモデル読み込み
    for (const auto& pair : kModelPathList) {
        std::wstring path = pair.second.c_str();
        int handle = MV1LoadModel(path.c_str());
        _modelHandleList[pair.first] = handle;
    }
}

int ObjectHandleHolder::GetModelHandle(std::string name)
{
    for (const auto& pair : _modelHandleList) {
        // 名前が一致したら
        if (pair.first == name) {
            // モデル複製し返す
            return MV1DuplicateModel(pair.second);
        }
    }
    // 最後まで見つからなければ-1
    printf("指定の名前のモデルが見つからなかった : %s\n", name.c_str());
    return -1;
}
