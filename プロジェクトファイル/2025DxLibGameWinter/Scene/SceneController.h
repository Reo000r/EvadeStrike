#pragma once
#include <memory>
#include <list>

class SceneBase;

/// <summary>
/// 各シーンを管理する
/// シングルトン化する
/// </summary>
class SceneController final
{
private:

	// シングルトン
	SceneController();
	SceneController(const SceneController&) = delete;
	void operator=(const SceneController&) = delete;

	/// <summary>
	/// 実行中のシーンのポインタ
	/// </summary>
	using SceneStace_t = std::list<std::shared_ptr<SceneBase>>;
	SceneStace_t _scenes;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>SceneControllerシングルトンオブジェクト</returns>
	static SceneController& GetInstance();

	/// <summary>
	/// Applicationから呼び出されるUpdate
	/// 内部のSceneのUpdateを呼び出す
	/// </summary>
	void Update();

	/// <summary>
	/// Applicationから呼び出されるDraw
	/// 内部のSceneのDrawを呼び出す
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 次の状態をセットする(セットするのは各状態の役割)
	/// </summary>
	/// <param name="scene">次の状態</param>
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// 現在実行中のシーンの上に別のシーンを乗せる
	/// </summary>
	/// <param name="scene">乗せたいシーン</param>
	void PushScene(std::shared_ptr<SceneBase> scene);

	/// <summary>
	/// 現在表面(最上部)にある実行中のシーンを取り除く
	/// </summary>
	void PopScene();
};

