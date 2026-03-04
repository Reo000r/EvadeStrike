#pragma once
#include <memory>
#include <map>

class GameObjectManager;

/// <summary>
/// <para> ゲームシーンの進行速度を管理するシングルトンクラス </para>
/// <para> MEMO:全体のゲームスピードと個別のゲームスピードを変えたい場合があるのでそれに対応させたい </para>
/// <para> (例外のものに対して通常の速度から変化後の速度に対しての影響値を設定するなど) </para>
/// </summary>
class TimeScaleManager final
{
private:
	TimeScaleManager();
	TimeScaleManager(const TimeScaleManager&) = delete;
	void operator=(const TimeScaleManager&) = delete;

public:

	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static TimeScaleManager& GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 指定したフレームかけて指定のタイムスケールに変更する
	/// すべてのオブジェクトに変更を加える
	/// </summary>
	/// <param name="scale">変更後のタイムスケール</param>
	/// <param name="frame">タイムスケールが変わる速度(フレーム)</param>
	void ChangeScale(const float scale, const int frame = 1);
	/// <summary>
	/// 指定したフレームかけて指定のタイムスケールに変更する
	/// プレイヤーのみ変更を加える
	/// </summary>
	/// <param name="scale">変更後のタイムスケール</param>
	/// <param name="frame">タイムスケールが変わる速度(フレーム)</param>
	void ChangePlayerScale(const float scale, const int frame = 1);
	/// <summary>
	/// 指定したフレームかけて指定のタイムスケールに変更する
	/// プレイヤー以外のオブジェクトのみ変更を加える
	/// </summary>
	/// <param name="scale">変更後のタイムスケール</param>
	/// <param name="frame">タイムスケールが変わる速度(フレーム)</param>
	void ChangeOtherScale(const float scale, const int frame = 1);

	/// <summary>
	/// 現在のタイムスケールを取得する
	/// </summary>
	/// <returns></returns>
	float GetPlayerCurrentScale() const { return _playerData.currentScale; }
	/// <summary>
	/// 現在のプレイヤー以外のタイムスケールを取得する
	/// </summary>
	/// <returns></returns>
	float GetOtherCurrentScale() const { return _otherData.currentScale; }

	/// <summary>
	/// 通常のタイムスケールを取得する
	/// </summary>
	/// <returns></returns>
	float GetDefaultScale();

private:

	std::weak_ptr<GameObjectManager> _objectManager;

	struct ScaleData {
		float currentScale;		// 現在のゲームスピード
		float addScaleValue;	// 1fあたりのゲームスピード変更量
		float nextScale;		// 更新後のゲームスピード

		void Update() {
			// 追加ゲームスピード量があるなら更新
			if (addScaleValue != 0.0f) {
				// すでに目標の速度を越していたら
				if (((currentScale - nextScale) >= 0.0f && addScaleValue > 0.0f) ||
					((currentScale - nextScale) <= 0.0f && addScaleValue < 0.0f)) {
					// 追加速度を0にし、速度を目標速度にする
					addScaleValue = 0.0f;
					currentScale = nextScale;
					return;
				}
				// ゲームスピードを変更
				currentScale += addScaleValue;
			}
			// scaleが0を下回っていた場合は0にする
			if (currentScale < 0.0f) {
				currentScale = 0.0f;
			}
		}
	};

	ScaleData _playerData;	// プレイヤーのタイムスケールデータ
	ScaleData _otherData;	// その他のタイムスケールデータ

};

