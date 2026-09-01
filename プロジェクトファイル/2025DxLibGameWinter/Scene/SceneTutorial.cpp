#include "SceneTutorial.h"
#include "SceneTitle.h"
#include "SceneGamePlay.h"
#include "Library/System/Tutorial/TutorialManager.h"
#include "Library/System/UI/UITutorialTableOfContents.h"
#include "Library/System/UI/UITutorialDescription.h"
#include "Library/System/UI/UITutorialControllerDisplay.h"
#include "Library/System/UI/UIConfirmPopup.h"
#include "Library/System/UI/UIManager.h"
#include "Library/System/Statistics.h"
#include "Objects/Character/Player/Player.h"

namespace {
	const std::string kStageCsvPath = "Data/CSV/TutorialStagePlaceData.csv";

	// 目次UIの配置(画面右下)
	const Position2 kTocAnchorPos =
		Position2(Statistics::kScreenWidth * 0.85f, Statistics::kScreenHeight * 0.62f);
	constexpr float kTocScale = 1.0f;

	// 説明文UIの配置(画面下中央)
	const Position2 kDescAnchorPos =
		Position2(Statistics::kScreenCenterWidth, Statistics::kScreenHeight * 0.92f);
	constexpr float kDescScale = 1.0f;

	// コントローラー表示UIの配置(画面左下)
	const Position2 kControllerAnchorPos =
		Position2(Statistics::kScreenWidth * 0.15f, Statistics::kScreenHeight * 0.82f);
	constexpr float kControllerScale = 2.0f;
}

SceneTutorial::SceneTutorial(bool showSkipConfirmation) :
	SceneGameBase(),
	_showSkipConfirmation(showSkipConfirmation),
	_isPopupActive(false),
	_skipToGamePlay(false),
	_tutorialManager(std::make_shared<TutorialManager>())
{
}

std::string SceneTutorial::GetStageCsvPath() const
{
	return kStageCsvPath;
}

BGMType SceneTutorial::GetBGMType() const
{
	return BGMType::Tutorial;
}

std::weak_ptr<TutorialManager> SceneTutorial::GetTutorialManagerForPlace()
{
	return _tutorialManager;
}

void SceneTutorial::OnAfterCommonInit()
{
	// ステージ生成実行により、TutorialEventWallは既にTutorialManagerへ登録済み
	// ここでチュートリアルcsvを読み込む
	_tutorialManager->Init(_enemyManager, _player, _justDodgeManager);

	auto toc = std::make_shared<UITutorialTableOfContents>(kTocAnchorPos, kTocScale);
	auto desc = std::make_shared<UITutorialDescription>(kDescAnchorPos, kDescScale);
	auto ctrl = std::make_shared<UITutorialControllerDisplay>(kControllerAnchorPos, kControllerScale);
	UIManager::GetInstance().RegisterUI(toc);
	UIManager::GetInstance().RegisterUI(desc);
	UIManager::GetInstance().RegisterUI(ctrl);
	_tocUI = toc;
	_descUI = desc;
	_controllerUI = ctrl;

	_tocUI.lock()->SetTopics(_tutorialManager->GetTocEntries());

	// 開始直後のUI内容を反映
	ApplyCurrentStepToUI();

	_isPopupActive = false;
	_skipToGamePlay = false;
	if (_showSkipConfirmation) {
		// ポップアップ表示中はプレイヤーの操作を無効にする
		if (!_player.expired()) {
			_player.lock()->SetControlEnabled(false);
		}
		auto popup = std::make_shared<UIConfirmPopup>(
			L"チュートリアルをスキップしますか？",
			[this]() {
				_skipToGamePlay = true;
				_isPopupActive = false;
				// 操作を再度有効にする
				if (!_player.expired()) {
					_player.lock()->SetControlEnabled(true);
				}
			},
			[this]() {
				_isPopupActive = false;
			}
		);
		UIManager::GetInstance().RegisterUI(popup);
		_isPopupActive = true;
	}
}

void SceneTutorial::OnAdditionalUpdate()
{
	if (_isPopupActive) {
		// ポップアップ表示中はチュートリアルの進行を止める
		return;
	}

	_tutorialManager->Update();

	if (_tutorialManager->ConsumeStepChangedFlag()) {
		ApplyCurrentStepToUI();
	}
}

void SceneTutorial::OnAdditionalStageDraw() const
{
	_tutorialManager->Draw();
}

bool SceneTutorial::CheckSceneCompleteCondition() const
{
	return _skipToGamePlay || _tutorialManager->IsFinished();
}

std::shared_ptr<SceneBase> SceneTutorial::CreateNextScene() const
{
	if (_skipToGamePlay) {
		return std::make_shared<SceneGamePlay>();
	}
	return std::make_shared<SceneTitle>();
}

void SceneTutorial::ApplyCurrentStepToUI()
{
	const TutorialStepData& step = _tutorialManager->GetCurrentStepData();
	std::vector<std::wstring> buttons = SplitByPlus(step.controllerInput);

	if (!_descUI.expired()) {
		_descUI.lock()->SetDescriptionText(step.description);
		// 出現中に対象ボタンが押されたら全文表示にする
		_descUI.lock()->SetSkipButtons(buttons);
	}
	if (!_controllerUI.expired()) {
		_controllerUI.lock()->SetActiveButtons(buttons);
	}
	if (!_tocUI.expired()) {
		_tocUI.lock()->SetCurrentRegion(_tutorialManager->GetCurrentRegionId());
	}
}

std::vector<std::wstring> SceneTutorial::SplitByPlus(const std::wstring& str)
{
	std::vector<std::wstring> result;
	if (str.empty() || str == L"None") return result;

	std::wstring current;
	for (wchar_t c : str) {
		if (c == L'+') {
			if (!current.empty()) result.push_back(current);
			current.clear();
		}
		else {
			current += c;
		}
	}
	if (!current.empty()) result.push_back(current);

	return result;
}