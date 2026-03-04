#pragma once
#include "SceneBase.h"

class ScenePause : public SceneBase
{
public:
	ScenePause();

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

	bool _isPauseEnd;
};

