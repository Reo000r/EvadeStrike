#include "SceneController.h"
#include "SceneTitle.h"
#include "SceneGamePlay.h"

SceneController::SceneController()
{
	// 一番最初のシーンだけは割り当てておく
	ChangeScene(std::make_shared<SceneTitle>());
	//ChangeScene(std::make_shared<SceneGamePlay>());
}

SceneController& SceneController::GetInstance()
{
	static SceneController controller;
	return controller;
}

void SceneController::Update()
{
	// 一番上のUpdateのみ実行
	// 下層のUpdateは眠っておいてもらう
	_scenes.back()->Update();
}

void SceneController::Draw() const
{
	// Drawは全て実行
	for (const std::shared_ptr<SceneBase>& scene : _scenes) {
		scene->Draw();
	}
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	if (_scenes.empty()) {
		// 空だった場合は先頭に置く
		_scenes.emplace_back(scene);
	}
	else {
		// なにかある場合は最後の要素を置き換える
		_scenes.back() = scene;
	}
	scene->Init();
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	// 最後尾に追加
	_scenes.emplace_back(scene);

	// 初期化処理
	scene->Init();
}

void SceneController::PopScene()
{
	// 実行中のシーンが1つ以下なら
	if (_scenes.size() <= 1) {
		// 実行すべきシーンがなくなるため許可しない
		return;
	}

	// 最後尾を取り除く
	_scenes.pop_back();
}
