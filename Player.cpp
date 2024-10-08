﻿# include "Player.hpp"

Player::Player(const InitData& init) : IScene{ init },
		chart(Chart(getData().chartPath,false,getData().startPos)),
		song(Audio(FileSystem::PathAppend(FileSystem::ParentPath(getData().chartPath), chart.data[U"wav"]))) {//初期化子を使って初期化しよう
	song.play();
	song.setVolume(((double)getData().volume_per) / 100);
	song.seekTime(chart.startSec);
	trace_s_vol = Parse<double>(chart.data[U"tracesvol"]);
	getData().MaxCombo = chart.notes.size();
	getData().Perfect = 0;
	getData().Great = 0;
	getData().Good = 0;
}

bool isPressed_key[8];
bool isPressed_slider[16];
bool isPressed_above[16];

void Player::update() {
	if (!song.isPlaying())changeScene(State::Result);
	//ノートを処理
	Array<Note*> judgeNotes;
	//キーボード
	Array<Input> keysA = { KeyD,KeyF,KeyJ,KeyK,KeyE,KeyR,KeyU,KeyI };
	for (int i = 0; i < 8; i++) {
		if (keysA[i].down()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.01, 0.25 * (i + 1) - 0.01, song.posSec(), Settings::goodSec, noteType::Tap);
		}
		if (keysA[i].pressed()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.01, 0.25 * (i + 1) - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_s);
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.01, 0.25 * (i + 1) - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_B);
		}
		if (isPressed_key[i] && !keysA[i].pressed()) {
			judgeNotes << chart.getJudgeNote(0.25 * i + 0.01, 0.25 * (i + 1) - 0.01, song.posSec(), Settings::goodSec, noteType::Swing);
		}
		isPressed_key[i] = keysA[i].pressed();
	}
	//スライダー
	if (const auto slider = Gamepad(0)) {
		for (auto [i, button] : Indexed(slider.buttons)) {
			if (button.down()) {
				judgeNotes << chart.getJudgeNote(0.0625 * i + 0.01, 0.0625 * (i + 1) - 0.01, song.posSec(), Settings::goodSec, noteType::Tap);
			}
			if (button.pressed()) {
				judgeNotes << chart.getJudgeNote(0.0625 * i + 0.01, 0.0625 * (i + 1) - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_s);
				judgeNotes << chart.getJudgeNote(0.0625 * i + 0.01, 0.0625 * (i + 1) - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_B);
			}
			if (isPressed_slider[i] && !button.pressed()) {
				judgeNotes << chart.getJudgeNote(0.0625 * i + 0.01, 0.0625 * (i + 1) - 0.01, song.posSec(), Settings::goodSec, noteType::Swing);
			}
			isPressed_slider[i] = button.pressed();
		}
	}
	//上レーン detectedHandsの構造のせいで分かりにくい
	bool isPressed_above_new[16];//後で最新に更新する
	for (int32 i = 0; i < 16; i++) {
		isPressed_above_new[i] = false;
	}
	for (int32 x : getData().detectedHands) {
		if (x == -1)continue;
		int i = x;//(15-x) + 16;
		isPressed_above_new[i] = true;
	}
	for (int32 i = 0; i < 16; i++) {
		if (isPressed_above_new[i]) {
			judgeNotes << chart.getJudgeNote(0.0625 * i + 1.0 + 0.01, 0.0625 * (i + 1) + 1.0 - 0.01, song.posSec(), Settings::perfectSec, noteType::Tap);
			judgeNotes << chart.getJudgeNote(0.0625 * i + 1.0 + 0.01, 0.0625 * (i + 1) + 1.0 - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_s);
			judgeNotes << chart.getJudgeNote(0.0625 * i + 1.0 + 0.01, 0.0625 * (i + 1) + 1.0 - 0.01, song.posSec(), Settings::perfectSec, noteType::Trace_B);
			if (!getData().enableSwing) {
				judgeNotes << chart.getJudgeNote(0.0625 * i + 1.0 + 0.01, 0.0625 * (i + 1) + 1.0 - 0.01, song.posSec(), Settings::goodSec, noteType::Swing);
			}
		}
		else {
			if (getData().enableSwing && isPressed_above[i]) {
				judgeNotes << chart.getJudgeNote(0.0625 * i + 1.0 + 0.01, 0.0625 * (i + 1) + 1.0 - 0.01, song.posSec(), Settings::goodSec, noteType::Swing);
			}
		}
	}
	for (int32 i = 0; i < 16; i++) {
		isPressed_above[i] = isPressed_above_new[i];
	}
	//押下されたノートを処理
	for (Note* judgeNote : judgeNotes) {
		if (judgeNote != nullptr) {
			if (judgeNote->beaten)continue;
			double noteSec = std::get<0>(judgeNote->getPosition());
			if (Abs(noteSec - song.posSec()) <= Settings::perfectSec) {
				getData().Perfect += 1;
			}
			else if (Abs(noteSec - song.posSec()) <= Settings::greatSec) {
				getData().Great += 1;
			}
			else if (Abs(noteSec - song.posSec()) <= Settings::goodSec) {
				getData().Good += 1;
			}
			if (judgeNote->getType() == noteType::Trace_s) {
				AudioAsset(U"shot").playOneShot(trace_s_vol);//音量設定あり
			} else {
				AudioAsset(U"shot").playOneShot();
			}
			judgeNote->beaten = true;
		}
	}
	//auto
	bool beat = false;
	bool isTrace_sOnly = true;
	if (!getData().isAuto) { return; }
	for (Note& note : chart.notes) {
		double noteSec = std::get<0>(note.getPosition());
		if (song.posSec() > noteSec && note.getType()!=noteType::Guide && note.beaten==false) {
			note.beaten = true;
			getData().Perfect += 1;
			beat = true;
			if (note.getType() != noteType::Trace_s) {
				isTrace_sOnly = false;
			}
		}
	}
	if (beat) AudioAsset(U"shot").playOneShot(isTrace_sOnly ? trace_s_vol : 1);
}

