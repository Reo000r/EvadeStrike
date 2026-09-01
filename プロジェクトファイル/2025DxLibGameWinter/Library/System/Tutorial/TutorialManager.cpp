#include "TutorialManager.h"
#include "TutorialEventWall.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Objects/Character/Player/Player.h"
#include "Objects/Character/Player/PlayerComboHolder.h"
#include "Objects/Character/Player/JustDodge/JustDodgeManager.h"
#include "Library/System/Input.h"
#include <algorithm>
#include <cassert>

namespace {
	const std::string kTutorialDataPath = "Data/CSV/TutorialData.csv";

	// スティック入力を行ったとみなす閾値
	constexpr float kStickInputThreshold = 0.8f;

	constexpr int kTutorialIDDivValue = 100;

	// EnemySpawnInfo/EnemyBehaviorのデモ敵出現に使う暫定の敵種別
	// (将来、説明専用の敵クラス(遠距離攻撃のみ/ダメージ0近接攻撃のみ)が
	//  追加された際は、ここを差し替える)
	constexpr EnemyType kDemoEnemyType = EnemyType::Weak;
}

TutorialManager::TutorialManager() :
	_currentTutorialEventId(0),
	_finalRegionId(-1),
	_stepChanged(true),
	_isFinished(false),
	_hasSpawnedDemoEnemyForCurrentStep(false)
{
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Init(
	std::weak_ptr<EnemyManager> enemyManager,
	std::weak_ptr<Player> player,
	std::weak_ptr<JustDodgeManager> justDodgeManager)
{
	_enemyManager = enemyManager;
	_player = player;
	_justDodgeManager = justDodgeManager;

	// csvを読み込む
	_allSteps = TutorialDataLoader::LoadData(kTutorialDataPath);

	_currentTutorialEventId = 0;
	_stepChanged = true;
	_isFinished = false;
	_hasSpawnedDemoEnemyForCurrentStep = false;

	// 目次エントリの構築と最終区画番号の算出
	_tocEntries.clear();
	int maxRegionId = -1;
	for (const auto& step : _allSteps) {
		int regionId = RegionIdFromTutorialEventId(step.tutorialEventId);
		if (regionId > maxRegionId) maxRegionId = regionId;

		// ContentsNameが-1の行は目次に表示しない
		if (step.contentsName == L"-1") continue;

		TutorialTocEntry entry;
		entry.regionId = regionId;
		entry.name = step.contentsName;
		_tocEntries.push_back(entry);
	}
	_finalRegionId = maxRegionId;

	// 開始直後のステップがデモ敵出現を伴う場合に対応
	TrySpawnDemoEnemyForCurrentStep();
}

void TutorialManager::Update()
{
	// ステップ進行判定
	if (!_isFinished) {
		if (CheckCurrentActionComplete()) {
			AdvanceStep();
		}
	}

	// 登録済みイベント壁の更新
	for (auto& wall : _registeredWalls) {
		wall->Update();
	}
	// Physicsから解放させる
	for (auto& wall : _registeredWalls) {
		if (!wall->IsExpired()) continue;
		wall->ReleasePhysics();
	}
	// 削除対象になった壁をコンテナから一括削除
	std::erase_if(_registeredWalls, [](const std::shared_ptr<TutorialEventWall>& wall) {
		return wall->IsExpired();
		});
}

void TutorialManager::Draw() const
{
	for (const auto& wall : _registeredWalls) {
		wall->Draw();
	}
}

void TutorialManager::OnTriggerWallTouched(int tutorialWallId)
{
	// 現在値に関わらず無条件で上書きする
	_currentTutorialEventId = tutorialWallId * kTutorialIDDivValue + kTutorialIDDivValue;
	_stepChanged = true;
	_hasSpawnedDemoEnemyForCurrentStep = false;
	TrySpawnDemoEnemyForCurrentStep();
}

void TutorialManager::RegisterTutorialWall(std::shared_ptr<TutorialEventWall> wall)
{
	auto it = std::find(_registeredWalls.begin(), _registeredWalls.end(), wall);
	if (it == _registeredWalls.end()) {
		_registeredWalls.push_back(wall);
	}
	else {
		assert(false && "指定のTutorialEventWallは登録済");
	}
}

const TutorialStepData& TutorialManager::GetCurrentStepData() const
{
	const TutorialStepData* found = FindStepById(_currentTutorialEventId);
	if (found != nullptr) return *found;
	return _emptyStepData;
}

int TutorialManager::GetCurrentRegionId() const
{
	return RegionIdFromTutorialEventId(_currentTutorialEventId);
}

bool TutorialManager::ConsumeStepChangedFlag()
{
	bool changed = _stepChanged;
	_stepChanged = false;
	return changed;
}

bool TutorialManager::CheckCurrentActionComplete() const
{
	const TutorialStepData& step = GetCurrentStepData();

	switch (step.type) {
	case TutorialType::Explain:
	case TutorialType::EnemyBehavior:
	case TutorialType::Score:
		return Input::GetInstance().IsTrigger("System:Submit");

	case TutorialType::EnemySpawnInfo:
		return true;

	case TutorialType::Camera: {
		bool ret = Input::GetInstance().GetPadRightSitck().Magnitude() > kStickInputThreshold;
		// 満たしていれば操作を有効にする
		if (ret && !_player.expired()) {
			_player.lock()->SetControlEnabled(true);
		}
		return ret;
	}
		

	case TutorialType::Move:
		return Input::GetInstance().GetPadLeftStick().Magnitude() > kStickInputThreshold;

	case TutorialType::Attack:
		return Input::GetInstance().IsTrigger("Gameplay:Punch") ||
			Input::GetInstance().IsTrigger("Gameplay:Kick");

	case TutorialType::Combo: {
		if (_player.expired()) return false;
		std::weak_ptr<PlayerComboHolder> comboHolder = _player.lock()->GetComboHolder();
		if (comboHolder.expired()) return false;
		return comboHolder.lock()->CheckRawComboConsist();
	}

	case TutorialType::Evade:
		return Input::GetInstance().IsTrigger("Gameplay:Dodge");

	case TutorialType::JustEvade:
		if (_justDodgeManager.expired()) return false;
		return _justDodgeManager.lock()->IsJustDodge();

	default:
		break;
	}

	return false;
}

void TutorialManager::AdvanceStep()
{
	int nextId = _currentTutorialEventId + 1;
	const TutorialStepData* nextStep = FindStepById(nextId);

	// 次のIDに対応する行があればそのまま1つ進める
	if (nextStep != nullptr) {
		_currentTutorialEventId = nextId;
		_stepChanged = true;
		_hasSpawnedDemoEnemyForCurrentStep = false;
		TrySpawnDemoEnemyForCurrentStep();
		return;
	}

	// 現在の区画の最終ステップ

	int regionId = GetCurrentRegionId();
	NotifyRegionCleared(regionId);
	// TutorialEventIdはそのまま据え置き
	// regionId>=0の場合は次のトリガー壁接触(OnTriggerWallTouched)まで進行しない
	// regionId==-1の場合も区画0のトリガー壁に触れるまで進行しない

	// 最終区画の完了ならチュートリアル完了
	if (regionId == _finalRegionId) {
		_isFinished = true;
	}
}

void TutorialManager::NotifyRegionCleared(int tutorialWallId)
{
	for (auto& wall : _registeredWalls) {
		wall->OnCall(tutorialWallId, "TutorialComplete");
	}
}

void TutorialManager::TrySpawnDemoEnemyForCurrentStep()
{
	if (_hasSpawnedDemoEnemyForCurrentStep) return;

	const TutorialStepData& step = GetCurrentStepData();
	if (step.type != TutorialType::EnemySpawnInfo &&
		step.type != TutorialType::EnemyBehavior) {
		return;
	}
	if (!step.hasEventPos) return;
	if (_enemyManager.expired()) return;

	// 敵出現
	_enemyManager.lock()->SpawnEnemy(kDemoEnemyType, step.eventPos);
	_hasSpawnedDemoEnemyForCurrentStep = true;
}

const TutorialStepData* TutorialManager::FindStepById(int id) const
{
	for (const auto& step : _allSteps) {
		if (step.tutorialEventId == id) return &step;
	}
	return nullptr;
}

int TutorialManager::RegionIdFromTutorialEventId(int id)
{
	int ret = -1;
	if (id >= kTutorialIDDivValue) {
		ret = (id - kTutorialIDDivValue) / kTutorialIDDivValue;
	}
	return ret;
}