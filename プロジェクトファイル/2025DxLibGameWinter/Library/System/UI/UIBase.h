#pragma once
#include "Library/Geometry/Vector2.h"
#include <memory>

class UIManager;

class UIBase
{
public:

	UIBase(Position2 centerPos, bool isDraw, bool isShake = true) :
		_centerPos(centerPos),
		_isDraw(isDraw),
		_isShake(),
		_canDelete(false)
	{}
	virtual ~UIBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() abstract;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() abstract;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(Vector2 shakeOffset) abstract;
	
	Position2 GetCenterPos() const { return _centerPos; }
	void SetCenterPos(Position2 centerPos) { _centerPos = centerPos; }

	virtual void SetDrawState(bool isDraw) { _isDraw = isDraw; }
	bool IsShake() { return _isShake; }
	void SetDeleteState(bool canDelete) { _canDelete = canDelete; }
	bool CanDelete() { return _canDelete; }

protected:

	Position2 _centerPos;
	bool _isDraw;
	bool _isShake;
	bool _canDelete;
};

