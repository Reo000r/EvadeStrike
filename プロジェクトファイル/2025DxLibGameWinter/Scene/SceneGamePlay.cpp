#include "SceneGamePlay.h"
#include "SceneResult.h"
#include "SceneTitle.h"
#include "ScenePause.h"
#include "SceneController.h"
#include "Library/Physics/Physics.h"
#include "Library/System/SoundManager.h"
#include "Library/System/TimeScaleManager.h"
#include "Library/System/GameObjectManager.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/DebugTools/DebugField.h"
#include "Objects/Camera/Camera.h"
#include "Objects/Camera/CameraStateFollow.h"
#include "Objects/Character/Player/Player.h"
#include "Objects/Character/Player/JustDodge/JustDodgeManager.h"
#include "Library/ShadowGenerator.h"
#include "Library/Objects/Skydome.h"
#include "Library/System/ObjectHandleHolder.h"
#include "Library/System/StagePlacer.h"
#include "Library/System/Event/EventManager.h"
#include "Library/System/Effect/EffectManager.h"

#include "Library/System/Input.h"
#include "Library/Geometry/Vector2.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/UI/UIManager.h"
#include "Library/System/UI/UIButtonManager.h"
#include "Library/System/UI/UIGameScoreDrawer.h"
#include "ResultDataHolder.h"

#include <DxLib.h>
#include <cassert>


SceneGamePlay::SceneGamePlay() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneGamePlay::FadeinUpdate),
	_nowDrawState(&SceneGamePlay::FadeDraw),
	_nextSceneName(NextSceneName::Title),
	_nextScene(nullptr),
	_physics(std::make_shared<Physics>()),
	_camera(std::make_shared<Camera>()),
	_objectHandleHolder(std::make_shared<ObjectHandleHolder>()),
	_stagePlacer(std::make_shared<StagePlacer>()),
	_eventManager(std::make_shared<EventManager>()),
	_skydome(std::make_shared<Skydome>()),
	_gameObjectManager(std::make_shared<GameObjectManager>()),
	_enemyManager(std::make_shared<EnemyManager>()),
	_shadowGenerator(std::make_shared<ShadowGenerator>()),
	_player(),
	_debugField(std::make_shared<DebugField>()),
	_justDodgeManager(std::make_shared<JustDodgeManager>())
{
}

void SceneGamePlay::Init()
{
	SoundManager::GetInstance().PlaySoundType(BGMType::GamePlay);

	// ゲームスピード初期化
	TimeScaleManager::GetInstance().Init();

	UIManager::GetInstance().Init();
	UIButtonManager::GetInstance().Init();
	ResultDataHolder::GetInstance().Init();

	// UI
	auto uiScore = std::make_shared<UIGameScoreDrawer>();
	UIManager::GetInstance().RegisterUI(uiScore);

	_gameObjectManager->Init();

	// モデル読み込み
	_objectHandleHolder->Load();

	// プレイヤー初期化
	std::shared_ptr<Player> player = std::make_shared<Player>(_physics);
	player->Init();
	_gameObjectManager->Entry(player);
	_player = player;

	uiScore->SetPlayerComboHolder(_player.lock()->GetComboHolder());
	_player.lock()->SetUIGameScoreDrawer(uiScore);

	// カメラ初期化
	std::shared_ptr<CameraStateBase> state =
		std::make_shared<CameraStateFollow>(_camera, _player.lock());
	_camera->Init(state);
	// UI管理者にカメラのオフセット情報を渡す
	UIManager::GetInstance().SetCameraShaker(_camera->GetShaker());
	_player.lock()->GeneratePlayerUI();
	// 敵管理者初期化
	_enemyManager->Init(
		_physics, _player.lock(), _camera,
		_objectHandleHolder, _eventManager, 
		_gameObjectManager, _justDodgeManager, uiScore);
	// 敵情報をプレイヤーに共有可能に
	_player.lock()->SetEnemyManager(_enemyManager);
	// プレイヤーにカメラ設定
	_player.lock()->SetCamera(_camera);
	// スカイドームにカメラ設定
	_skydome->Init(_camera);
	// ジャスト回避管理初期化
	_justDodgeManager->Init();

	_player.lock()->SetJustDodgeManager(_justDodgeManager);

	// ステージ配置
	_stagePlacer->SetObjectHandleHolder(_objectHandleHolder);
	// ステージ配置時に敵配置データを取得
	auto enemySpawnDataList = _stagePlacer->Place(_physics, _eventManager);
	
	// 初期化
	_eventManager->Init(_physics, _enemyManager, enemySpawnDataList);

	_camera->SetStageObjectColliders(_stagePlacer->GetStageColliders(), _eventManager->GetEventColliders());

	// 影設定
	_shadowGenerator->Init(_player);
}

