#include "AnimationModel.h"
#include "Model.h"

#include <DxLib.h>
#include <cassert>

namespace {
	// アニメーションのブレンド時間(frame)
	constexpr float kAnimBlendFrame = 5.0f;
	// 全てにかかるアニメーションの再生速度(60fなら1.0、30fなら0.5が等速)
	constexpr float kAnimSpeed = 1.0f;
}

AnimationModel::AnimationModel() :
	_animDataList(),
	_currentAnimName(L""),
	_prevAnimName(L""),
	_blendRate(1.0f) // 最初はブレンドしていないので1.0
{
}

AnimationModel::~AnimationModel()
{
}

void AnimationModel::Init(int model)
{
	assert(model >= 0 && "モデルハンドルが正しくない");
	_handle = model;
}

void AnimationModel::Update(const float gameSpeed)
{
	// ブレンド処理が進行中か、
	// そうでなければ現在のアニメーションを更新
	UpdateAnimBlendRate(gameSpeed);
}

void AnimationModel::SetStartAnim(const std::wstring animName)
{
	// 最初のアニメーションを現在のものとして設定
	_currentAnimName = animName;
	std::shared_ptr<AnimData> currentAnim = FindAnimData(_currentAnimName);
	AttachAnim(_currentAnimName, currentAnim->isLoop);

	// ブレンドは不要なので、ウェイトを100%にする
	_blendRate = 1.0f;
	MV1SetAttachAnimBlendRate(
		_handle,
		FindAnimData(_currentAnimName)->attachNo, 
		_blendRate);
}

void AnimationModel::SetAnimData(const std::wstring animName, const float animSpeed, 
	const bool isLoop, float inputAcceptStartRate, float inputAcceptEndRate)
{
	// すでに同じアニメーションが登録されていないか確認
	for (const std::shared_ptr<AnimData> anim : _animDataList) {
		if (animName == anim->animName) {
			assert(false && "同一のアニメーションを登録しようとしている");
			return;
		}
	}
	std::shared_ptr<AnimData> animData = std::make_shared<AnimData>();
	animData->animIndex = MV1GetAnimIndex(_handle, animName.c_str());
	if (animData->animIndex < 0) {
		assert(false && "存在しないアニメーションを登録しようとしている");
	}
	animData->attachNo = -1;	// 実際に使う際に更新する
	animData->animName = animName;
	animData->animSpeed = animSpeed * kAnimSpeed;
	animData->frame = 0.0f;
	animData->totalFrame = MV1GetAnimTotalTime(_handle, animData->animIndex);
	animData->isLoop = isLoop;
	animData->isEnd = false;
	// 比率をフレーム値に変換
	animData->inputAcceptStartFrame = animData->totalFrame * inputAcceptStartRate;
	animData->inputAcceptEndFrame = animData->totalFrame * inputAcceptEndRate;
	_animDataList.emplace_front(animData);
}

void AnimationModel::AttachAnim(const std::wstring animName, const bool isLoop)
{
	// アニメーション名が空なら何もしない
	if (animName.empty()) return;

	std::shared_ptr<AnimData> animData = FindAnimData(animName);

	// すでにアタッチ済みなら何もしない
	if (animData->attachNo >= 0) return;

	// モデルにアニメーションをアタッチ
	animData->attachNo = MV1AttachAnim(_handle, animData->animIndex, -1, false);
	assert(animData->attachNo >= 0 && "アニメーションのアタッチ失敗");
	animData->frame = 0.0f;
	animData->isLoop = isLoop;
	animData->isEnd = false;
	// 再生時間をリセット
	MV1SetAttachAnimTime(_handle, animData->attachNo, 0.0f);
}

