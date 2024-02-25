# pragma once

# include "Common.hpp"
# include "Chart.hpp"

class Selector : public App::Scene {
private:
	Array<Chart> songList;
	Array<FilePath> chtFiles;//songListとchtFilesの添字は一致
	uint64 index;
public:
	Selector(const InitData& init);

	void update() override;

	void draw() const override;
};
