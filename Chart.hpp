# pragma once

# include "Common.hpp"

//MPM:Measure Per Minute 一分当たりの小節数 エディタ側がBPMを考慮する(拍子を考慮しないでよくするため)
// 
//MPM変化位置 ほぼ構造体
class MPMChange {
public:
	double pos;//位置(整数部分:小節,小数部分:小節内の位置)
	double MPM;//変化先MPM
};

//MPM変化位置のリスト
class MPMChangeList {
private:
	Array<MPMChange> MPMList;//MPM変化のリスト
public:
	//MPM変化のリストに追加
	void addMPMChange(double pos, double MPM);
	//小節番号と小節内位置から秒数を取得
	double getSec(double pos);
};

//ノートの種類
enum class noteType {
	Unset,
	Tap,
	Trace_s,
	Trace_B,
	Swing,
};

//ノート
class Note {
private:
	noteType type;//ノート種別
	double timing;//判定ラインに重なるまでの秒数
	double left;//左位置(0~1)
	double right;//右位置(0~1)
public:
	bool beaten=false;
	//コンストラクタ
	//小節番号(1~),小節内での位置(0~1),ノート種別,左位置(0~1),右位置(0~1)
	Note(double pos, noteType type, double left, double right, MPMChangeList mpmCngList);
	std::tuple<double, double, double> getPosition();
	void ApplyOffset(double offset);//位置を変更(offset分足す)
	noteType getType(void) const;
};

//譜面
class Chart {
private:
	MPMChangeList mpmCngList;//MPM変化点のリスト
public:
	Chart(String file, bool headerOnly);//コンストラクタ，fileで与えられたパスのファイルをパースする
	HashTable<String, String> data;//ヘッダ 曲情報
	Array<Note> notes;//ノートのリスト
	Note* getJudgeNote(double left, double right, double nowSec, double judgeSec, noteType type);//nowSecから一番近いleft~rightのノート(typeであるもの)を取得
	Note* getJudgeNote(				  double pos, double nowSec, double judgeSec, noteType type);//nowSecから一番近い		posのノート(typeであるもの)を取得
};
