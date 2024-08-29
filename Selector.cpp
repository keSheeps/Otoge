# include "Selector.hpp"

Selector::Selector(const InitData& init) : IScene{ init } {
	AudioAsset(U"selectbgm").play();
}
int cursor = 0;
void Selector::update() {
	ClearPrint();
	bool pressedD=false, pressedF = false, pressedJ = false, pressedK = false;
	if (getData().useSliderOnSelector) {
		if (const auto slider = Gamepad(0)) {
			for (auto [i, button] : Indexed(slider.buttons)) {
				if (button.down()) {
					if (0 <= i && i <= 3) pressedD = true;
					if (4 <= i && i <= 7) pressedF = true;
					if (8 <= i && i <= 11) pressedJ = true;
					if (12 <= i && i <= 15) pressedK = true;
				}
			}
		}
	}
	if (state == 0) {
		if (KeyD.down() || pressedD) state = 1;
		if (KeyF.down() || pressedF) {
			if ((int64)getData().index - 1 < 0)getData().index = getData().songList.size();
			getData().index = getData().index - 1;
		}
		if (KeyJ.down() || pressedJ) {
			getData().index = getData().index + 1;
			if (getData().index >= getData().songList.size())getData().index = 0;
		}
	}else if (state == 1) {
		if (KeyF.down() || pressedF) {
			state = 0;
		}
		if (KeyJ.down() || pressedJ) {
			getData().chartPath = getData().songList[getData().index].path;
			AudioAsset(U"selectbgm").stop();
			changeScene(State::Player);
		}
		if (KeyK.down() || pressedK) state = 2;
	}
	else if (state == 2) {
		if (KeyD.down() || pressedD) {
			if (cursor == 0) {
				getData().volume_per += 10;
				if (getData().volume_per > 100) getData().volume_per = 10;
			}
			if (cursor == 1) {
				getData().HS += 0.5;
				if (getData().HS > 4) getData().HS = 0.5;
			}
		}
		if (KeyF.down() || pressedF) {
			cursor = 0;
		}
		if (KeyJ.down() || pressedJ) {
			cursor = 1;
		}
		if (KeyK.down() || pressedK) state = 1;
	}
}

void Selector::draw() const {
	TextureAsset(U"background").draw(0, 0);
	//パネル
	if (state == 0) {
		TextureAsset(U"panel").drawAt(-Settings::ResW / 3, Settings::ResH / 2);
		TextureAsset(U"panel").drawAt(Settings::ResW + Settings::ResW / 3, Settings::ResH / 2);
	}
	if (state == 0 || state == 1) {
		TextureAsset(U"panel").drawAt(Settings::ResW / 2, Settings::ResH / 2);
		uint64 i = 0;
		for (Chart song : getData().songList) {
			if (i == getData().index) {
				font(song.data[U"title"]).drawAt(80, Scene::Size().x / 2, Scene::Size().y / 8 * 3, ColorF(0, 0, 0));//drawAt(80, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 3, ColorF(0, 0, 0));
				font(song.data[U"composer"]).drawAt(40, Scene::Size().x / 2, Scene::Size().y / 8 * 4, ColorF(0, 0, 0));//drawAt(40, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 4, ColorF(0, 0, 0));
				font(U"譜面:" + song.data[U"designer"]).drawAt(30, Scene::Size().x / 2, Scene::Size().y / 8 * 6, ColorF(0, 0, 0));//drawAt(30, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 6, ColorF(0, 0, 0));

				uint64 level = Parse<uint64>(song.data[U"level"]);
				Point difficultyPos(Settings::ResW / 2 + TextureAsset(U"panel").size().x / 2 - 80, Settings::ResH / 2 - TextureAsset(U"panel").size().y / 2 + 80);//80に深い意味はない いい感じの位置だったので(あとでSettingsに移動)
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
				double x = Scene::Size().x / 2 - TextureAsset(U"panel").size().x / 4 + 60;
				double y = Scene::Size().y / 2;
				const Quad quad{ Vec2{ x - 250,y - 250 }, Vec2{ x + 250,y - 250 },  Vec2{ x + 250,y + 250 },  Vec2{ x - 250,y + 250 } };
				//quad(song.jacket).drawAt(Scene::Size().x / 2 - TextureAsset(U"panel").size().x / 4 + 60, Scene::Size().y / 2);
			}
			i++;
		}
	}
	if (state == 2) {
		TextureAsset(U"panel-half").drawAt(Settings::ResW / 4, Settings::ResH / 2);
		TextureAsset(U"panel-half").drawAt(Settings::ResW / 4 * 3, Settings::ResH / 2);
		font(U"Volume").drawAt(Settings::ResW / 4, 300, ColorF(0, 0, 0));
		font(U"Hi-speed").drawAt(Settings::ResW / 4 * 3, 300, ColorF(0, 0, 0));
		font(Format((double)getData().volume_per) + U"%").drawAt(Settings::ResW / 4, 400, ColorF(0.7 * (cursor == 0), 0.7 * (cursor == 0), 0));
		font(U"x" + Format(getData().HS)).drawAt(Settings::ResW / 4 * 3, 400, ColorF(0.7 * (cursor == 1), 0.7 * (cursor == 1), 0));
	}
	//操作表示
	if (state == 0) {
		font(U"Decide").drawAt(Scene::Size().x / 8 * 1, Scene::Size().y - 50);
		font(U"←").drawAt(Scene::Size().x / 8 * 3, Scene::Size().y - 50);
		font(U"→").drawAt(Scene::Size().x / 8 * 5, Scene::Size().y - 50);
		//font(U"Change Lv.").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);//譜面に複数レベルを入れるのをやめた
	}else if (state == 1) {
		font(U"Cancel").drawAt(Scene::Size().x / 8 * 3, Scene::Size().y - 50);
		font(U"Play").drawAt(Scene::Size().x / 8 * 5, Scene::Size().y - 50);
		font(U"Settings").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
	}else if (state == 2) {
		font(U"Change").drawAt(Scene::Size().x / 8 * 1, Scene::Size().y - 50);
		font(U"←").drawAt(Scene::Size().x / 8 * 3, Scene::Size().y - 50);
		font(U"→").drawAt(Scene::Size().x / 8 * 5, Scene::Size().y - 50);
		font(U"Back").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
	}
}
