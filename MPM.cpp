# include "MPM.hpp"

void MPMChangeList::addMPMChange(double pos, double MPM) {
	MPMChange mpmChange;
	mpmChange.pos = pos;
	mpmChange.MPM = MPM;
	MPMList << mpmChange;
}

//ノートの位置を渡して秒数を確認する
double MPMChangeList::getSec(double pos) {
	double noteSec = 0;//合計秒数
	double lastPos = 0;//終点位置
	bool brk = false;
	if (MPMList.size() == 0) { Print << U"BPM設定がありません"; return -1; }
	for (int i = 0; i < MPMList.size() and !brk; i++) {
		//measure+posで位置を表せる
		if (i + 1 < MPMList.size()) {
			if (MPMList[i + 1].pos < pos) {
				lastPos = MPMList[i + 1].pos;
			}
			else {
				lastPos = pos;
				brk = true;
			}
		}
		else {
			lastPos = pos;
			brk = true;
		}
		
		//M[meas/min]とすると60/M[sec/meas]
		noteSec += (lastPos - MPMList[i].pos) * 60 / MPMList[i].MPM;
	}
	return noteSec;
}
