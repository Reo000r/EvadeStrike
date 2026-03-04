#pragma once
#include "SceneBase.h"
#include "Library/Geometry/Vector3.h"

#include <memory>

class UIButtonManager;

class SceneTitle final : public SceneBase
{
public:
	SceneTitle();
	~SceneTitle() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画全般
	/// </summary>
	void Draw() const override;

private:

	/// <summary>
	/// フェードイン時の更新処理
	/// </summary>
	void FadeinUpdate();
	/// <summary>
	/// 通常時の更新処理
	/// </summary>
	void NormalUpdate();
	/// <summary>
	/// フェードアウト時の更新処理
	/// </summary>
	void FadeoutUpdate();

	/// <summary>
	/// フェード時の描画
	/// </summary>
	void FadeDraw() const;
	/// <summary>
	/// 通常時の描画
	/// </summary>
	void NormalDraw() const;
	

	int _frame;

	// _updateや_drawが変数であることを分かりやすくしている
	using UpdateFunc_t = void (SceneTitle::*)();
	using DrawFunc_t = void (SceneTitle::*)() const;
	UpdateFunc_t _nowUpdateState = nullptr;
	DrawFunc_t _nowDrawState = nullptr;

	enum class NextSceneName {
		GamePlay,
	};

	NextSceneName _nextSceneName;
	std::shared_ptr<SceneBase> _nextScene;

private:

};