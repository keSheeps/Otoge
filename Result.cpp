# include "Result.hpp"

Result::Result(const InitData& init) : IScene{ init } {

	/*getData().MaxCombo = 100;
	getData().Perfect = 80;
	getData().Great = 10;
	getData().Good = 2;*/

	Miss = getData().MaxCombo - (getData().Perfect + getData().Great + getData().Good);
	level = Parse<uint64>(getData().songList[getData().index].data[U"level"]);
}

void Result::update() {
	if (KeyK.down()) {
		#ifdef AC
			changeScene(State::Title);
		#endif
		#ifdef CS
			changeScene(State::Selector);
		#endif
	}
}

void Result::draw() const {
	ClearPrint();
	TextureAsset(U"background").draw(0, 0);
	TextureAsset(U"panel-half").drawAt(Settings::ResW / 4, Settings::ResH / 2);
	font(getData().songList[getData().index].data[U"title"]).drawAt(80, Scene::Size().x / 4, Scene::Size().y / 8 * 3, ColorF(0, 0, 0));
	font(getData().songList[getData().index].data[U"composer"]).drawAt(40, Scene::Size().x / 4, Scene::Size().y / 8 * 4, ColorF(0, 0, 0));
	font(U"譜面:" + getData().songList[getData().index].data[U"designer"]).drawAt(30, Scene::Size().x / 4, Scene::Size().y / 8 * 6, ColorF(0, 0, 0));
	Point difficultyPos(Settings::ResW / 2  - 160, Settings::ResH / 2 - TextureAsset(U"panel").size().y / 2 + 80);//80に深い意味はない いい感じの位置だったので(あとでSettingsに移動)
	if (1 <= level && level < 4) {
		TextureAsset(U"difficulty-1").scaled(0.5).drawAt(difficultyPos);
	}
	else if (4 <= level && level < 7) {
		TextureAsset(U"difficulty-2").scaled(0.5).drawAt(difficultyPos);
	}
	else if (7 <= level && level < 10) {
		TextureAsset(U"difficulty-3").scaled(0.5).drawAt(difficultyPos);
	}
	else {
		TextureAsset(U"difficulty-4").scaled(0.5).drawAt(difficultyPos);
	}
	font(level).drawAt(80, difficultyPos);


	TextureAsset(U"panel-half").drawAt(Settings::ResW / 4*3, Settings::ResH / 2);
	font(getData().MaxCombo).drawAt(80,Settings::ResW / 2 + 230, Settings::ResH / 2 - TextureAsset(U"panel").size().y / 2 + 80, ColorF(0, 0, 0));
	font(U"Combo").drawAt(Settings::ResW / 4*3+100, Settings::ResH / 2 - TextureAsset(U"panel").size().y / 2 + 90, ColorF(0, 0, 0));
	if (getData().MaxCombo == getData().Perfect) {
		TextureAsset(U"result-AP").scaled(0.4).drawAt(Settings::ResW / 4 * 3, Settings::ResH / 17 * 6 + 20);
	}
	else if (Miss == 0) {
		TextureAsset(U"result-FC").scaled(0.4).drawAt(Settings::ResW / 4 * 3, Settings::ResH / 17 * 6 + 20);
	}
	else {
		TextureAsset(U"result-SC").scaled(0.4).drawAt(Settings::ResW / 4 * 3, Settings::ResH / 17 * 6+20);
	}
	font(U"Perfect:" + Format(getData().Perfect)).drawAt(60, Settings::ResW / 4 * 3, Settings::ResH / 17 * 7+60, ColorF(0, 0, 0));
	font(U"Great:" + Format(getData().Great)).drawAt(60, Settings::ResW / 4 * 3, Settings::ResH / 17 * 8+80, ColorF(0, 0, 0));
	font(U"Good:" + Format(getData().Good)).drawAt(60, Settings::ResW / 4 * 3, Settings::ResH / 17 * 9+100, ColorF(0, 0, 0));
	font(U"Miss:" + Format(Miss)).drawAt(60, Settings::ResW / 4 * 3, Settings::ResH / 17 * 10+120, ColorF(0, 0, 0));
	font(U"Next").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
}
