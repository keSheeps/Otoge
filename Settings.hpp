# ifndef APP_SETTINGS_HPP_
# define APP_SETTINGS_HPP_

namespace Settings {
	//システム設定
	constexpr int ResW = 1366;//横解像度(Resolution-Width)
	constexpr int ResH = 768;//縦解像度(Resolution-Height)
	constexpr bool FSC = false;//フルスクリーンにするか(FullSCreen)
	//プレイ画面のレイアウト
	constexpr int VPY = 75;//消失点(Vanishing Point)のY座標
	constexpr int LaneW = 1360;//レーンの横幅(一番手前)
	constexpr int UpperZ = 493;//上レーンの高さ
	constexpr double viewHeadSec = -3.5;//レーンの消失点の秒数(現在秒数から)
	constexpr double viewTailSec = 0.3;//レーンの一番手前の秒数(現在秒数から)
	//判定位置 goodSec>greatSec>perfectSecを要請する
	constexpr double goodSec = 0.11667;//±116.67ms
	constexpr double greatSec = 0.03333;//±33.33ms
	constexpr double perfectSec = 0.01667;//±16.67ms
	//操作タイプ どれか一つを選ぶ
	//#define KEYS
	//#define SLIDER
	#define AUTO
	//計算用
	constexpr int CenterX = ResW / 2;//画面中央のX座標
	constexpr int LaneHeight = ResH - VPY;
}

# endif //APP_SETTINGS_HPP_
