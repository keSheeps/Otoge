# pragma once

# include "Common.hpp"

class Result : public App::Scene {
private:
	Font font{ FontMethod::MSDF, 48 };
	uint64 Miss;
	uint64 level;
public:
	Result(const InitData& init);

	void update() override;

	void draw() const override;
};
