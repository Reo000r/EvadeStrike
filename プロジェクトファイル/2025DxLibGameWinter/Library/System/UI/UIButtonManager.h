#pragma once
#include <vector>
#include <memory>
#include <functional>

class UIButton;

class UIButtonManager
{
private:
	UIButtonManager();
	UIButtonManager(const UIButtonManager&) = delete;
	void operator=(const UIButtonManager&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static UIButtonManager& GetInstance();

	void Init();
	void Update();
	void Draw() const;
	
	void AddButton(const std::shared_ptr<UIButton>& button);
	
	using Action_t = std::function<void()>;
	// 押されたボタンの役割を返す（なければ nullptr）
	Action_t FetchAction();

private:
	std::vector<std::shared_ptr<UIButton>> _buttons;
};
