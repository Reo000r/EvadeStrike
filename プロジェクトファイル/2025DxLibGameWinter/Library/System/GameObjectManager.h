#pragma once
#include <vector>
#include <memory>

class GameObject;
class JustDodgeManager;

/// <summary>
/// ゲームで使用するGameObjectを管理するクラス
/// </summary>
class GameObjectManager final
{
public:
	GameObjectManager();
	~GameObjectManager();

	/// <summary>
	/// 登録
	/// </summary>
	void Entry(std::shared_ptr<GameObject> object);
	/// <summary>
	/// 解放
	/// </summary>
	void Release(std::shared_ptr<GameObject> object);

	/// <summary>
	/// GameObjectの初期生成
	/// </summary>
	void Init();
	/// <summary>
	/// GameObjectの更新
	/// </summary>
	void Update();
	/// <summary>
	/// GameObjectの描画
	/// </summary>
	void Draw() const;

	const std::vector<std::shared_ptr<GameObject>> GetGameObjects() const { return _gameObjects; }

private:
	// 管理しているGameObject
	std::vector<std::shared_ptr<GameObject>> _gameObjects;
};

