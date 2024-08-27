# include "Title.hpp"

Title::Title(const InitData& init) : IScene{ init } {
	
}

void Title::update() {
	if (getData().useSliderOnSelector) {
		if (const auto slider = Gamepad(0)) {
			for (auto [i, button] : Indexed(slider.buttons)) {
				if (button.down()) {
					changeScene(State::Selector);
				}
			}
		}
	}
	if (Keyboard::GetAllInputs().size() > 0) {
		changeScene(State::Selector);
	}
}

void Title::draw() const {
	TextureAsset(U"title").draw(0, 0);
	font(U"Press any key!").drawAt(30, Scene::Size().x / 2, Scene::Size().y / 8 * 6, ColorF(1, 1, 1));
}
