# pragma once

# include <Siv3D.hpp> // OpenSiv3D v0.6.14
# include "Chart.hpp"

namespace Settings {
	//システム設定
	constexpr int ResW = 1366;//横解像度(Resolution-Width)
	constexpr int ResH = 768;//縦解像度(Resolution-Height)
	constexpr bool FSC = false;//フルスクリーンにするか(FullSCreen)
	const String songsDir = U"songs";
	//プレイ画面のレイアウト
	constexpr int VPY = 75;//消失点(Vanishing Point)のY座標
	constexpr int LaneW = 1360;//レーンの横幅(一番手前)
	constexpr int UpperZ = 493;//上レーンの高さ
	constexpr double viewHeadSec = -3.5;//レーンの消失点の秒数(現在秒数から)
	constexpr double viewTailSec = 0.3;//レーンの一番手前の秒数(現在秒数から)
	//判定位置 goodSec>greatSec>perfectSecを要請する
	constexpr double goodSec = 0.11667;//±116.67ms
	constexpr double greatSec = 0.03333;//±33.33ms
	constexpr double perfectSec = 0.02;//±20.00ms
	//動作モード どれか一つを選ぶ
//#define AC
#define CS
//計算用
	constexpr int CenterX = ResW / 2;//画面中央のX座標
	constexpr int LaneHeight = ResH - VPY;
}

enum class State {
	Title,
	Selector,
	Player,
	Result,
};

struct GameData {
	Array<Chart> songList;
	uint64 index = 0;
	String chartPath;
	Array<int> detectedHands;
	double HS = 1;
	int volume_per = 100;
	double startPos = 0;
	bool isAuto = false;
	uint64 MaxCombo = 0;
	uint64 Perfect = 0;
	uint64 Great = 0;
	uint64 Good = 0;
	//ミスの数は上の4つから計算する
};

using App = SceneManager<State, GameData>;
