#include "UIManager.h"
#include "UIBase.h"
#include "Objects/Camera/CameraShaker.h"
#include <cassert>

namespace {
	// 振動量の補正
	// 1.0でカメラと同じ量
	constexpr float kUIShakeMul = 0.7f;
}

UIManager::UIManager()
{
	// 処理なし
}

UIManager& UIManager::GetInstance()
{
	static UIManager manager;
	return manager;
}

void UIManager::Init()
{
	_uiList.clear();
}

void UIManager::Update()
{
	// 削除可能なUIを削除
	size_t count = std::erase_if(
		_uiList,
		[](std::shared_ptr<UIBase> target) { return target->CanDelete(); });

	// 更新
	auto list = _uiList;
	for (auto& ui : list) {
		ui->Update();
	}
}

void UIManager::Draw()
{
	Vector2 shakeOffset = GetShakeOffset();
	for (auto& ui : _uiList) {
		Vector2 offset = Vector2(0, 0);
		// 振動を行うなら振動量を充てる
		if (ui->IsShake()) offset = shakeOffset;
		ui->Draw(offset);
	}
}

void UIManager::RegisterUI(std::shared_ptr<UIBase> ui)
{
	// 登録されていれば
	if (std::find(_uiList.begin(), _uiList.end(), ui) != 
		_uiList.end()) {
		assert(false && "指定のUIBaseは登録済み");
		return;
	}

	// 初期化し登録
	ui->Init();
	_uiList.emplace_back(ui);
}

Vector2 UIManager::GetShakeOffset() const
{
	// 無効なポインタならオフセットなし
	if (_cameraShaker.expired()) {
		return Vector2(0, 0);
	}

	Vector3 rawOffset = _cameraShaker.lock()->GetCurrentOffset();
	Vector2 shakeOffset = Vector2(rawOffset.x, rawOffset.y) * kUIShakeMul;
	return shakeOffset;
}
