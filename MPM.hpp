# ifndef MPM_H_
# define MPM_H_

# include <Siv3D.hpp>

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

# endif // MPM_H_
