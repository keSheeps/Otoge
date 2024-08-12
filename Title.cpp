# include "Title.hpp"

Title::Title(const InitData& init) : IScene{ init } {
	
}

void Title::update() {

	if (const auto slider = Gamepad(0)) {
		for (auto [i, button] : Indexed(slider.buttons)) {
			if (button.down()) {
				changeScene(State::Selector);
			}
		}
	}
	if (Keyboard::GetAllInputs().size() > 0) {
		changeScene(State::Selector);
	}
}

void Title::draw() const {
	ClearPrint();
	Print << U"Press Any Key!";
}
