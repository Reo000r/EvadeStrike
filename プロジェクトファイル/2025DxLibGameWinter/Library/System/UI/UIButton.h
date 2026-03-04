#pragma once
#include <memory>
#include <functional>
#include <string>
#include "Library/Geometry/Vector2.h"

// UIオブジェクトの基本型
enum class UIAnimType {
	Scale,      // 拡縮
	Sway,       // 横揺れ
	Rotate,     // 回転
	None        // なし
};

class UIButton : public std::enable_shared_from_this<UIButton>
{
public:
	using Action_t = std::function<void()>;
	/// <summary>
	/// テキスト用コンストラクタ
	/// </summary>
	/// <param name="text"></param>
	/// <param name="pos"></param>
	/// <param name="fontHandle"></param>
	/// <param name="baseScale"></param>
	/// <param name="action"></param>
	UIButton(
		const std::wstring& text,
		const Vector2& pos,
		int fontHandle,
		float baseScale,
		Action_t action
	);
	/// <summary>
	/// 画像用コンストラクタ
	/// </summary>
	/// <param name="imageHandle"></param>
	/// <param name="pos"></param>
	/// <param name="baseScale"></param>
	/// <param name="action"></param>
	UIButton(
		int imageHandle, 
		const Vector2& pos, 
		float baseScale, 
		Action_t action);
	~UIButton();

	void Update(bool canSelectMovement);
	void Draw() const;

	// 選択状態
	void SetSelected(bool selected);
	bool IsSelectMovement() const { return _isSelectMovement; }
	bool IsSelected() const { return _isSelected; }

	// 選択可否設定
	void SetSelectable(bool selectable) { _isSelectable = selectable; }
	bool IsSelectable() const { return _isSelectable; }
	void SetAnimType(UIAnimType type) { _animType = type; }

	void SetAnimSpeed(float speed) { _animSpeed = speed; }
	void SetScaleRange(float min, float max) { _minScale = min; _maxScale = max; }
	void SetSwayParam(float amount, float speedRate) { _swayAmount = amount; _swaySpeedRate = speedRate; }

	/// <summary>
	/// <para> 押されていたらactionを返す </para>
	/// <para> でなければnullptr </para>
	/// </summary>
	/// <returns></returns>
	Action_t ConsumeActionIfPressed();

	// 方向指定
	void SetUp(const std::shared_ptr<UIButton>& btn) { _up = btn; }
	void SetDown(const std::shared_ptr<UIButton>& btn) { _down = btn; }
	void SetLeft(const std::shared_ptr<UIButton>& btn) { _left = btn; }
	void SetRight(const std::shared_ptr<UIButton>& btn) { _right = btn; }

private:
	/// <summary>
	/// 入力更新
	/// </summary>
	void HandleMoveInput();

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimation();

private:
	std::wstring _text;
	Vector2 _pos;
	int _fontHandle;

	// 画像ハンドル
	int _imageHandle;
	// 画像モード判定
	bool _isImageType; 


	Action_t _action;

	bool _isSelected;
	bool _isSelectMovement;
	// 選択可能かどうか
	bool _isSelectable;

	// アニメーション用
	float _baseScale;
	float _currentScale;
	float _animTime;

	float _animSpeed;
	float _minScale;
	float _maxScale;
	float _swayAmount;
	float _swaySpeedRate;
	float _rotAngle;

	// アニメーションの種類
	UIAnimType _animType; 
	// 横揺れの基準位置用
	float _defaultX;

	// 隣接ボタン
	std::weak_ptr<UIButton> _up;
	std::weak_ptr<UIButton> _down;
	std::weak_ptr<UIButton> _left;
	std::weak_ptr<UIButton> _right;
};
