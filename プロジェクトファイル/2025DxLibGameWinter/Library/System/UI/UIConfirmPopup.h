#pragma once
#include "UIBase.h"
#include <functional>
#include <string>

/// <summary>
/// 確認ポップアップUI
/// </summary>
class UIConfirmPopup : public UIBase
{
public:
	using Callback_t = std::function<void()>;

	UIConfirmPopup(
		const std::wstring& message,
		Callback_t onYes,
		Callback_t onNo);
	~UIConfirmPopup() override = default;

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

private:
	std::wstring _message;
	Callback_t _onYes;
	Callback_t _onNo;
	bool _isYesSelected;
};