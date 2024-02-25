# include "Selector.hpp"

bool IsChtFile(const FilePath& path)
{
	return (FileSystem::Extension(path) == U"cht");
}

Selector::Selector(const InitData& init) : IScene{ init },index(0) {
	const Array<FilePath> paths = FileSystem::DirectoryContents(Settings::songsDir);
	chtFiles = paths.filter(IsChtFile);
	for (const auto& path : chtFiles)
	{
		Chart chart = Chart(path, true);
		songList << chart;
	}
}

void Selector::update() {
	if (KeyUp.down()) {
		if (index-1 < 0)index = songList.size();
		index = index - 1;
	}
	if (KeyDown.down()) {
		index = index + 1;
		if (index >= songList.size())index = 0;
	}
	if (KeyEnter.down()) {
		getData().chartPath = chtFiles[index];
		changeScene(State::Player);
	}
}

void Selector::draw() const {
	ClearPrint();
	uint64 i = 0;
	for (Chart song : songList) {
		Print << song.data[U"title"] + (i==index ? U"←" : U"");
		i++;
	}
}
