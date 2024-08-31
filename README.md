# 音ゲーを作る #
OpenSiv3Dのテンプレートのプロジェクトに上書きすると正常にコンパイルできるプロジェクトになります

# 構造 #
Main.cppが実行直後に呼ばれます ここでカメラ関係の入出力も行っています(OpenSiv3DのPipe https://zenn.dev/reputeless/books/siv3d-documentation/viewer/api-c を参照)<br>
ここからTitle.cpp内のクラスが呼ばれます ここはキーを押してSelector.cppに飛ぶだけ<br>
Selector.cppで選曲(曲のリストはMain.cppで作っています)
Player.cppで再生
Result.cppで結果を表示してSelector.cppに戻ります

# 譜面読み込みについて #
Player.cppの最初の部分でchart(Chart(getData().chartPath,false,getData().startPos)を実行しています ここが譜面をchart(変数)に読み込む部分 ChartクラスはChart.cppにまとまっています<br>
処理を簡単にするために曲の情報としてBPMではなく一分当たりの小節数を持つことにしています(これがMPM) またノートの位置としてレーン上の点をを0～2の実数で2点持つことにしています(0～1:下 1～2:上)(Note.left,Note.right) 時間軸では整数部分が小節，小数部分が小節内での位置となるように実数を持っています(Note.timing)<br>
timingは譜面データ頭のMPM変化点のリストから計算しています

# 絵などについて #
タイトル絵:友人の友人
その他:Collaguru( https://twitter.com/collaguru_ohs )
効果音:https://soundeffect-lab.info/sound/button/