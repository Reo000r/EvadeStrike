#pragma once
#include "SceneGameBase.h"

/// <summary>
/// 通常のゲームシーン
/// </summary>
class SceneGamePlay final : public SceneGameBase
{
public:
	SceneGamePlay();
	~SceneGamePlay() = default;

protected:

	/// <summary>
	/// 読み込むステージ配置CSVのパスを返す
	/// </summary>
	std::string GetStageCsvPath() const override;
	/// <summary>
	/// このシーンで再生するBGM種別を返す
	/// </summary>
	BGMType GetBGMType() const override;

	/// <summary>
	/// フェードインが完了した瞬間(通常状態に切り替わる瞬間)に呼ばれる
	/// </summary>
	void OnFadeInComplete() override;
	/// <summary>
	/// 通常時更新処理の中、共通更新処理の後に呼ばれる
	/// </summary>
	void OnAdditionalUpdate() override;

	/// <summary>
	/// シーン終了条件を満たしたかどうか
	/// </summary>
	bool CheckSceneCompleteCondition() const override;
	/// <summary>
	/// フェードアウト完了後に生成する次シーンを返す
	/// </summary>
	std::shared_ptr<SceneBase> CreateNextScene() const override;
};
