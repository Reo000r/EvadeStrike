#include "SceneResult.h"
#include "SceneTitle.h"
#include "SceneGamePlay.h"
#include "SceneController.h"
#include "Library/System/Statistics.h"
#include "Library/System/SoundManager.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/UI/UIButtonManager.h"
#include "Library/System/UI/UIButton.h"
#include "Library/System/UI/UIManager.h"
#include "Library/System/UI/UIGraph.h"
#include "Library/System/UI/UIResultScoreDrawer.h"
#include "ResultDataHolder.h"
#include <DxLib.h>
#include <cassert>
#include <EffekseerForDXLib.h>

SceneResult::SceneResult() :
    _frame(Statistics::kFadeInterval),
    _nowUpdateState(&SceneResult::FadeinUpdate),
    _nowDrawState(&SceneResult::FadeDraw),
    _nextSceneName(NextSceneName::Title),
    _nextScene(nullptr)
{
}

void SceneResult::Init()
{
    SoundManager::GetInstance().PlaySoundType(BGMType::Result);
    UIManager::GetInstance().Init();
    UIButtonManager::GetInstance().Init();

    // ボタンの共通フェードアウト設定用ラムダ
    auto startFadeout = [this](NextSceneName next) {
        _nextSceneName = next;
        _nowUpdateState = &SceneResult::FadeoutUpdate;
        _nowDrawState = &SceneResult::FadeDraw;
        _frame = 0;
        };

    // リザルト
    int resultHandle = LoadGraph(L"Data/Graph/EvadeStrike_Result.png");
    auto result = std::make_shared<UIButton>(
        resultHandle,
        Vector2(Statistics::kScreenWidth * 0.5f,
            Statistics::kScreenHeight * 0.2f),
        0.5f * Statistics::kScreenWidthFullHDRatio,
        [](){}
    );
    // リトライボタン
    int retryHandle = LoadGraph(L"Data/Graph/EvadeStrike_Retry.png");
    auto retry = std::make_shared<UIButton>(
        retryHandle,
        Vector2(Statistics::kScreenWidth * 0.5f,
            Statistics::kScreenHeight * 0.77f),
        0.2f * Statistics::kScreenWidthFullHDRatio,
        [this, startFadeout]() { startFadeout(NextSceneName::Retry); }
    );

    // タイトルへボタン
    int titleHandle = LoadGraph(L"Data/Graph/EvadeStrike_Title.png");
    auto title = std::make_shared<UIButton>(
        titleHandle,
        Vector2(Statistics::kScreenWidth * 0.5f,
            Statistics::kScreenHeight * 0.85f),
        0.2f * Statistics::kScreenWidthFullHDRatio,
        [this, startFadeout]() { startFadeout(NextSceneName::Title); }
    );

    // 終了ボタン
    int exitHandle = LoadGraph(L"Data/Graph/EvadeStrike_Exit.png");
    auto exit = std::make_shared<UIButton>(
        exitHandle,
        Vector2(Statistics::kScreenWidth * 0.5f,
            Statistics::kScreenHeight * 0.93f),
        0.2f * Statistics::kScreenWidthFullHDRatio,
        []() {
            // 終了処理
            EffectManager::GetInstance().Terminate();
            SoundManager::GetInstance().Terminate();
            Effkseer_End();
            DxLib_End();
            return;
        }
    );

    // ボタンの上下関係の設定
    retry->SetDown(title);
    title->SetUp(retry);
    title->SetDown(exit);
    exit->SetUp(title);

    retry->SetSelected(true);

    result->SetSelectable(false);
    result->SetAnimType(UIAnimType::None);

    UIButtonManager::GetInstance().AddButton(result);
    UIButtonManager::GetInstance().AddButton(retry);
    UIButtonManager::GetInstance().AddButton(title);
    UIButtonManager::GetInstance().AddButton(exit);

    // UI
    auto resultBack = std::make_shared<UIGraph>(
        ResultDataHolder::GetInstance().GetLastGraphHandle(),
        Position2(Statistics::kScreenWidth * 0.3f, Statistics::kScreenHeight * 0.45f),
        true, 1.4f, 0.5f);
    UIManager::GetInstance().RegisterUI(resultBack);

    auto scoreDrawer = std::make_shared<UIResultScoreDrawer>();
    UIManager::GetInstance().RegisterUI(scoreDrawer);
}

void SceneResult::Update() 
{ 
    (this->*_nowUpdateState)();
}

void SceneResult::Draw() const 
{
    (this->*_nowDrawState)();
}

void SceneResult::FadeinUpdate()
{
    _frame--;
    if (_frame <= 0) {
        _nowUpdateState = &SceneResult::NormalUpdate;
        _nowDrawState = &SceneResult::NormalDraw;
    }
}

void SceneResult::NormalUpdate()
{
    UIButtonManager::GetInstance().Update();
    auto action = UIButtonManager::GetInstance().FetchAction();
    if (action != nullptr) {
        action();
    }
}

void SceneResult::FadeoutUpdate()
{
    _frame++;
    if (_frame >= Statistics::kFadeInterval) {
        if (_nextSceneName == NextSceneName::Retry) {
            EffectManager::GetInstance().DeleteAllEffect();
            _nextScene = std::make_shared<SceneGamePlay>();
        }
        else if (_nextSceneName == NextSceneName::Title) {
            EffectManager::GetInstance().DeleteAllEffect();
            _nextScene = std::make_shared<SceneTitle>();
        }

        if (_nextScene) {
            SceneController::GetInstance().ChangeScene(_nextScene);
        }
    }
}

void SceneResult::FadeDraw() const
{
    UIManager::GetInstance().Draw();
    UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
    DrawFormatString(0, 0, 0xffffff, L"Scene Result");
#endif

    float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
    SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
    DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneResult::NormalDraw() const
{
    UIManager::GetInstance().Draw();
    UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
    DrawFormatString(0, 0, 0xffffff, L"Scene Result");
#endif
}