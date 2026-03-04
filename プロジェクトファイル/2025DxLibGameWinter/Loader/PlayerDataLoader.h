#pragma once
#include "Objects/Character/Player/PlayerComboHolder.h"
#include "Objects/Character/Player/PlayerAttackAnimData.h"
#include "Library/Objects/AttackColStats.h"
#include <unordered_map>

class PlayerDataLoader
{
public:
	PlayerDataLoader();
	~PlayerDataLoader() = default;

	/// <summary>
	/// データを読み込む
	/// </summary>
	void Load();

	/// <summary>
	/// <para> IDを指定してデータを取得 </para>
	/// <para> 存在していなかった場合はidが-1のデータが返る </para>
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	PlayerAttackAnimData GetAttackAnimData(int id);
	/// <summary>
	/// 保存しているデータを返す
	/// </summary>
	/// <returns></returns>
	std::unordered_map<int, PlayerAttackAnimData> GetAttackAnimDataList();
	/// <summary>
	/// 攻撃前状態での攻撃先のデータを返す
	/// </summary>
	/// <returns></returns>
	PlayerAttackAnimData GetBeforeAtackAnimData() const;

	/// <summary>
	/// <para> 攻撃種別を指定してデータを取得 </para>
	/// <para> 不正な種別の場合はkindがnoneのデータが返る </para>
	/// </summary>
	/// <param name="kind"></param>
	/// <returns></returns>
	AttackColStats GetAttackStats(PlayerActionKind kind);
	/// <summary>
	/// 保存しているデータを返す
	/// </summary>
	/// <returns></returns>
	std::unordered_map<PlayerActionKind, AttackColStats> GetAttackStatsList();

	int GetSpecialAttackID() const;

private:

	/// <summary>
	/// 攻撃アニメーションデータを読み込む
	/// </summary>
	/// <returns></returns>
	void AttackAnimDataLoad();
	/// <summary>
	/// 引数の文字列をデータ化して返す
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	PlayerAttackAnimData AttackAnimDataSplit(std::wstring str);

	/// <summary>
	/// 攻撃データを読み込む
	/// </summary>
	/// <returns></returns>
	void AttackStatsLoad();
	/// <summary>
	/// 引数の文字列をデータ化して返す
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	AttackColStats AttackStatsSplit(std::wstring str);

	std::unordered_map<int, PlayerAttackAnimData> _attackAnimDataMap;
	std::unordered_map<PlayerActionKind, AttackColStats> _attackStatsMap;
};