void Player::draw() const{
	Array<Note> notes = chart.notes;
	//ラムダ式の定義
	auto timeToRatio = [=](double time) {//時刻を0~1(表示部分の一番上~一番下)の範囲に変換する,また3(2)次関数にする これは判定位置が下から動いている分も考慮している
		double ratio = (song.posSec() - time - Settings::viewHeadSec / getData().HS) / (Settings::viewTailSec/getData().HS - Settings::viewHeadSec / getData().HS);
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
		auto isUpper = std::get<1>(note.getPosition()) >= 1;//1以上ならisUpperがtrue
		double noteYPosFront = timeToYPos(std::get<0>(note.getPosition()) - (0.02 + 0.04 * isUpper) / getData().HS, isUpper);
		double noteYPos = timeToYPos(std::get<0>(note.getPosition()), isUpper);
		double noteYPosBack = timeToYPos(std::get<0>(note.getPosition()) + 0.02/getData().HS, isUpper);
		double noteYPosBack2 = timeToYPos(std::get<0>(note.getPosition()) + 0.04 / getData().HS, isUpper);
		double noteYPosShadow = timeToYPos(std::get<0>(note.getPosition()) + 0.1 / getData().HS, isUpper);
		if ((Settings::VPY < noteYPosBack) and (noteYPosFront + Settings::UpperZ*isUpper < Settings::ResH) and !note.beaten) {
			auto noteXPosBack = timeToXPos(note.getPosition(), -0.02);
			auto noteXPosBack2 = timeToXPos(note.getPosition(), -0.04);
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
				const Quad quad_note_2{ Vec2{ std::get<0>(noteXPosBack2), noteYPosBack2 }, Vec2{ std::get<1>(noteXPosBack2), noteYPosBack2 },
					Vec2{ std::get<1>(noteXPosFront), noteYPosFront}, Vec2{ std::get<0>(noteXPosFront), noteYPosFront } };
				quad_shadow(TextureAsset(U"texture_swing_shadow")).draw();
				quad_note_2(TextureAsset(U"texture_swing")).draw();
			}
			if (note.getType() == noteType::Guide) {
				quad_shadow(TextureAsset(U"texture_trace_shadow")).draw();
			}
		}
	}
	TextureAsset(U"upper").draw(0, 0);
}
