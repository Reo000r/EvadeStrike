#pragma once
#include "UIBase.h"
#include <string>
#include <vector>

/// <summary>
/// チュートリアル用コントローラー表示UI
/// </summary>
class UITutorialControllerDisplay : public UIBase
{
public:
	UITutorialControllerDisplay(Position2 anchorPos, float scale);
	~UITutorialControllerDisplay() override = default;

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

	/// <summary>
	/// <para> 点滅させる対象ボタンを設定する </para>
	/// <para> 例: {"Y", "B"} なら Y と B のオーバーレイのみ点滅する </para>
	/// <para> A,B,X,Y,RB,LB,RT,LT,RS,LS,Pause </para>
	/// </summary>
	void SetActiveButtons(const std::vector<std::wstring>& buttonKeys);

	/// <summary>
	/// アンカー座標を設定する(画面左下を想定)
	/// </summary>
	void SetAnchorPos(Position2 pos) { SetCenterPos(pos); }

	/// <summary>
	/// <para> スケールを設定する(縦横一律) </para>
	/// <para> コントローラー画像とボタンサイズが比例して変わり、相対的な位置関係は保たれる </para>
	/// </summary>
	void SetScale(float scale) { _scale = scale; }

	/// <summary>
	/// 指定のボタンキーに対応する入力が今押されたかを返す
	/// </summary>
	static bool IsInputForButtonKey(const std::wstring& key);

private:

	/// <summary>
	/// ボタンオーバーレイ1つ分の情報
	/// </summary>
	struct ButtonOverlay {
		std::wstring key;					// "A","B","X","Y","RB","LB","RT","LT","RS","LS","Pause"
		int imageHandle = -1;
		Vector2 offsetFromAnchorBase;		// scaleが1の時のアンカーからの相対オフセット
		bool isActive = false;				// 現在のステップで点滅させる対象か
		float blinkTime = 0.0f;				// 点滅用の経過時間
		float flashTime = 0.0f;				// 実押下時のフラッシュ演出用の残り時間
	};

	/// <summary>
	/// ボタンオーバーレイの初期配置・画像読み込み
	/// </summary>
	void InitOverlays();

	int _controllerImageHandle;
	std::vector<ButtonOverlay> _overlays;
	float _scale;

	// 左右スティックの現在の傾き
	Vector2 _leftStickCurrentTilt;
	Vector2 _rightStickCurrentTilt;

#ifdef _DEBUG

	void DebugUpdate();
	void DebugDraw() const;

public:
	/// <summary>
	/// デバッグ調整モード中かどうか
	/// </summary>
	bool IsDebugAdjustMode() const { return _isDebugAdjustMode; }
private:
	bool _isDebugAdjustMode;
	int _debugSelectedIndex;
#endif
};