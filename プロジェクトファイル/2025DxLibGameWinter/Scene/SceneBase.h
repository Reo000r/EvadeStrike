#pragma once

class SceneController;

/// <summary>
/// シーンの基底クラス
/// 実体は持たない
/// </summary>
class SceneBase
{
public:
	SceneBase() {};

	/// <summary>
	/// 初期化
	/// コンストラクタで初期化するときに
	/// タイミングが悪かったりした場合の処理
	/// </summary>
	virtual void Init() abstract;

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	virtual void Update() abstract;

	/// <summary>
	/// 描画全般
	/// </summary>
	virtual void Draw() const abstract;
};