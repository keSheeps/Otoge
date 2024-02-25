# include <Siv3D.hpp> // OpenSiv3D v0.6.14

# include "Settings.hpp"
using namespace Settings;

# include "Note.hpp"
# include "MPM.hpp"

class Chart {
public:
	HashTable<String, String> data;
	MPMChangeList mpmCngList;
	Array<Note> notes;
};

class Assets {
public:
	HashTable<String, Texture> textures;
	HashTable<String, Audio> audios;
	Chart chart;
};

Assets Init();
Chart Parse(String file);
void Process(Chart& chart, double nowSec, HashTable<String, Audio> audios);
void Draw(Chart chart, double nowSec, HashTable<String, Texture> textures);

void Main()
{
	Assets assets = Init();

	assets.audios[U"music"].play();
	while (System::Update())
	{
		Process(assets.chart, assets.audios[U"music"].posSec(), assets.audios);
		Draw(assets.chart, assets.audios[U"music"].posSec(), assets.textures);
	}
}

Assets Init() {
	// シーンのサイズを固定
	// ウィンドウサイズに合わせて表示を拡大縮小
	Window::SetFullscreen(FSC);
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(ResW, ResH);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	Chart chart = Parse(U"test.cht");
	Assets assets;
	assets.chart = chart;
	assets.textures.emplace(U"bg", Texture( U"assets/otoge-play-stage.png" ));
	assets.textures.emplace(U"upper", Texture( U"assets/otoge-play-stage-upper.png"));
	assets.textures.emplace(U"texture_tap", Texture(  U"assets/otoge-play-note-tap.png", TextureDesc::Mipped));
	assets.textures.emplace(U"texture_tap_shadow", Texture(  U"assets/otoge-play-note-tap-shadow.png", TextureDesc::Mipped));
	assets.textures.emplace(U"texture_trace", Texture( U"assets/otoge-play-note-trace.png", TextureDesc::Mipped));
	assets.textures.emplace(U"texture_trace_shadow", Texture( U"assets/otoge-play-note-trace-shadow.png", TextureDesc::Mipped));
	assets.audios.emplace(U"music", Audio( chart.data[U"wav"]));
	assets.audios.emplace(U"shot", Audio( U"assets/決定ボタンを押す52.wav"));
	return assets;
}

