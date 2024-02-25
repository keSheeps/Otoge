# pragma once

# include "Common.hpp"
# include "Chart.hpp"

class Player : public App::Scene {
private:
	Chart chart;
	Audio song;

public:
	Player(const InitData& init);

	void update() override;

	void draw() const override;
};
