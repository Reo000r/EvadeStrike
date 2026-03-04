#include "InputStateHolder.h"
#include <cassert>

namespace {
	// 保存する最大要素数
	constexpr int kMaxHoldRecord = 30;
}

InputStateHolder::InputStateHolder() :
	_records(kMaxHoldRecord)
{
	// 要素がない場合は
	if (_records.empty()) {
		// 要素を一つだけ追加しておく
		_records.emplace_back(Input::GetInstance().GetCurrentInputRecord());
	}
}

InputStateHolder& InputStateHolder::GetInstance()
{
	static InputStateHolder holder;
	return holder;
}

void InputStateHolder::Update()
{
	Input& input = Input::GetInstance();
	// 現在の入力情報を取得
	Input::InputRecord_t currentRecord = input.GetCurrentInputRecord();
	// 保存可能な要素数と同等なら
	// 最も古い情報(末尾要素)を削除
	if (_records.size() == kMaxHoldRecord) {
		_records.pop_back();
	}
	// もしくは超えていたら
	else if (_records.size() > kMaxHoldRecord) {
		// 最大要素数-1以降の情報を削除
		_records.erase(_records.begin() + kMaxHoldRecord - 1, _records.end());
	}
	// 先頭に要素追加
	_records.emplace_front(currentRecord);
	// 要素があったなら追加
	if (_currentRecords.size() > 0) {
		_currentRecords.emplace_front(currentRecord);
	}
	// もし想定サイズを超えていたら
	if (_records.size() > kMaxHoldRecord) {
		assert(false && "指定要素数を超えている");
		// 最大要素数以降の情報を削除
		_records.erase(_records.begin() + kMaxHoldRecord, _records.end());
	}
}

int InputStateHolder::GetMaxHoldRecord() const
{
	return kMaxHoldRecord;
}

Input::InputRecord_t InputStateHolder::GetInputRecord(int num) const
{
	// もし指定された要素番目が存在しないものなら
	if (_records.max_size() > num) {
		assert(false && "指定要素番目が存在しない");
		// maxsize-1(最後の要素番目)にする
		num = static_cast<int>(_records.max_size()) - 1;
	}
	// 0以下なら
	else if (num < 0) {
		assert(false && "0以下の要素番目は指定不可");
		// 0(最初の要素番目)にする
		num = 0;
	}
	// 指定された要素を返す
	return _records[num];
}

int InputStateHolder::GetPressFrame(std::string inputName, float frame) const
{
	Input& input = Input::GetInstance();
	// もし探索数が最大入力保存数を超えていたら
	if (kMaxHoldRecord < frame) {
		assert(false && "フレームの指定が広すぎる");
		frame = kMaxHoldRecord - 1;
	}
	// 探索
	for (int i = 0; i < frame; ++i) {
		// 押されていた場合は
		if (input.IsPress(_records[i], inputName)) {
			return i;
		}
	}
	// 押されていなかった場合は
	return -1;
}

void InputStateHolder::StartCurrentRecords()
{
	// 要素をすべて削除
	ResetCurrentRecords();
	Input& input = Input::GetInstance();
	// 現在の入力情報を取得
	Input::InputRecord_t currentRecord = input.GetCurrentInputRecord();
	// 要素追加
	_currentRecords.emplace_front(currentRecord);
}

void InputStateHolder::ResetCurrentRecords()
{
	// 要素をすべて削除
	_currentRecords.clear();
}

