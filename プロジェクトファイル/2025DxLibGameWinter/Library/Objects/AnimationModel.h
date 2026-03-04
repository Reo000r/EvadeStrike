#pragma once
#include "Model.h"
#include <string>
#include <memory>
#include <forward_list>

/// <summary>
/// アニメーションを行うモデル
/// </summary>
class AnimationModel final : public Model
{
public:
	/// <summary>
	/// アニメーションデータ構造体
	/// </summary>
	struct AnimData
	{
		int animIndex = -1;			// アニメーション番号(元データにおける)
		int attachNo = -1;			// アタッチ番号
		std::wstring animName = L"";	// アニメーションの名前
		float animSpeed = 1.0f;		// アニメーションの再生速度
		float frame = 0.0f;			// アニメーションの再生時間
		float totalFrame = 0.0f;	// アニメーションの総再生時間
		bool  isLoop = false;		// ループするか
		bool  isEnd = false;		// ループしない場合終了しているか

		float inputAcceptStartFrame = 0.0f; // 入力受付開始フレーム
		float inputAcceptEndFrame = 0.0f;   // 入力受付終了フレーム
	};

	AnimationModel();
	~AnimationModel();

	void Init(int model);
	void Update(const float gameSpeed);

	/// <summary>
	/// 最初に使用するアニメーションを設定
	/// </summary>
	/// <param name="animName"></param>
	void SetStartAnim(const std::wstring animName);
	/// <summary>
	/// ゲーム中で使用するアニメーションデータ
	/// </summary>
	void SetAnimData(const std::wstring animName, const float animSpeed, const bool isLoop, 
		float inputAcceptStartRate = 0.0f, 
		float inputAcceptEndRate = 1.0f);
	/// <summary>
	/// <para> アニメーション名を指定しアタッチ </para>
	/// <para> (ブレンドの進行状況が止まるため初期化する目的で使用) </para>
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void AttachAnim(const std::wstring animName, const bool isLoop);

	/// <summary>
	/// 指定されたアニメーションの更新
	/// </summary>
	/// <param name="data"></param>
	/// <param name="gameSpeed"></param>
	void UpdateAnim(std::shared_ptr<AnimData> data, const float gameSpeed);

	/// <summary>
	/// ブレンド比率の更新
	/// </summary>
	void UpdateAnimBlendRate(const float gameSpeed);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	/// <param name="gameSpeed"></param>
	void ChangeAnim(const std::wstring& animName, const bool isLoop, const float gameSpeed);
	/// <summary>
	/// アニメーションデータを指定してアニメーション変更を行う
	/// </summary>
	/// <param name="currentData"></param>
	/// <param name="prevData"></param>
	void ChangeAnim(std::shared_ptr<AnimData> currentData, std::shared_ptr<AnimData> prevData);

	/// <summary>
	/// 現在再生中のアニメーションの再生進捗をリセットする
	/// </summary>
	void ResetCurrentAnimProgress();

	/// <summary>
	/// <para> アニメーションデータを名前で検索し参照を返す </para>
	/// <para> 見つからなかった場合は先頭のアニメーションデータを返す </para>
	/// <para> debugならassertを投げる </para>
	/// </summary>
	/// <param name="animName"></param>
	/// <returns></returns>
	std::shared_ptr<AnimData> FindAnimData(const std::wstring animName) const;

	std::shared_ptr<Model> GetModel() { return shared_from_this(); }

	std::shared_ptr<AnimData> GetCurrentAnimData() const { return FindAnimData(_currentAnimName); }
	std::wstring GetCurrentAnimName() const{ return _currentAnimName; }
	
	/// <summary>
	/// <para> 現在再生中のアニメーションのフレーム数を返す </para>
	/// <para> アニメーションが再生されていなければ0.0f </para>
	/// </summary>
	/// <returns></returns>
	float GetCurrentAnimFrame();

	/// <summary>
	/// 指定のアニメーションが終了しているか
	/// </summary>
	/// <param name="animName"></param>
	/// <returns></returns>
	bool IsEnd(const std::wstring animName) { return FindAnimData(animName)->isEnd; }
	/// <summary>
	/// 指定のアニメーションがループするか
	/// </summary>
	/// <param name="animName"></param>
	/// <returns></returns>
	bool IsLoop(const std::wstring animName) { return FindAnimData(animName)->isLoop; }

	/// <summary>
	/// <para> 再生中のアニメーションの進行度が入力を受け付ける範囲内にあるか </para>
	/// <para> 範囲を設定していない場合はデフォルトでtrueが返る </para>
	/// </summary>
	/// <returns></returns>
	bool CanAcceptCurrentAnimInput();

private:
	
	// アニメーションのforward_list(list)
	// 順番を気にする必要がないかつ
	// メモリの効率化のためforward_list
	std::forward_list<std::shared_ptr<AnimData>> _animDataList;

	// 現在再生中のアニメーション名
	std::wstring _currentAnimName;
	// 前に再生されていてブレンドアウトしていくアニメーション名
	std::wstring _prevAnimName;

	// アニメーションのブレンド比率
	// _currentAnimName のウェイトとして使用する
	// 0.0~1.0
	float _blendRate;
};

