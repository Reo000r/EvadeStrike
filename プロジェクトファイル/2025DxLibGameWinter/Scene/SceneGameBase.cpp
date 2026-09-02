#include "SceneGameBase.h"
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
#include "ScenePause.h"

#include <DxLib.h>
#include <cassert>


SceneGameBase::SceneGameBase() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneGameBase::FadeinUpdate),
	_nowDrawState(&SceneGameBase::FadeDraw),
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

void SceneGameBase::Init()
{
	SoundManager::GetInstance().PlaySoundType(GetBGMType());

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
	// TutorialManagerはSceneTutorialのみ有効な値を返し、それ以外は空のweak_ptr
	auto enemySpawnDataList = _stagePlacer->Place(
		_physics, _eventManager, GetStageCsvPath(), GetTutorialManagerForPlace());

	// 初期化
	_eventManager->Init(_physics, _enemyManager, enemySpawnDataList);

	_camera->SetStageObjectColliders(_stagePlacer->GetStageColliders(), _eventManager->GetEventColliders());

	// 影設定
	_shadowGenerator->Init(_player);

	// シーン固有の追加初期化
	OnAfterCommonInit();

	_frame = Statistics::kFadeInterval;
	_nowUpdateState = &SceneGameBase::FadeinUpdate;
	_nowDrawState = &SceneGameBase::FadeDraw;
}

void SceneGameBase::Update()
{
	(this->*_nowUpdateState)();
}

void SceneGameBase::Draw() const
{
	(this->*_nowDrawState)();
}

void SceneGameBase::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneGameBase::NormalUpdate;
		_nowDrawState = &SceneGameBase::NormalDraw;

		// シーン固有のフェードイン完了時処理
		OnFadeInComplete();
	}
}

void SceneGameBase::NormalUpdate()
{
	if (CheckSceneCompleteCondition()) {
		_nowUpdateState = &SceneGameBase::FadeoutUpdate;
		_nowDrawState = &SceneGameBase::FadeDraw;
		_frame = 0;
		return;
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

	// シーン固有の追加更新
	OnAdditionalUpdate();

	TimeScaleManager::GetInstance().Update();
}

void SceneGameBase::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		EffectManager::GetInstance().DeleteAllEffect();
		_nextScene = CreateNextScene();

		if (!_nextScene) {
			assert(false && "次のシーンが不明");
		}
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;
	}
}

void SceneGameBase::FadeDraw() const
{
	// シャドウマップに描画
	_shadowGenerator->ShadowDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	OnAdditionalStageDraw();
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
	OnAdditionalStageDraw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_enemyManager->DrawAttackRangeIndicators();
	_shadowGenerator->NormalDrawEnd();
	// 影なしの通常の手前描画
	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene GameBase(Fade)");
#endif

	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGameBase::NormalDraw() const
{
	// シャドウマップに描画
	_shadowGenerator->ShadowDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	OnAdditionalStageDraw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	EffectManager::GetInstance().Draw();
	_shadowGenerator->ShadowDrawEnd();
	// 影なしの通常の奥描画
	_skydome->Draw();
	// 通常の描画
	_shadowGenerator->NormalDrawStart();
	_stagePlacer->Draw();
	_gameObjectManager->Draw();
	_eventManager->Draw();
	OnAdditionalStageDraw();
	_enemyManager->Draw();
	_justDodgeManager->Draw();
	_enemyManager->DrawAttackRangeIndicators();
	EffectManager::GetInstance().Draw();
	_shadowGenerator->NormalDrawEnd();
	// 影なしの通常の手前描画
	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();
}