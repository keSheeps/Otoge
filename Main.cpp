# include "Common.hpp"
# include "Title.hpp"
# include "Selector.hpp"
# include "Player.hpp"
# include "Result.hpp"

bool IsChtFile(const FilePath& path)
{
	return (FileSystem::Extension(path) == U"cht");
}

String getDetectedHands(ChildProcess DetectHand) {
	String input;
	DetectHand.istream() >> input;
	return input.split('\n').back();
}

void Main()
{
	// シーンのサイズを固定
	// ウィンドウサイズに合わせて表示を拡大縮小
	Window::SetFullscreen(Settings::FSC);
	Window::SetStyle(WindowStyle::Sizable);
	Window::Resize(Settings::ResW, Settings::ResH);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::SetBackground(ColorF{ 0.5, 0.5, 0.5 });

	//Assetsの登録
	TextureAsset::Register(U"title", U"assets/title.png");
	AudioAsset::Register(U"titlebgm", U"assets/jupiter.wav");
	AudioAsset::Register(U"selectbgm", U"assets/microcosmos.wav");
	TextureAsset::Register(U"background", U"assets/bg.png");
	TextureAsset::Register(U"panel", U"assets/panel.png");
	TextureAsset::Register(U"difficulty-1", U"assets/difficulty-1.png");
	TextureAsset::Register(U"difficulty-2", U"assets/difficulty-2.png");
	TextureAsset::Register(U"difficulty-3", U"assets/difficulty-3.png");
	TextureAsset::Register(U"difficulty-4", U"assets/difficulty-4.png");

	TextureAsset::Register(U"bg", U"assets/otoge-play-stage.png");
	TextureAsset::Register(U"upper", U"assets/otoge-play-stage-upper.png");
	TextureAsset::Register(U"texture_tap", U"assets/otoge-play-note-tap.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_tap_shadow", U"assets/otoge-play-note-tap-shadow.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_trace", U"assets/otoge-play-note-trace.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_trace_shadow", U"assets/otoge-play-note-trace-shadow.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_swing", U"assets/otoge-play-note-swing.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_swing_shadow", U"assets/otoge-play-note-swing-shadow.png", TextureDesc::Mipped);
	AudioAsset::Register(U"shot", U"assets/決定ボタンを押す52.wav");

	TextureAsset::Register(U"panel-half", U"assets/panel-half.png");
	TextureAsset::Register(U"result-SC", U"assets/result-SC.png");
	TextureAsset::Register(U"result-FC", U"assets/result-FC.png");
	TextureAsset::Register(U"result-AP", U"assets/result-AP.png");

	//設定ファイルを読む
	const INI ini{ U"config.ini" };
	bool useDetectHands = Parse<bool>(ini[U"System.useDetectHands"]);
	bool enableSwing = Parse<bool>(ini[U"System.enableSwing"]);
	bool useSliderOnSelector = Parse<bool>(ini[U"System.useSliderOnSelector"]);
	//シーンの登録
	App manager;
	manager.add<Title>(State::Title);
	manager.add<Selector>(State::Selector);
	manager.add<Player>(State::Player);
	manager.add<Result>(State::Result);
	manager.get()->enableSwing = enableSwing;
	manager.get()->useSliderOnSelector = useSliderOnSelector;
	//第1引数は.exeファイルの位置 第2引数:譜面パス(既定:選曲から開始) 第3引数:HS(既定:1) 第4引数:開始小節(既定:0)
	const Array<String> args = System::GetCommandLineArgs();
	if (size(args) >= 2) {
		manager.get()->chartPath = args[1];
		if (size(args) >= 3) {
			manager.get()->HS = Parse<double>(args[2]);
			if (size(args) >= 4) {
				manager.get()->startPos = Parse<double>(args[3]);
			}
		}
		manager.init(State::Player);//ここでコンストラクタが呼ばれるので前においてはいけない
		manager.get()->isAuto = true;
		useDetectHands = false;
	}

	//曲をすべて取得
	const Array<FilePath> paths = FileSystem::DirectoryContents(Settings::songsDir);
	const Array<FilePath> chtFiles = paths.filter(IsChtFile);
	for (const auto& path : chtFiles)
	{
		Chart chart = Chart(path, true, 0);
		manager.get()->songList << chart;
	}

	//手の検出プログラムを開始
	ChildProcess DetectHand;
	AsyncTask<String> task;
	if (useDetectHands) {
		DetectHand = ChildProcess(U"DetectHands.exe", Pipe::StdInOut);
		task = Async(getDetectedHands, DetectHand);
	}
	while (System::Update())
	{
		if (useDetectHands) {
			if (task.isReady()) {
				String temp = task.get();
				if (temp != U"") {
					Array<int32> inputs = temp.split(',').map([](String str) {
						return Parse<int32>(str);
					});
					manager.get()->detectedHands = inputs;
				}
				task = Async(getDetectedHands, DetectHand);
			}
		}
		if (not manager.update())//現在のシーンのupdateとdrawの実行
		{
			break;
		}
	}
	if (useDetectHands) {
		if (task.isValid())
		{
			task.wait();
		}
		DetectHand.terminate();
	}
}
