#include "UIButton.h"
#include "Library/System/Input.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/SoundManager.h"
#include <DxLib.h>
#include <algorithm>

UIButton::UIButton(
	const std::wstring& text,
	const Vector2& pos,
	int fontHandle,
	float baseScale,
	Action_t action) :
	_text(text),
	_pos(pos),
	_fontHandle(fontHandle),
	_imageHandle(-1),
	_isImageType(false),
	_action(action),
	_isSelectMovement(false),
	_isSelected(false),
	_isSelectable(true),
	_baseScale(baseScale),
	_currentScale(baseScale),
	_animTime(0.0f),
	_animSpeed(5.0f),
	_minScale(1.0f),
	_maxScale(1.2f),
	_swayAmount(10.0f),
	_swaySpeedRate(0.3f),
	_rotAngle(0.0f),
	_animType(UIAnimType::Scale),
	_defaultX(0.0f)
{
}

UIButton::UIButton(
	int imageHandle, 
	const Vector2& pos, 
	float baseScale, 
	Action_t action) :
	_text(),
	_pos(pos),
	_fontHandle(-1),
	_imageHandle(imageHandle),
	_isImageType(true),
	_action(action),
	_isSelected(false),
	_isSelectMovement(false),
	_isSelectable(true),
	_baseScale(baseScale),
	_currentScale(baseScale),
	_animTime(0.0f),
	_animSpeed(5.0f),
	_minScale(1.0f),
	_maxScale(1.2f),
	_swayAmount(10.0f),
	_swaySpeedRate(0.3f),
	_rotAngle(0.0f),
	_animType(UIAnimType::Scale),
	_defaultX(pos.x)
{
}

UIButton::~UIButton()
{
	if (_imageHandle != -1) {
		DeleteGraph(_imageHandle);
		_imageHandle = -1;
	}
}

void UIButton::SetSelected(bool selected)
{
	_isSelected = selected;
	if (!selected)
	{
		_animTime = 0.0f;
		_currentScale = _baseScale;
	}
}


void UIButton::Update(bool canSelectMovement)
{
	// 選択権譲渡状態をリセット
	_isSelectMovement = false;

	// 選択不可オブジェクトでもアニメーションは実行
	if (!_isSelectable) {
		UpdateAnimation();
		return;
	}

	// 選択されていた場合は
	if (_isSelected) {
		// 移動可能な場合は
		if (canSelectMovement) {
			// 選択権移動
			HandleMoveInput();
		}
		// アニメーション
		UpdateAnimation();
	}
}

void UIButton::HandleMoveInput()
{
	if (Input::GetInstance().IsTrigger("System:Up")) {
		if (auto next = _up.lock()) {
			SetSelected(false);
			next->SetSelected(true);
			_isSelectMovement = true;
			return;
		}
	}
	else if (Input::GetInstance().IsTrigger("System:Down")) {
		if (auto next = _down.lock()) {
			SetSelected(false);
			next->SetSelected(true);
			_isSelectMovement = true;
			return;
		}
	}
	else if (Input::GetInstance().IsTrigger("System:Left")) {
		if (auto next = _left.lock()) {
			SetSelected(false);
			next->SetSelected(true);
			_isSelectMovement = true;
			return;
		}
	}
	else if (Input::GetInstance().IsTrigger("System:Right")) {
		if (auto next = _right.lock()) {
			SetSelected(false);
			next->SetSelected(true);
			_isSelectMovement = true;
			return;
		}
	}
}

void UIButton::UpdateAnimation()
{
	// 時間を進める
	_animTime += (1.0f / 60.0f) * _animSpeed;

	switch (_animType) {
	case UIAnimType::Scale: {
		// 拡縮の変動幅
		const float kScaleRange = _maxScale - _minScale;

		float t = (sinf(_animTime) + 1.0f) * 0.5f;

		// 最小値から最大値の間で線形補間
		_currentScale = _baseScale * (_minScale + (kScaleRange * t));
		break;
	}
	case UIAnimType::Sway: {
		// 微弱な横揺れ
		float offsetX = sinf(_animTime * _swaySpeedRate) * _swayAmount;
		// 描画位置を直接いじらず、描画時にオフセットを加算するようにする
		_pos.x = _defaultX + offsetX;
		break;
	}
	case UIAnimType::Rotate: {
		// 回転
		_rotAngle = _animTime * 0.1f;
		break;
	}
	case UIAnimType::None:
	default:
		_currentScale = _baseScale;
		break;
	}
}

UIButton::Action_t UIButton::ConsumeActionIfPressed()
{
	// 選択中かつ
	// 決定が押されたら
	if (_isSelected && Input::GetInstance().IsTrigger("System:Submit")) {
		SoundManager::GetInstance().PlaySoundType(SEType::Enter);
		return _action;
	}
	return nullptr;
}

void UIButton::Draw() const
{
	int color = 0xffffff;
	if (_isSelected) color = 0xffffaa;
	int alpha = 255; // デフォルトは不透明

	// 選択可能かつ
	// 現在選択されていない場合は半透明にする
	if (_isSelectable && !_isSelected) {
		alpha *= 0.7f;
	}

	// 画像の場合
	if (_isImageType) {
		// 画像描画の場合
		int w, h;
		GetGraphSize(_imageHandle, &w, &h);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRectRotaGraph(
			static_cast<int>(_pos.x), static_cast<int>(_pos.y), // 画面上の描画中心
			0, 0,												// 元画像の左上座標
			w, h,												// 元画像の幅と高さ
			_currentScale,										// 拡大率
			_rotAngle,											// 回転（ラジアン）
			_imageHandle,										// グラフィックハンドル
			TRUE,												// 透過フラグ
			FALSE												// 反転フラグ
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// テキストの場合
	else {
		int defaultFontSize = GetFontSize();
		// 現在のアニメーションスケールを適用したフォントサイズに設定
		int currentFontSize = static_cast<int>(24 * _currentScale);
		SetFontSize(currentFontSize);

		// 文字列の幅と高さを取得
		int stringWidth, stringHeight, lineCount;
		GetDrawStringSizeToHandle(&stringWidth, &stringHeight, &lineCount, _text.c_str(), static_cast<int>(_text.length()), _fontHandle);

		// 中心位置を基準に描画座標を計算
		int drawX = static_cast<int>(_pos.x - (stringWidth / 2.0f));
		int drawY = static_cast<int>(_pos.y - (stringHeight / 2.0f));

		// 文字描画
		DrawString(
			drawX,
			drawY,
			_text.c_str(),
			color,
			_fontHandle
		);

		SetFontSize(defaultFontSize);
	}
}