void SceneGamePlay::Update()
{
	(this->*_nowUpdateState)();
}

void SceneGamePlay::Draw() const
{
	(this->*_nowDrawState)();
}

void SceneGamePlay::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneGamePlay::NormalUpdate;
		_nowDrawState = &SceneGamePlay::NormalDraw;

		// リザルト画面のデフォルトを設定しておく
		ResultDataHolder::GetInstance().ReserveCopyResultScreen();
	}
}

void SceneGamePlay::NormalUpdate()
{
	if ((Input::GetInstance().IsPress("Debug:NextScene1") &&
		Input::GetInstance().IsPress("Debug:NextScene2")) ||
		_eventManager->IsClearable()) {
		_nextSceneName = NextSceneName::Result;
		_nowUpdateState = &SceneGamePlay::FadeoutUpdate;
		_nowDrawState = &SceneGamePlay::FadeDraw;
		_frame = 0;
	}
	if (!_player.lock()->IsAlive()) {
		_nextSceneName = NextSceneName::Result;
		_nowUpdateState = &SceneGamePlay::FadeoutUpdate;
		_nowDrawState = &SceneGamePlay::FadeDraw;
		_frame = 0;
	}

	if (Input::GetInstance().IsTrigger("Debug:PauseScene")) {
		SceneController::GetInstance().PushScene(std::make_shared<ScenePause>());
		return;
	}

	_enemyManager->Update();
	_gameObjectManager->Update();
	_camera->Update();
	_eventManager->Update();
	_skydome->Update();

	_justDodgeManager->Update();

	_physics->Update();

	_shadowGenerator->Update();

#ifdef _DEBUG
	// 速度変更
	TimeScaleManager& manager = TimeScaleManager::GetInstance();
	float afterScale = 0.0f;
	if (Input::GetInstance().IsTrigger("Debug:Key1")) {
		afterScale = manager.GetPlayerCurrentScale() + 0.2f;
		manager.ChangeScale(afterScale);
		printf("Scale加算:%.2f\n", afterScale);
	}
	if (Input::GetInstance().IsTrigger("Debug:Key2")) {
		afterScale = manager.GetPlayerCurrentScale() - 0.2f;
		manager.ChangeScale(afterScale);
		printf("Scale減算:%.2f\n", afterScale);
	}
	if (Input::GetInstance().IsTrigger("Debug:Key3")) {
		afterScale = 1.0f;
		manager.ChangeScale(afterScale);
		printf("Scaleリセット:%.2f\n", afterScale);
	}
#endif // _DEBUG


	TimeScaleManager::GetInstance().Update();
}

void SceneGamePlay::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		if (_nextSceneName == NextSceneName::Title) {
			EffectManager::GetInstance().DeleteAllEffect();
			_nextScene = std::make_shared<SceneTitle>();
		}
		else if (_nextSceneName == NextSceneName::Result) {
			EffectManager::GetInstance().DeleteAllEffect();
			_nextScene = std::make_shared<SceneResult>();
		}
		else {
			assert(false && "次のシーンが不明");
		}

		if (!_nextScene) {
			assert(false && "次のシーンが不明");
		}
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;
	}
}

void SceneGamePlay::FadeDraw() const
{
	// シャドウマップに描画
	_shadowGenerator->ShadowDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_shadowGenerator->ShadowDrawEnd();
	// 影なしの通常の奥描画
	_skydome->Draw();
	// 通常の描画
	_shadowGenerator->NormalDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_shadowGenerator->NormalDrawEnd();
	// 影なしの通常の手前描画
	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif

	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGamePlay::NormalDraw() const
{
	// シャドウマップに描画
	_shadowGenerator->ShadowDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_shadowGenerator->ShadowDrawEnd();
	// 影なしの通常の奥描画
	_skydome->Draw();
	// 通常の描画
	_shadowGenerator->NormalDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_shadowGenerator->NormalDrawEnd();
	// 影なしの通常の手前描画
	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	//DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif
}
