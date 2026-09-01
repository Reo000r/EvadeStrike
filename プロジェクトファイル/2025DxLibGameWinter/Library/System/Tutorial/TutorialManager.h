#pragma once
#include "Loader/TutorialDataLoader.h"
#include <memory>
#include <string>
#include <vector>

class EnemyManager;
class Player;
class JustDodgeManager;
class TutorialEventWall;

/// <summary>
/// 目次1項目分のデータ
/// </summary>
struct TutorialTocEntry {
	int regionId = -1;   // -1または (tutorialEventId-100)/100
	std::wstring name;   // ContentsName
};

/// <summary>
/// <para> チュートリアルの進行を管理する </para>
/// <para> CSVで定義された順序に従って進行させる </para>
/// </summary>
class TutorialManager
{
public:
	TutorialManager();
	~TutorialManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(
		std::weak_ptr<EnemyManager> enemyManager,
		std::weak_ptr<Player> player,
		std::weak_ptr<JustDodgeManager> justDodgeManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 登録済みイベント壁の描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// トリガー状態のTutorialEventWallから接触時に呼ばれる
	/// </summary>
	/// <param name="tutorialWallId">接触した壁が持つチュートリアル区画ID</param>
	void OnTriggerWallTouched(int tutorialWallId);

	/// <summary>
	/// TutorialEventWallを登録する
	/// </summary>
	void RegisterTutorialWall(std::shared_ptr<TutorialEventWall> wall);

	/// <summary>
	/// チュートリアルが完了したか
	/// </summary>
	bool IsFinished() const { return _isFinished; }

	/// <summary>
	/// 現在表示すべきステップ情報を返す
	/// </summary>
	const TutorialStepData& GetCurrentStepData() const;

	/// <summary>
	/// 現在の区画IDを返す
	/// </summary>
	int GetCurrentRegionId() const;

	/// <summary>
	/// 目次に表示する全項目を返す
	/// </summary>
	std::vector<TutorialTocEntry> GetTocEntries() const { return _tocEntries; }

	/// <summary>
	/// <para> 前回の呼び出し以降にステップが変化したかを返す </para>
	/// <para> 呼び出すと内部フラグをリセットする </para>
	/// </summary>
	bool ConsumeStepChangedFlag();

private:

	/// <summary>
	/// 現在のステップのTutorialTypeに応じた完了条件を判定する
	/// </summary>
	bool CheckCurrentActionComplete() const;

	/// <summary>
	/// IDを進める
	/// </summary>
	void AdvanceStep();

	/// <summary>
	/// 指定した区画IDの完了を登録済みの全イベント壁へ通知する
	/// </summary>
	void NotifyRegionCleared(int tutorialWallId);

	/// <summary>
	/// 現在のステップがEnemySpawnInfo/EnemyBehaviorかつEventPos指定ありなら
	/// デモ用の敵を出現させる
	/// </summary>
	void TrySpawnDemoEnemyForCurrentStep();

	/// <summary>
	/// 指定IDに対応するステップ情報を検索する
	/// </summary>
	const TutorialStepData* FindStepById(int id) const;

	/// <summary>
	/// チュートリアルイベントIDから区画IDを算出する
	/// </summary>
	static int RegionIdFromTutorialEventId(int id);

	std::vector<TutorialStepData> _allSteps;    // Init時に一度だけロード
	std::vector<TutorialTocEntry> _tocEntries;  // Init時に一度だけ構築
	int _currentTutorialEventId;
	int _finalRegionId;   // _allStepsの最大区画IDから自動算出
	bool _stepChanged;
	bool _isFinished;
	bool _hasSpawnedDemoEnemyForCurrentStep;

	std::vector<std::shared_ptr<TutorialEventWall>> _registeredWalls;

	std::weak_ptr<EnemyManager> _enemyManager;
	std::weak_ptr<Player> _player;
	std::weak_ptr<JustDodgeManager> _justDodgeManager;

	// 不正なID要求時に返すためのダミーデータ
	TutorialStepData _emptyStepData;
};