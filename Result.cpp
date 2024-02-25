# include "Result.hpp"

Result::Result(const InitData& init) : IScene{ init } {

}

void Result::update() {
	if (Keyboard::GetAllInputs().size() > 0) {
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
	Print << U"Result:Press Any Key!";
}
