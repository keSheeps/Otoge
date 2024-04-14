# include "Selector.hpp"

Selector::Selector(const InitData& init) : IScene{ init } {
}

void Selector::update() {
	ClearPrint();
	if (state == 0) {
		if (KeyF.down()) {
			if ((int64)getData().index - 1 < 0)getData().index = getData().songList.size();
			getData().index = getData().index - 1;
		}
		if (KeyJ.down()) {
			getData().index = getData().index + 1;
			if (getData().index >= getData().songList.size())getData().index = 0;
		}
		if (KeyD.down())state = 1;
	}else if (state == 1) {
		if (KeyF.down()) {
			state = 0;
		}
		if (KeyJ.down()) {
			getData().chartPath = getData().songList[getData().index].path;
			changeScene(State::Player);
		}
		if (KeyK.down()) state = 2;
	}
	else if (state == 2) {
		if (KeyK.down()) state = 1;
	}
}

void Selector::draw() const {
	TextureAsset(U"background").draw(0, 0);
	TextureAsset(U"panel").drawAt(Settings::ResW / 2, Settings::ResH / 2);
	if (state == 0) {
		TextureAsset(U"panel").drawAt(-Settings::ResW / 3, Settings::ResH / 2);
		TextureAsset(U"panel").drawAt(Settings::ResW + Settings::ResW / 3, Settings::ResH / 2);
	}
	if (state == 0 || state == 1) {
		uint64 i = 0;
		for (Chart song : getData().songList) {
			if (i == getData().index) {
				font(song.data[U"title"]).drawAt(80, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 3, ColorF(0, 0, 0));
				font(song.data[U"composer"]).drawAt(40, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 4, ColorF(0, 0, 0));
				font(U"譜面:" + song.data[U"designer"]).drawAt(30, Scene::Size().x / 2 + TextureAsset(U"panel").size().x / 4, Scene::Size().y / 8 * 6, ColorF(0, 0, 0));

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
				quad(song.jacket).drawAt(Scene::Size().x / 2 - TextureAsset(U"panel").size().x / 4 + 60, Scene::Size().y / 2);
			}
			i++;
		}
	}
	if (state == 0) {
		font(U"Decide").drawAt(Scene::Size().x / 8 * 1, Scene::Size().y - 50);
		font(U"←").drawAt(Scene::Size().x / 8 * 3, Scene::Size().y - 50);
		font(U"→").drawAt(Scene::Size().x / 8 * 5, Scene::Size().y - 50);
		//font(U"Change Lv.").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
	}else if (state == 1) {
		font(U"Cancel").drawAt(Scene::Size().x / 8 * 3, Scene::Size().y - 50);
		font(U"Play").drawAt(Scene::Size().x / 8 * 5, Scene::Size().y - 50);
		font(U"Settings").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
	}else if (state == 2) {
		font(U"Back").drawAt(Scene::Size().x / 8 * 7, Scene::Size().y - 50);
	}
}
