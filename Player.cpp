# include "Player.hpp"

Player::Player(const InitData& init) : IScene{ init },
		chart(Chart(getData().chartPath,false)),
		song(Audio(FileSystem::PathAppend(FileSystem::ParentPath(getData().chartPath), chart.data[U"wav"]))) {//初期化子を使って初期化しよう
	song.play();
}

void Player::update() {
	//ノートを処理
	Array<Note*> judgeNotes;
#ifdef KEYS
	Array<Input> keysA = { KeyC,KeyV,KeyM,KeyComma,Key3,Key4,Key7,Key8 };
	for (int i = 0; i < 8; i++) {
		if (keysA[i].down()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, song.posSec(), Settings::goodSec, noteType::Tap);
		}
		if (keysA[i].pressed()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, song.posSec(), Settings::perfectSec, noteType::Trace_s);
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, song.posSec(), Settings::perfectSec, noteType::Trace_B);
		}
	}
	Array<Input> keysB = { KeyD,KeyF,KeyJ,KeyK,KeyE,KeyR,KeyU,KeyI };
	for (int i = 0; i < 8; i++) {
		if (keysB[i].down()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.1, 0.25 * (i + 1) - 0.1, song.posSec(), Settings::goodSec, noteType::Swing);
		}
	}
	for (Note* judgeNote : judgeNotes) {
		if (judgeNote != nullptr) {
			double noteSec = std::get<0>(judgeNote->getPosition());
			if (Abs(noteSec - song.posSec()) <= Settings::perfectSec) {
				Print << U"Perfect";
			}
			else if (Abs(noteSec - song.posSec()) <= Settings::greatSec) {
				Print << U"Great";
			}
			else if (Abs(noteSec - song.posSec()) <= Settings::goodSec) {
				Print << U"Good";
			}
			AudioAsset(U"shot").playOneShot();
			judgeNote->beaten = true;
		}
	}
#endif
#ifdef SLIDER
	Array<double> poses;
	//ここでposesを取得(タッチされている座標)
	Note* judgeNote;
	for (double pos : poses) {
		judgeNote = chart.getJudgeNote(pos, song.posSec(), Settings::goodSec, noteType::Tap);
		if (judgeNote != nullptr) {
			AudioAsset(U"shot").playOneShot();
			judgeNote->beaten = true;
		}
	}
#endif
#ifdef AUTO
	for (Note& note : chart.notes) {
		double noteSec = std::get<0>(note.getPosition());
		if (song.posSec() > noteSec) {
			if (!note.beaten) AudioAsset(U"shot").playOneShot();
			note.beaten = true;
		}
	}
#endif
	if (!song.isPlaying())changeScene(State::Result);
}

void Player::draw() const{
	Array<Note> notes = chart.notes;
	//ラムダ式の定義
	auto timeToRatio = [=](double time) {//時刻を0~1(表示部分の一番上~一番下)の範囲に変換する,また3(2)次関数にする これは判定位置が下から動いている分も考慮している
		double ratio = (song.posSec() - time - Settings::viewHeadSec) / (Settings::viewTailSec - Settings::viewHeadSec);
		//return ratio * ratio * (ratio < 0 ? -1 : 1)//２次関数
		return ratio * ratio * ratio;
		};
	auto timeToYPos = [=](double time, bool isUpper) {//時刻をY座標に変換する,上ならtrue 下ならfalse
		if (isUpper) return Settings::VPY + (Settings::LaneHeight - Settings::UpperZ) * timeToRatio(time);
		return Settings::VPY + Settings::LaneHeight * timeToRatio(time);
		};
	auto timeToXPos = [=](std::tuple<double, double, double> position, double timeDelta) {//時刻をX座標に変換する
		double time = std::get<0>(position) + timeDelta;
		double left = std::get<1>(position);
		double right = std::get<2>(position);
		if (left >= 1) {
			left -= 1;
			right -= 1;
		}
		double leftX = Settings::CenterX - (double)Settings::LaneW / 2 * timeToRatio(time) * 2 * (0.5 - left);
		double rightX = Settings::CenterX + (double)Settings::LaneW / 2 * timeToRatio(time) * 2 * (right - 0.5);
		return std::tuple<double, double>(leftX, rightX);
		};

	TextureAsset(U"bg").draw(0, 0);
	//ノートを描画
	for (Note note : notes) {
		double noteYPosFront = timeToYPos(std::get<0>(note.getPosition()) - 0.02, std::get<1>(note.getPosition()) >= 1);//1以上ならisUpperがtrue
		double noteYPos = timeToYPos(std::get<0>(note.getPosition()), std::get<1>(note.getPosition()) >= 1);//同様
		double noteYPosBack = timeToYPos(std::get<0>(note.getPosition()) + 0.02, std::get<1>(note.getPosition()) >= 1);//同様
		double noteYPosShadow = timeToYPos(std::get<0>(note.getPosition()) + 0.1, std::get<1>(note.getPosition()) >= 1);//同様
		if ((Settings::VPY < noteYPosBack) and (noteYPosFront < Settings::ResH) and !note.beaten) {
			auto noteXPosBack = timeToXPos(note.getPosition(), -0.02);
			auto noteXPos = timeToXPos(note.getPosition(), 0);
			auto noteXPosFront = timeToXPos(note.getPosition(), 0.02);
			auto noteXPosShadow = timeToXPos(note.getPosition(), 0.1);

			const Quad quad_shadow{ Vec2{ std::get<0>(noteXPosShadow), noteYPosShadow }, Vec2{ std::get<1>(noteXPosShadow), noteYPosShadow },
				Vec2{ std::get<1>(noteXPos), noteYPos }, Vec2{ std::get<0>(noteXPos), noteYPos } };
			const Quad quad_note{ Vec2{ std::get<0>(noteXPosBack), noteYPosBack }, Vec2{ std::get<1>(noteXPosBack), noteYPosBack },
				Vec2{ std::get<1>(noteXPosFront), noteYPosFront}, Vec2{ std::get<0>(noteXPosFront), noteYPosFront } };
			if (note.getType() == noteType::Tap) {
				quad_shadow(TextureAsset(U"texture_tap_shadow")).draw();
				quad_note(TextureAsset(U"texture_tap")).draw();
			}
			if (note.getType() == noteType::Trace_s) {
				quad_shadow(TextureAsset(U"texture_trace_shadow")).draw();
				quad_note(TextureAsset(U"texture_trace")).draw();
			}
			if (note.getType() == noteType::Trace_B) {
				quad_shadow(TextureAsset(U"texture_trace_shadow")).draw();
				quad_note(TextureAsset(U"texture_trace")).draw();
			}
			if (note.getType() == noteType::Swing) {
				quad_shadow(TextureAsset(U"texture_swing_shadow")).draw();
				quad_note(TextureAsset(U"texture_swing")).draw();
			}
		}
	}
	TextureAsset(U"upper").draw(0, 0);
}
