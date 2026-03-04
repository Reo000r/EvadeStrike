#include "GameObjectManager.h"
#include "Library/Objects/GameObject.h"
#include "Objects/Character/Player/Player.h"
#include <cassert>

GameObjectManager::GameObjectManager() :
	_gameObjects()
{
	// 処理なし
}

GameObjectManager::~GameObjectManager()
{
	// 処理なし
}

void GameObjectManager::Entry(std::shared_ptr<GameObject> object)
{
	// 登録されているか検索
	auto it = std::find(_gameObjects.begin(), _gameObjects.end(), object);
	// 登録されていたなら
	if (it != _gameObjects.end()) {
		assert(false && "指定のobjectは登録済");
		return;	// 処理を中断
	}
	// 登録
	_gameObjects.emplace_back(object);
}

void GameObjectManager::Release(std::shared_ptr<GameObject> object)
{
	// 登録解除(eraseif 要C++20)
	size_t count = std::erase_if(_gameObjects,
		[object](std::shared_ptr<GameObject> target) { return target == object; });
	// 登録されてなかったらassert
	if (count <= 0) {
		assert(false && "指定のobjectは未登録");
	}
}

void GameObjectManager::Init()
{
	// MEMO:Managerはライブラリなのでゲーム固有の処理はなるべく置きたくない
	// 「ゲーム開始時に必要なオブジェクトを生成する」クラスを作るべき

	for (auto& object : _gameObjects) {
		object->Init();
	}
}

void GameObjectManager::Update()
{
	for (auto& object : _gameObjects) {
		object->UpdateTimeScale();
		object->Update();
	}
}

void GameObjectManager::Draw() const
{
	for (auto& object : _gameObjects) {
		object->Draw();
	}
}
