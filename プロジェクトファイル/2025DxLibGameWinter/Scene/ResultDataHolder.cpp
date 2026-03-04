#include "ResultDataHolder.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	// 画面キャプチャ命令が来てから何フレーム待つか
	constexpr int kCopyScreenBuffer = 7;
}

ResultDataHolder::ResultDataHolder() :
	_score(0.0f),
	_lastGraphHandle(-1),
	_copyScreenProgress(-1)
{
	// 処理なし
}

ResultDataHolder& ResultDataHolder::GetInstance()
{
	static ResultDataHolder holder;
	return holder;
}

void ResultDataHolder::Init()
{
	_score = 0.0f;
	if (_lastGraphHandle >= 0) {
		DeleteGraph(_lastGraphHandle);
	}
	_lastGraphHandle = -1;
}

void ResultDataHolder::CopyResultScreen()
{
	if (_copyScreenProgress > 0) _copyScreenProgress--;
	if (_copyScreenProgress != 0) return;

	if (_lastGraphHandle >= 0) {
		DeleteGraph(_lastGraphHandle);
		_lastGraphHandle = -1;
	}

	int screenWidth = Statistics::kScreenWidth;
	int screenHeight = Statistics::kScreenHeight;
	// 新しい保存用ハンドルを作成
	_lastGraphHandle = MakeScreen(screenWidth, screenHeight, FALSE);

	// 現在の描画画面（裏画面など）の内容をコピー
	// GetDrawScreenGraph(左上x, 左上y, 右下x, 右下y, コピー先ハンドル)
	GetDrawScreenGraph(0, 0, screenWidth, screenHeight, _lastGraphHandle);

	_copyScreenProgress = -1;
}

void ResultDataHolder::ReserveCopyResultScreen()
{
	_copyScreenProgress = kCopyScreenBuffer;
}

void ResultDataHolder::AddScore(float score)
{
	_score += score;
	printf("スコア追加[%.01f] 合計[%.01f]\n", score, _score);
}
