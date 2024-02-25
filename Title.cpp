# include "Title.hpp"

Title::Title(const InitData& init) : IScene{ init } {
	
}

void Title::update() {
	if (Keyboard::GetAllInputs().size() > 0) {
		changeScene(State::Selector);
	}
}

void Title::draw() const {
	ClearPrint();
	Print << U"Press Any Key!";
}