void AnimationModel::UpdateAnim(std::shared_ptr<AnimData> data, const float gameSpeed)
{
	// アニメーションがアタッチされていない場合return
	if (data->attachNo == -1) return;
	// ゲームスピードを考慮してアニメーションを進める
	data->frame += data->animSpeed * gameSpeed;
	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = data->totalFrame;
	
	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (data->isLoop) {
		// アニメーションをループさせる
		while (data->frame > totalTime) {
			data->frame -= totalTime;
		}
	}
	else {
		// 最後のフレームで停止させる
		if (data->frame > totalTime) {
			data->frame = totalTime;
			data->isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(_handle, data->attachNo, data->frame);
}

void AnimationModel::UpdateAnimBlendRate(const float gameSpeed)
{
	// 現在のアニメーションを進める
	if (!_currentAnimName.empty()) {
		UpdateAnim(FindAnimData(_currentAnimName), gameSpeed);
	}

	// ブレンド中かどうか
	if (_blendRate < 1.0f) {
		// 古いアニメーションのフレームは更新せず
		// 影響度だけを下げる

		// ブレンド率を更新
		if (kAnimBlendFrame) _blendRate += 1.0f / kAnimBlendFrame * gameSpeed;

		// ブレンドが完了した場合の処理
		if (_blendRate >= 1.0f) {
			_blendRate = 1.0f;

			// 古いアニメーションは完全に不要になったのでデタッチ
			if (!_prevAnimName.empty()) {
				std::shared_ptr<AnimData> prevAnim = FindAnimData(_prevAnimName);
				if (prevAnim->attachNo != -1) {
					MV1DetachAnim(_handle, prevAnim->attachNo);
					prevAnim->attachNo = -1;
				}
				// 前のアニメーション名をクリアしてブレンド処理を終了
				_prevAnimName.clear();
			}
		}
	}

	// モデルにブレンド率を適用
	if (!_currentAnimName.empty()) {
		MV1SetAttachAnimBlendRate(_handle, FindAnimData(_currentAnimName)->attachNo, _blendRate);
	}
	if (!_prevAnimName.empty()) {
		MV1SetAttachAnimBlendRate(_handle, FindAnimData(_prevAnimName)->attachNo, 1.0f - _blendRate);
	}
}

void AnimationModel::ChangeAnim(const std::wstring& animName, const bool isLoop, const float gameSpeed)
{
	// 既に再生中、または遷移しようとしているアニメーションならreturn
	if (animName == _currentAnimName) return;

	// もし現在ブレンド中なら、
	// そのブレンド元のアニメーションは不要になるのでデタッチ
	if (!_prevAnimName.empty()) {
		std::shared_ptr<AnimData> prevAnim = FindAnimData(_prevAnimName);
		if (prevAnim->attachNo != -1) {
			MV1DetachAnim(_handle, prevAnim->attachNo);
			prevAnim->attachNo = -1;
		}
	}

	// 今まで再生していたアニメーションを前のアニメーションにし、
	// 新しいアニメーションを現在のアニメーションにする
	_prevAnimName = _currentAnimName;
	_currentAnimName = animName;
	// 新しいアニメーションをアタッチ
	AttachAnim(_currentAnimName, isLoop);
	_blendRate = 0.0f;

	// アタッチしたばかりのアニメーションに少しだけ影響力を持たせる
	UpdateAnimBlendRate(gameSpeed);
}

void AnimationModel::ChangeAnim(std::shared_ptr<AnimData> currentData, std::shared_ptr<AnimData> prevData)
{
	// 再生中側
	// 元のデータをデタッチ
	if (!_currentAnimName.empty()) {
		std::shared_ptr<AnimData> currentAnim = FindAnimData(_currentAnimName);
		if (currentAnim->attachNo != -1) {
			MV1DetachAnim(_handle, currentAnim->attachNo);
			currentAnim->attachNo = -1;
		}
	}
	// 新しいアニメーションに置き換える
	_currentAnimName = currentData->animName;
	// 新しいアニメーションをアタッチ
	AttachAnim(_currentAnimName, currentData->isLoop);
	
	// ブレンドアウト側
	// 元のデータをデタッチ
	if (!_prevAnimName.empty()) {
		std::shared_ptr<AnimData> prevAnim = FindAnimData(_prevAnimName);
		if (prevAnim->attachNo != -1) {
			MV1DetachAnim(_handle, prevAnim->attachNo);
			prevAnim->attachNo = -1;
		}
	}
	// 新しいアニメーションに置き換える
	_prevAnimName = prevData->animName;
	// 新しいアニメーションをアタッチ
	AttachAnim(_prevAnimName, prevData->isLoop);

	_blendRate = 0.0f;
}

void AnimationModel::ResetCurrentAnimProgress()
{
	auto animData = GetCurrentAnimData();
	animData->frame = 0.0f;
	animData->isEnd = false;
}

std::shared_ptr<AnimationModel::AnimData> AnimationModel::FindAnimData(const std::wstring animName) const
{
	// アニメーション名が空の場合はnullptrを返す
	if (animName.empty()) {
		assert(false && "指定のアニメーション名は空");
		return nullptr;
	}

	// 検索
	for (std::shared_ptr<AnimData> data : _animDataList) {
		if (animName == data->animName)  return data;
	}

	printf("指定の名前のアニメーションが登録されていなかった\n");
	printf("該当名：%ls\n", animName.c_str());
	assert(false && "指定の名前のアニメーションが登録されていなかった");
	return nullptr;
}

float AnimationModel::GetCurrentAnimFrame()
{
	if (_currentAnimName.empty()) return 0.0f;
	return FindAnimData(_currentAnimName)->frame;
}

bool AnimationModel::CanAcceptCurrentAnimInput()
{
	// 再生中のアニメーション
	const std::shared_ptr<AnimData> data = FindAnimData(_currentAnimName);
	// 範囲内ならtrue
	return (data->frame > data->inputAcceptStartFrame && 
			data->frame < data->inputAcceptEndFrame);
}
