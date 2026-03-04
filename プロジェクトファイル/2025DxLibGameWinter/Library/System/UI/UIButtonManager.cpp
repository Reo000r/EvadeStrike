#include "UIButtonManager.h"
#include "UIButton.h"

UIButtonManager::UIButtonManager()
{
	// 処理なし
}

UIButtonManager& UIButtonManager::GetInstance()
{
	static UIButtonManager manager;
	return manager;
}

void UIButtonManager::Init()
{
	_buttons.clear();
}

void UIButtonManager::Update()
{
	// 選択権を譲渡可能か
	bool canSelectMove = true;
	for (auto& btn : _buttons) {
		// 更新
		btn->Update(canSelectMove);
		// 選択権を渡していたら
		if (btn->IsSelectMovement()) {
			// 選択権を渡せないようにする
			canSelectMove = false;
		}
	}
}

void UIButtonManager::Draw() const
{
	for (auto& btn : _buttons) {
		btn->Draw();
	}
}

void UIButtonManager::AddButton(const std::shared_ptr<UIButton>& button)
{
	_buttons.emplace_back(button);
}

UIButtonManager::Action_t UIButtonManager::FetchAction()
{
	for (auto& btn : _buttons) {

		std::function<void()> action = btn->ConsumeActionIfPressed();
		if (action != nullptr) return action;
	}

	for (auto& btn : _buttons) {
		// 選択不可ならスキップ
		if (!btn->IsSelectable()) continue;
		// ボタンが押されていたら
		std::function<void()> action = btn->ConsumeActionIfPressed();
		if (action != nullptr) {
			// 登録されているアクションを返す
			return action;
		}
	}
	return nullptr;
}
