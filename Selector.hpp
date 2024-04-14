# pragma once

# include "Common.hpp"

class Selector : public App::Scene {
private:
	Font font{ FontMethod::MSDF, 48 };
	uint64 state=0;
public:
	Selector(const InitData& init);

	void update() override;

	void draw() const override;
};
