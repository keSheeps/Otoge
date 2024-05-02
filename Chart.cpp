# include "Common.hpp"//Chart.hppをCommon.hppの中で読む

void MPMChangeList::addMPMChange(double pos, double MPM) {
	MPMChange mpmChange{};
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
		if (static_cast<size_t>(i) + 1 < MPMList.size()) {
			if (MPMList[static_cast<size_t>(i) + 1].pos < pos) {
				lastPos = MPMList[static_cast<size_t>(i) + 1].pos;
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

Note::Note(double pos, noteType type, double left, double right, MPMChangeList mpmCngList) {
	this->timing = mpmCngList.getSec(pos);
	this->type = type;
	this->left = left;
	this->right = right;
}

std::tuple<double, double, double> Note::getPosition() {
	return std::tuple<double,double,double>(timing,left,right);
}

void Note::ApplyOffset(double offset) {
	timing += offset;
}

noteType Note::getType(void) const {
	return type;
}

Chart::Chart(String file,bool headerOnly) {
	
	TextReader reader{ file };
	if (not reader) {
		throw Error{ U"Failed to open chart file." };
	}
	path = file;
	//以下の通り#MPM,#NOTEの#以下の部分は特に意味はないが見た目のため記述を推奨
	// .chtファイルは"絶対に"UTF-8
	String line;
	//ヘッダ読込
	while (reader.readLine(line)) {
		if (line[0] == U'#')break;
		if (line[0] == U'/')continue;
		data[line.split(U':')[0]] = line.split(U':')[1];
	}
	if (headerOnly) {
		if (data.contains(U"jacket")) {
			jacket = Texture(FileSystem::PathAppend(FileSystem::ParentPath(file), data[U"jacket"]));
		}
		else {
			jacket = Texture(U"assets/texture-none.png");
		}
		return;
	}
	//MPM変更地点読込
	while (reader.readLine(line)) {
		if (line[0] == U'#')break;
		if (line[0] == U'/')continue;
		mpmCngList.addMPMChange(Parse<double>(line.split(U',')[0]), Parse<double>(line.split(U',')[1]));
	}
	//ノート読込み
	while (reader.readLine(line)) {
		if (line[0] == U'/')continue;
		if (Parse<double>(line.split(U',')[2]) < 1 and 1 < Parse<double>(line.split(U',')[3]))continue;//上下にまたがるノートは無理！(0-1は下1-2は上)
		noteType type = noteType::Unset;
		if (line.split(U',')[1] == U"Tap") { type = noteType::Tap; }
		if (line.split(U',')[1] == U"Trace_s") { type = noteType::Trace_s; }
		if (line.split(U',')[1] == U"Trace_B") { type = noteType::Trace_B; }
		if (line.split(U',')[1] == U"Swing") { type = noteType::Swing; }
		if (type == noteType::Unset) { continue; }
		Note note(Parse<double>(line.split(U',')[0]), type, Parse<double>(line.split(U',')[2]), Parse<double>(line.split(U',')[3]), mpmCngList);//ノート種別も入れる
		if (data.contains(U"offset")) {
			note.ApplyOffset(Parse<double>(data[U"offset"]));
		}
		notes << note;
	}
}

//一番近い位置のノートを得る，引数:判定位置[判定左位置,判定右位置],現在時刻,判定幅,判定するノートのタイプ
Note* Chart::getJudgeNote(double left, double right, double nowSec, double judgeSec, noteType type) {
	Note* judgeNote = nullptr;
	for (Note& note : notes) {
		double noteSec = std::get<0>(note.getPosition());
		double notePos1 = std::get<1>(note.getPosition());
		double notePos2 = std::get<2>(note.getPosition());
		double noteLeft = Min(notePos1,notePos2);
		double noteRight = Max(notePos1, notePos2);
		if (note.getType() != type) continue;//判定する種類のノートでなかったら×
		if (note.beaten) continue;//ノートがすでに叩かれていたら×
		if (!((left <= noteLeft and noteLeft <= right) or (left <= noteRight and noteRight <= right)
			or (noteLeft <= left and left <= noteRight) or (noteLeft <= right and right <= noteRight))) continue;//キー判定範囲とノートが重なっていなかったら×
		if (Abs(noteSec - nowSec) > judgeSec) continue;//判定時間とノート位置が重なっていなかったら×
		if (!(judgeNote == nullptr or noteSec < std::get<0>(judgeNote->getPosition()))) continue;//一番早いノートでなかったら×
		judgeNote = &note;//判定するノートを更新
	}
	return judgeNote;
}

Note* Chart::getJudgeNote(double pos, double nowSec, double judgeSec, noteType type) {
	Note* judgeNote = nullptr;
	for (Note& note : notes) {
		double noteSec = std::get<0>(note.getPosition());
		double notePos1 = std::get<1>(note.getPosition());
		double notePos2 = std::get<2>(note.getPosition());
		double noteLeft = Min(notePos1, notePos2);
		double noteRight = Max(notePos1, notePos2);
		if (note.getType() != type) continue;//判定する種類のノートでなかったら×
		if (note.beaten) continue;//ノートがすでに叩かれていたら×
		if (!(noteLeft <= pos and pos <= noteRight)) continue;//叩いた位置とノートが重なっていなかったら×
		if (Abs(noteSec - nowSec) > judgeSec) continue;//判定時間とノート位置が重なっていなかったら×
		if (!(judgeNote == nullptr or noteSec < std::get<0>(judgeNote->getPosition()))) continue;//一番早いノートでなかったら×
		judgeNote = &note;//判定するノートを更新
	}
	return judgeNote;
}
