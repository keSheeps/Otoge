# ifndef NOTES_H_
# define NOTES_H_

# include <Siv3D.hpp>
# include "MPM.hpp"

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
	noteType getType(void);
};

# endif
