#include "TutorialDataLoader.h"
#include "Library/System/StringUtility.h"
#include <cassert>
#include <fstream>
#include <cstdio>

namespace {
	const std::string kHeader =
		"TutorialEventId,ContentsName,TutorialType,Description,ControllerInput,EventPosX,EventPosY,EventPosZ";

	// ステージの拡縮倍率
	constexpr float kStageScale = 1.75f;
	// 敵位置補正値
	constexpr float kModelTransformOffset = 100.0f * kStageScale;//100.0f;
}

TutorialDataLoader::TutorialDataLoader()
{
}

TutorialDataLoader::~TutorialDataLoader()
{
}

std::vector<TutorialStepData> TutorialDataLoader::LoadData(const std::string& fileName)
{
	// データ
	std::vector<TutorialStepData> steps;

	// ファイルを開く
	std::ifstream file(fileName.c_str());
	// もしもファイルを開けなかったら
	if (!file.is_open()) {
		assert(false && "ファイルが開けなかった\n");
		assert(false && "　path : %s\n", fileName.c_str());
		file.close();
		return steps;
	}

	// 1行分のデータ
	std::string line;

	// ヘッダ照合
	std::getline(file, line);
	if (line != kHeader) {
		assert(false && "ヘッダが一致していない\n");
		assert(false && "　path : %s\n", fileName.c_str());
		assert(false && "　header : %s\n", kHeader.c_str());
	}

	// 1行ずつ読み取る
	while (std::getline(file, line)) {
		// 空行はスキップ
		if (line.empty()) continue;

		std::vector<std::string> fields = SplitCsvLine(line);
		// 列数が足りない場合は空文字で埋めておく(EventPosが未記入の行を許容するため)
		while (fields.size() < 8) fields.push_back("");

		TutorialStepData step;

		// TutorialEventId
		if (!fields[0].empty()) {
			step.tutorialEventId = std::stoi(fields[0]);
		}

		// ContentsName( "-1" なら目次非表示のまま)
		std::wstring contentsName = StringUtility::GetWStringFromString(fields[1]);
		if (!contentsName.empty()) {
			step.contentsName = contentsName;
		}

		// TutorialType
		step.type = StringToTutorialType(fields[2]);

		// Description
		step.description = ReplaceEscapedNewline(StringUtility::GetWStringFromString(fields[3]));

		// ControllerInput
		std::wstring controllerInput = StringUtility::GetWStringFromString(fields[4]);
		if (!controllerInput.empty()) {
			step.controllerInput = controllerInput;
		}

		// EventPosX,Y,Z
		bool hasX = !fields[5].empty();
		bool hasY = !fields[6].empty();
		bool hasZ = !fields[7].empty();
		if (hasX && hasY && hasZ) {
			step.eventPos.x = std::stof(fields[5]) * kModelTransformOffset;
			step.eventPos.y = std::stof(fields[6]) * kModelTransformOffset;
			step.eventPos.z = std::stof(fields[7]) * kModelTransformOffset;
			step.hasEventPos = true;
		}

		// 配列に追加
		steps.emplace_back(step);
	}

	// ファイルを閉じて情報を返す
	file.close();
	return steps;
}

TutorialType TutorialDataLoader::StringToTutorialType(const std::string& str)
{
	if (str == "Explain")        return TutorialType::Explain;
	if (str == "Camera")         return TutorialType::Camera;
	if (str == "Move")           return TutorialType::Move;
	if (str == "EnemySpawnInfo") return TutorialType::EnemySpawnInfo;
	if (str == "Attack")         return TutorialType::Attack;
	if (str == "Combo")          return TutorialType::Combo;
	if (str == "Evade")          return TutorialType::Evade;
	if (str == "JustEvade")      return TutorialType::JustEvade;
	if (str == "EnemyBehavior")  return TutorialType::EnemyBehavior;
	if (str == "Score")          return TutorialType::Score;

	assert(false && "未対応のTutorialType:%s\n", str.c_str());
	return TutorialType::Explain;
}

std::vector<std::string> TutorialDataLoader::SplitCsvLine(const std::string& line)
{
	std::vector<std::string> fields;
	std::string field;
	bool inQuotes = false;

	for (size_t i = 0; i < line.size(); ++i) {
		char c = line[i];
		if (inQuotes) {
			// ダブルクォートの中
			if (c == '"') {
				// ""はエスケープされた"として扱う
				if (i + 1 < line.size() && line[i + 1] == '"') {
					field += '"';
					++i;
				}
				else {
					inQuotes = false;
				}
			}
			else {
				field += c;
			}
		}
		else {
			// ダブルクォートの外
			if (c == '"') {
				inQuotes = true;
			}
			else if (c == ',') {
				fields.push_back(field);
				field.clear();
			}
			else {
				field += c;
			}
		}
	}
	fields.push_back(field);
	return fields;
}

std::wstring TutorialDataLoader::ReplaceEscapedNewline(const std::wstring& str)
{
	std::wstring result;
	result.reserve(str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		// \nを実際の改行に変換
		if (str[i] == L'\\' && i + 1 < str.size() && str[i + 1] == L'n') {
			result += L'\n';
			++i;
		}
		else {
			result += str[i];
		}
	}
	return result;
}