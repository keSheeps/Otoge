# pragma once

# include "Common.hpp"

class Title : public App::Scene {
private:
	Font font{ FontMethod::MSDF, 48 };
public:
	Title(const InitData& init);

	void update() override;

	void draw() const override;
};
