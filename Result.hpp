# pragma once

# include "Common.hpp"

class Result : public App::Scene {
private:
public:
	Result(const InitData& init);

	void update() override;

	void draw() const override;
};
