# pragma once

# include "Common.hpp"

class Title : public App::Scene {
private:
public:
	Title(const InitData& init);

	void update() override;

	void draw() const override;
};