Chart Parse(String file) {

	TextReader reader{ file };
	if (not reader) {
		throw Error{ U"Failed to open chart file." };
	}

	String line;
	Chart chart;
	HashTable<String, String> data;
	MPMChangeList mpmCngList;
	Array<Note> notes;

	//以下の通り#MPM,#NOTEの#以下の部分は特に意味はないが見た目のため記述を推奨
	// .chtファイルは"絶対に"UTF-8
	//ヘッダ読込
	while (reader.readLine(line)) {
		if (line[0] == U'#')break;
		if (line[0] == U'/')continue;
		data[line.split(U':')[0]] = line.split(U':')[1];
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
		noteType type=noteType::Unset;
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
	//Print << data;
	chart.data = data;
	chart.mpmCngList = mpmCngList;
	chart.notes = notes;

	return chart;
}

//一番近い位置のノートを得る，引数:全てのノート,判定位置[判定左位置,判定右位置],現在時刻,判定幅,判定するノートのタイプ
Note* getJudgeNote(Array<Note>& notes, double left, double right, double nowSec, double judgeSec, noteType type) {
	Note* judgeNote = nullptr;
	for (Note& note : notes) {
		double noteSec = std::get<0>(note.getPosition());
		double noteLeft = std::get<1>(note.getPosition());
		double noteRight = std::get <2> (note.getPosition());
		if (note.getType() != type) continue;//判定する種類のノートでなかったら×
		if (note.beaten) continue;//ノートがすでに叩かれていたら×
		if (! ((left <= noteLeft and noteLeft <= right) or (left <= noteRight and noteRight <= right)
			or (noteLeft <= left and left <= noteRight) or (noteLeft <= right and right <= noteRight))) continue;//キー判定範囲とノートが重なっていなかったら×
		if (Abs(noteSec - nowSec) > judgeSec) continue;//判定時間とノート位置が重なっていなかったら×
		if (! (judgeNote == nullptr or noteSec < std::get<0>(judgeNote->getPosition())) ) continue;//一番早いノートでなかったら×
		judgeNote = &note;//判定するノートを更新
	}
	return judgeNote;
}

Note* getJudgeNote(Array<Note>& notes, double pos, double nowSec, double judgeSec, noteType type) {
	Note* judgeNote = nullptr;
	for (Note& note : notes) {
		double noteSec = std::get<0>(note.getPosition());
		double noteLeft = std::get<1>(note.getPosition());
		double noteRight = std::get <2>(note.getPosition());
		if (note.getType() != type) continue;//判定する種類のノートでなかったら×
		if (note.beaten) continue;//ノートがすでに叩かれていたら×
		if (!(noteLeft <= pos and pos <= noteRight)) continue;//叩いた位置とノートが重なっていなかったら×
		if (Abs(noteSec - nowSec) > judgeSec) continue;//判定時間とノート位置が重なっていなかったら×
		if (!(judgeNote == nullptr or noteSec < std::get<0>(judgeNote->getPosition()))) continue;//一番早いノートでなかったら×
		judgeNote = &note;//判定するノートを更新
	}
	return judgeNote;
}

void Process(Chart& chart, double nowSec, HashTable<String, Audio> audios) {
	//ノートを処理
	Array<Note>& notes = chart.notes;
	Array<Note*> judgeNotes;
	#ifdef KEYS
	Array<Input> keysA = { KeyC,KeyV,KeyM,KeyComma,Key3,Key4,Key7,Key8 };
	for (int i = 0; i < 8; i++){
		if (keysA[i].down()) {
			judgeNotes << getJudgeNote(notes, 0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, nowSec, goodSec, noteType::Tap);
		}
		if (keysA[i].pressed()) {
			judgeNotes << getJudgeNote(notes, 0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, nowSec, perfectSec, noteType::Trace_s);
			judgeNotes << getJudgeNote(notes, 0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, nowSec, perfectSec, noteType::Trace_B);
		}
	}
	Array<Input> keysB = { KeyD,KeyF,KeyJ,KeyK,KeyE,KeyR,KeyU,KeyI };
	for (int i = 0; i < 8; i++) {
		if (keysB[i].down()) {
			judgeNotes << getJudgeNote(notes, 0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, nowSec, goodSec, noteType::Swing);
		}
	}
	for (Note* judgeNote : judgeNotes) {
		if (judgeNote != nullptr) {
			audios[U"shot"].playOneShot();
			double noteSec = std::get<0>(judgeNote->getPosition());
			if (Abs(noteSec - nowSec) <= perfectSec) {
				Print << U"Perfect";
			} else if (Abs(noteSec - nowSec) <= greatSec) {
				Print << U"Great";
			} else if (Abs(noteSec - nowSec) <= goodSec) {
				Print << U"Good";
			}
			judgeNote->beaten = true;
		}
	}
	#endif
	#ifdef SLIDER
	Array<double> poses;
	//ここでposesを取得(タッチされている座標)
	for (double pos : poses) {
		judgeNote = getJudgeNote(notes, pos, nowSec,goodSec, noteType::Tap);
		if (judgeNote != nullptr) {
			audios[U"shot"].playOneShot();
			judgeNote->beaten = true;
		}
	}
	#endif
	#ifdef AUTO
	for (Note& note : notes) {
		double noteSec = std::get<0>(note.getPosition());
		if (nowSec > noteSec) {
			if (!note.beaten) audios[U"shot"].playOneShot();
			note.beaten = true;
		}
	}
	#endif
	return;
}

void Draw(Chart chart, double nowSec, HashTable<String, Texture> textures) {
	Array<Note> notes = chart.notes;
	//ラムダ式の定義
	auto timeToRatio = [=](double time) {//時刻を0~1(表示部分の一番上~一番下)の範囲に変換する,また3(2)次関数にする これは判定位置が下から動いている分も考慮している
		double ratio = (nowSec - time - viewHeadSec) / (viewTailSec - viewHeadSec);
		//return ratio * ratio * (ratio < 0 ? -1 : 1)//２次関数
		return ratio*ratio*ratio;
	};
	auto timeToYPos = [=](double time,bool isUpper) {//時刻をY座標に変換する,上ならtrue 下ならfalse
		if(isUpper) return VPY + (LaneHeight - UpperZ) * timeToRatio(time);
		return VPY + LaneHeight * timeToRatio(time);
	};
	auto timeToXPos = [=](std::tuple<double, double, double> position,double timeDelta) {//時刻をX座標に変換する
		double time = std::get<0>(position)+timeDelta;
		double left = std::get<1>(position);
		double right = std::get<2>(position);
		if (left >= 1) {
			left -= 1;
			right -= 1;
		}
		double leftX = CenterX - (double)LaneW / 2 * timeToRatio(time) * 2*(0.5-left);
		double rightX = CenterX + (double)LaneW / 2 * timeToRatio(time) * 2*(right-0.5);
		return std::tuple<double, double>(leftX, rightX);
	};

	//レーンの線/判定ラインを描画
	//Line{ 0, timeToYPos(nowSec,false), ResW, timeToYPos(nowSec,false) }.draw(2, Palette::White);//判定ライン
	//for (int16 i = -LaneW / 2; i <= LaneW / 2; i += LaneW / 4) {
	//	Line{ CenterX, VPY, CenterX + i, ResH }.draw(2, Palette::White);
	//	//Line{ CenterX, VPY, CenterX + i, ResH - UpperZ }.draw(1, Palette::Lightpink);
	//}
	//Line{ 0, timeToYPos(nowSec,true), ResW, timeToYPos(nowSec,true) }.draw(2, Palette::Lightpink);//判定ライン
	//Line{ 0, ResH - (ResH - VPY) * viewTailSec / (viewTailSec - viewHeadSec) - UpperZ, ResW, ResH - (ResH - VPY) * viewTailSec / (viewTailSec - viewHeadSec) - UpperZ }.draw(2, Palette::Lightpink);//判定ライン


	textures[U"bg"].draw(0, 0);
	//ノートを描画
	for (Note note : notes) {
		double noteYPosFront = timeToYPos(std::get<0>(note.getPosition()) - 0.02, std::get<1>(note.getPosition()) >= 1);//1以上ならisUpperがtrue
		double noteYPos = timeToYPos(std::get<0>(note.getPosition()), std::get<1>(note.getPosition()) >= 1);//同様
		double noteYPosBack = timeToYPos(std::get<0>(note.getPosition()) + 0.02, std::get<1>(note.getPosition()) >= 1);//同様
		double noteYPosShadow = timeToYPos(std::get<0>(note.getPosition()) + 0.1, std::get<1>(note.getPosition()) >= 1);//同様
		if ((VPY < noteYPosBack) and (noteYPosFront < ResH) and !note.beaten) {
			auto noteXPosBack = timeToXPos(note.getPosition(),-0.02);
			auto noteXPos = timeToXPos(note.getPosition(), 0);
			auto noteXPosFront = timeToXPos(note.getPosition(),0.02);
			auto noteXPosShadow = timeToXPos(note.getPosition(), 0.1);

			const Quad quad_shadow{ Vec2{ std::get<0>(noteXPosShadow), noteYPosShadow }, Vec2{ std::get<1>(noteXPosShadow), noteYPosShadow },
				Vec2{ std::get<1>(noteXPos), noteYPos }, Vec2{ std::get<0>(noteXPos), noteYPos } };
			const Quad quad_note{ Vec2{ std::get<0>(noteXPosBack), noteYPosBack }, Vec2{ std::get<1>(noteXPosBack), noteYPosBack },
				Vec2{ std::get<1>(noteXPosFront), noteYPosFront}, Vec2{ std::get<0>(noteXPosFront), noteYPosFront } };
			if (note.getType() == noteType::Tap) {
				quad_shadow(textures[U"texture_tap_shadow"]).draw();
				quad_note(textures[U"texture_tap"]).draw();
			}
			if (note.getType() == noteType::Trace_s) {
				quad_shadow(textures[U"texture_trace_shadow"]).draw();
				quad_note(textures[U"texture_trace"]).draw();
			}
			if (note.getType() == noteType::Trace_B) {
				quad_shadow(textures[U"texture_trace_shadow"]).draw();
				quad_note(textures[U"texture_trace"]).draw();
			}
		}
	}
	textures[U"upper"].draw(0, 0);
}
