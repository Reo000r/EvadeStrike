#pragma once
#include "SceneGameBase.h"
#include <memory>
#include <string>
#include <vector>

class TutorialManager;
class UITutorialTableOfContents;
class UITutorialDescription;
class UITutorialControllerDisplay;
class UITutorialComboRoute;

/// <summary>
/// チュートリアルシーン
/// </summary>
class SceneTutorial final : public SceneGameBase
{
public:
	/// <summary>
	/// trueの場合シーン開始直後にスキップ確認ポップアップを表示する
	/// </param>
	/// </summary>
	SceneTutorial(bool showSkipConfirmation);
	~SceneTutorial() = default;

protected:

	std::string GetStageCsvPath() const override;
	BGMType GetBGMType() const override;
	std::weak_ptr<TutorialManager> GetTutorialManagerForPlace() override;

	void OnAfterCommonInit() override;
	void OnAdditionalUpdate() override;
	void OnAdditionalStageDraw() const override;

	bool CheckSceneCompleteCondition() const override;
	std::shared_ptr<SceneBase> CreateNextScene() const override;

private:

	/// <summary>
	/// TutorialManagerの現在のステップ内容を各UIへ反映する
	/// </summary>
	void ApplyCurrentStepToUI();

	/// <summary>
	/// +区切りの文字列をベクタに分割する
	/// </summary>
	static std::vector<std::wstring> SplitByPlus(const std::wstring& str);

	bool _showSkipConfirmation;
	bool _isPopupActive;
	bool _skipToGamePlay;

	// ステージ生成より前に必要なため、
	// コンストラクタの時点で生成しておく
	std::shared_ptr<TutorialManager> _tutorialManager;

	std::weak_ptr<UITutorialTableOfContents> _tocUI;
	std::weak_ptr<UITutorialDescription> _descUI;
	std::weak_ptr<UITutorialControllerDisplay> _controllerUI;
	std::weak_ptr<UITutorialComboRoute> _comboRouteUI;
};