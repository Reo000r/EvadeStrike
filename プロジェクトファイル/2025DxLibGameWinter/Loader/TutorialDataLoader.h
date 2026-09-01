#pragma once
#include "Library/Geometry/Vector3.h"
#include <string>
#include <vector>

/// <summary>
/// チュートリアルの各ステップで行わせるアクション種別
/// </summary>
enum class TutorialType {
	Explain,        // 解説
	Camera,         // カメラ操作
	Move,           // 移動
	EnemySpawnInfo, // 敵出現ロジックの解説
	Attack,         // 攻撃
	Combo,          // コンボ攻撃
	Evade,          // 回避
	JustEvade,      // ジャスト回避
	EnemyBehavior,  // 敵
	Score,          // スコア
};

/// <summary>
/// チュートリアル説明CSV1行分のデータ
/// </summary>
struct TutorialStepData {
	int tutorialEventId = -1;                          // 主キー(チュートリアルイベントID)
	std::wstring contentsName = L"-1";                 // 目次名("-1"なら目次に表示しない)
	TutorialType type = TutorialType::Explain;          // チュートリアルタイプ
	std::wstring description;                          // 説明文(\nは改行として解釈済み)
	std::wstring controllerInput = L"None";            // コントローラー入力表示("+"区切り、"None"は非表示)
	Position3 eventPos = Position3(0.0f, 0.0f, 0.0f);   // デモ敵の出現位置
	bool hasEventPos = false;                          // EventPosX,Y,Zが明示的に指定されていたか
};

/// <summary>
/// <para> チュートリアルCSVを読み込むローダー</para>
/// </summary>
class TutorialDataLoader
{
public:
	TutorialDataLoader();
	virtual ~TutorialDataLoader();

	/// <summary>
	/// 指定のファイルからチュートリアルステップ情報を取得
	/// </summary>
	/// <param name="fileName">パス</param>
	/// <returns></returns>
	static std::vector<TutorialStepData> LoadData(const std::string& fileName);

private:

	/// <summary>
	/// 文字列をTutorialTypeへ変換する
	/// </summary>
	static TutorialType StringToTutorialType(const std::string& str);

	/// <summary>
	/// <para> ダブルクォートを考慮して1行をカンマ区切りで分割する </para>
	/// <para> ダブルクォートで囲まれたフィールド内のカンマは区切りとして扱わない </para>
	/// </summary>
	static std::vector<std::string> SplitCsvLine(const std::string& line);

	/// <summary>
	/// 文字列中の\nを改行文字に変換する
	/// </summary>
	static std::wstring ReplaceEscapedNewline(const std::wstring& str);
};