#pragma once
#include "Library/Geometry/Vector2.h"
#include <vector>
#include <memory>

class UIBase;
class CameraShaker;

class UIManager
{
private:
	UIManager();
	UIManager(const UIManager&) = delete;
	void operator=(const UIManager&) = delete;

public:

	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static UIManager& GetInstance();

	void Init();
	void Update();
	void Draw();

	void RegisterUI(std::shared_ptr<UIBase> ui);
	void SetCameraShaker(std::weak_ptr<CameraShaker> cameraShaker) { _cameraShaker = cameraShaker; }
	Vector2 GetShakeOffset() const;

private:
	std::vector<std::shared_ptr<UIBase>> _uiList;
	std::weak_ptr<CameraShaker> _cameraShaker;

};

