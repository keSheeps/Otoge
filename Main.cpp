# include "Common.hpp"
# include "Title.hpp"
# include "Selector.hpp"
# include "Player.hpp"
# include "Result.hpp"

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
	TextureAsset::Register(U"bg", U"assets/otoge-play-stage.png");
	TextureAsset::Register(U"upper", U"assets/otoge-play-stage-upper.png");
	TextureAsset::Register(U"texture_tap", U"assets/otoge-play-note-tap.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_tap_shadow", U"assets/otoge-play-note-tap-shadow.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_trace", U"assets/otoge-play-note-trace.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_trace_shadow", U"assets/otoge-play-note-trace-shadow.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_swing", U"assets/otoge-play-note-swing.png", TextureDesc::Mipped);
	TextureAsset::Register(U"texture_swing_shadow", U"assets/otoge-play-note-swing-shadow.png", TextureDesc::Mipped);
	AudioAsset::Register(U"shot", U"assets/決定ボタンを押す52.wav");

	//シーンの登録
	App manager;
	manager.add<Title>(State::Title);
	manager.add<Selector>(State::Selector);
	manager.add<Player>(State::Player);
	manager.add<Result>(State::Result);

	//手の検出プログラムを開始
	ChildProcess DetectHand{ U"DetectHands.exe", Pipe::StdInOut };

	manager.get()->chartPath = U"songs/test.cht";
	//manager.init(State::Player);
	while (System::Update())
	{
		String input;
		DetectHand.istream() >> input;
		String temp = input.split('\n').back();
		if (temp != U"") {
			Print << temp;
			Array<int32> inputs = temp.split(',').map([](String str) {
				return Parse<int32>(str);
			});
			manager.get()->detectedHands = inputs;
		}
		if (not manager.update())//現在のシーンのupdateとdrawの実行
		{
			break;
		}
	}
	DetectHand.terminate();
}
