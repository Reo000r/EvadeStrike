#include "EffectFactory.h"
#include "EffekseerEffect.h"
#include <filesystem>
#include <EffekseerForDXLib.h>

namespace {
	const std::string kSourceFolder = "Data/Effect/";	// ファイルを読む階層
	const std::string kEffectFileExtension = ".efkefc";	// ファイル拡張子
	constexpr float kDefaultEffectSize = 100.0f;
}

// staticメンバー変数の実体を定義
std::unordered_map<std::string, int> EffectFactory::_effectSourceHandles;

void EffectFactory::LoadResources()
{
	// 特定のフォルダ内にあるエフェクトファイルを一括で読み込む

	namespace fs = std::filesystem;
#ifdef _DEBUG
	printf("\n---エフェクトファイルの読み込みを開始---\n");
	int normalLoad = 0, abnormalLoad = 0;
#endif	// _DEBUG

	// 指定したフォルダの中のすべてのファイルを読み込む
	for (const fs::directory_entry& file : fs::directory_iterator(kSourceFolder)) {
		std::string path = file.path().string();
		// 相対パスではなくファイル名だけで取れるようにする
		std::string pathCopy = path;
		// パスを消したファイル名
		std::string fileName = pathCopy.erase(0, kSourceFolder.size());

		// もし想定された拡張子でないときは飛ばす
		if (file.path().extension().string() != kEffectFileExtension) {
#ifdef _DEBUG
			printf("想定された拡張子ではないファイルを検知\n");
			printf("　パス：%s\n", path.c_str());
			++abnormalLoad;
#endif	// _DEBUG
			continue;
		}

		// 読み込み
		const int handle = LoadEffekseerEffect(path.c_str(), kDefaultEffectSize);
		_effectSourceHandles[fileName] = handle;

#ifdef _DEBUG
		// 不正なハンドルなら
		if (handle == -1) {
			++abnormalLoad;
			assert(false && "読み込みに失敗");
		}
		else {
			++normalLoad;
		}
		printf("エフェクトファイルを読み込みました\n");
		printf("　相対パス　：%s\n", path.c_str());
		printf("　ファイル名：%s\n", fileName.c_str());
		printf("　ハンドル　：%d\n", handle);
#endif	// _DEBUG
	}

#ifdef _DEBUG
	printf("成功 %d, 失敗 %d\n", normalLoad, abnormalLoad);
	if (abnormalLoad > 0) printf("＊＊＊正常に読み込めなかったファイルが存在します＊＊＊\n");
	printf("---エフェクトファイルの読み込みを終了---\n\n");
#endif	// _DEBUG
}

void EffectFactory::ReleaseResources()
{
	// 読み込んだエフェクトハンドルを解放
	for (const std::pair<std::string, int>& pair : _effectSourceHandles) {
		DeleteEffekseerEffect(pair.second);
	}
	_effectSourceHandles.clear();
}

std::shared_ptr<EffekseerEffect> EffectFactory::GenerateEffect(const std::string& fileName, const Position3& initPos)
{
	// 元となるハンドルを取得
	int sourceHandle = _effectSourceHandles[fileName];

	// 生成されたエフェクト
	std::shared_ptr<EffekseerEffect> effect;

	// エフェクトを生成しハンドルを取得
	int effectHandle = PlayEffekseer3DEffect(sourceHandle);
	// エフェクトの生成に失敗していたら
	if (effectHandle == -1) {
		assert(false && "エフェクトの生成に失敗");
		return effect;	// 不完全なエフェクトを返す
	}

	// インスタンスを生成し初期化を行う
	effect = std::make_shared<EffekseerEffect>();
	effect->Init(effectHandle, initPos);

	// 生成したインスタンスを返す
	return effect;
}
