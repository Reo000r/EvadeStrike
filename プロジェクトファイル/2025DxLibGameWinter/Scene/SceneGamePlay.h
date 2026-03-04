#pragma once
#include "SceneBase.h"
#include <memory>

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

class SceneGamePlay final : public SceneBase
{
public:
	SceneGamePlay();
	~SceneGamePlay() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画全般
	/// </summary>
	void Draw() const override;

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


	int _frame;

	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void (SceneGamePlay::*)();
	using DrawFunc_t = void (SceneGamePlay::*)() const;
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	enum class NextSceneName {
		Title,
		Result,
	};

	NextSceneName _nextSceneName;
	std::shared_ptr<SceneBase> _nextScene;

private:

	std::shared_ptr<Physics> _physics;
	std::shared_ptr<Camera> _camera;
	std::shared_ptr<ObjectHandleHolder> _objectHandleHolder;
	std::shared_ptr<StagePlacer> _stagePlacer;
	std::shared_ptr<EventManager> _eventManager;
	std::shared_ptr<Skydome> _skydome;
	std::shared_ptr<GameObjectManager> _gameObjectManager;
	std::shared_ptr<EnemyManager> _enemyManager;
	std::shared_ptr<JustDodgeManager> _justDodgeManager;	// ジャスト回避管理
	std::shared_ptr<ShadowGenerator> _shadowGenerator;
	std::weak_ptr<Player> _player;
	
	std::shared_ptr<DebugField> _debugField;
};

