# ifndef NOTES_H_
# define NOTES_H_

# include <Siv3D.hpp>
# include "MPM.hpp"

//ノートの種類
enum class noteType {
	Tap,
	SlideA,
	SlideB,
	Damage,
	SwingUp,
	SwingDown,
};

//ノート
class Note {
private://(TODO)timingのゲッタを用意してこっちに移す----------------------------------------------------------------------------------------------
	noteType type;//ノート種別
	double timing;//判定ラインに重なるまでの秒数
	double left;//左位置(0~1)
	double right;//右位置(0~1)
public:
	bool beaten=false;
	std::tuple<double, double, double> getPosition();
	//コンストラクタ
	//小節番号(1~),小節内での位置(0~1),ノート種別,左位置(0~1),右位置(0~1)
	Note(double pos, noteType type, double left, double right, MPMChangeList mpmCngList);
	void ApplyOffset(double offset);//位置を変更(offset分足す)
};

# endif
