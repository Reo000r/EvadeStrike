#pragma once
#include "SceneBase.h"
#include "Library/System/SoundManager.h"
#include <memory>
#include <string>

class GameObjectManager;
class EnemyManager;
class Camera;
class ObjectHandleHolder;
class StagePlacer;
class EventManager;
class Skydome;
class Physics;
class JustDodgeManager;
class Player;
class ShadowGenerator;
class DebugField;
class TutorialManager;

/// <summary>
/// メインゲーム部分に共通する処理をまとめた基底クラス
/// </summary>
class SceneGameBase abstract : public SceneBase
{
public:
	SceneGameBase();
	virtual ~SceneGameBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() final;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	void Update() final;

	/// <summary>
	/// 描画全般
	/// </summary>
	void Draw() const final;

protected:

	/// <summary>
	/// 読み込むステージ配置CSVのパスを返す
	/// </summary>
	virtual std::string GetStageCsvPath() const abstract;

	/// <summary>
	/// このシーンで再生するBGM種別を返す
	/// </summary>
	virtual BGMType GetBGMType() const abstract;

	/// <summary>
	/// <para> StagePlacer::Place()呼び出し時に渡すTutorialManagerを返す </para>
	/// <para> SceneTutorial以外は既定の空weak_ptrのままで問題ない </para>
	/// </summary>
	virtual std::weak_ptr<TutorialManager> GetTutorialManagerForPlace() { return std::weak_ptr<TutorialManager>(); }

	/// <summary>
	/// 共通初期化が終わった直後に呼ばれる
	/// </summary>
	virtual void OnAfterCommonInit() {}

	/// <summary>
	/// フェードインが完了した瞬間(通常状態に切り替わる瞬間)に呼ばれる
	/// </summary>
	virtual void OnFadeInComplete() {}

	/// <summary>
	/// 通常時更新処理の中、共通更新処理の後に呼ばれる
	/// </summary>
	virtual void OnAdditionalUpdate() {}

	/// <summary>
	/// 通常時描画処理の中、共通描画処理と同じタイミングで呼ばれる
	/// </summary>
	virtual void OnAdditionalStageDraw() const {}

	/// <summary>
	/// シーン終了条件を満たしたかどうか
	/// </summary>
	virtual bool CheckSceneCompleteCondition() const abstract;

	/// <summary>
	/// フェードアウト完了後に生成する次シーンを返す
	/// </summary>
	virtual std::shared_ptr<SceneBase> CreateNextScene() const abstract;


	int _frame;

	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void (SceneGameBase::*)();
	using DrawFunc_t = void (SceneGameBase::*)() const;
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	std::shared_ptr<SceneBase> _nextScene;

	std::shared_ptr<Physics> _physics;
	std::shared_ptr<Camera> _camera;
	std::shared_ptr<ObjectHandleHolder> _objectHandleHolder;
	std::shared_ptr<StagePlacer> _stagePlacer;
	std::shared_ptr<EventManager> _eventManager;
	std::shared_ptr<Skydome> _skydome;
	std::shared_ptr<GameObjectManager> _gameObjectManager;
	std::shared_ptr<EnemyManager> _enemyManager;
	std::shared_ptr<JustDodgeManager> _justDodgeManager;
	std::shared_ptr<ShadowGenerator> _shadowGenerator;
	std::weak_ptr<Player> _player;

	std::shared_ptr<DebugField> _debugField;

private:

	/// <summary>
	/// フェードイン時の更新処理
	/// </summary>
	void FadeinUpdate();
	/// <summary>
	/// 通常時の更新処理
	/// </summary>
	void NormalUpdate();
	/// <summary>
	/// フェードアウト時の更新処理
	/// </summary>
	void FadeoutUpdate();

	/// <summary>
	/// フェード時の描画
	/// </summary>
	void FadeDraw() const;
	/// <summary>
	/// 通常時の描画
	/// </summary>
	void NormalDraw() const;
};