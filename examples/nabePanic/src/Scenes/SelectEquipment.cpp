#include "SelectEquipment.hpp"
#include "Controller.hpp"

SelectEquipment::SelectEquipment(const InitData& init) : IScene{ init }
{
	for (auto i : step(3)) {
		seme_soubi << Texture{ Resource(U"nabePanic/images/{}/seme_{}.png"_fmt(!(getData().stage_id) ? U"nabe" : U"pafe", i + 1)) };
		nige_soubi << Texture{ Resource(U"nabePanic/images/{}/nige_{}.png"_fmt(!(getData().stage_id) ? U"nabe" : U"pafe", i + 1)) };
	}
	bgm.play(1s);
}

SelectEquipment::~SelectEquipment() {}

void SelectEquipment::update()
{
	if (flag == 2.0)
	{
		Array<Image> images;
		animation.read(images, delays);
		for (const auto& image : images)
		{
			textures << Texture{ image };
		}
		ClearPrint();
		flag = 1.0;
	}

	if (flag > 0)
	{
		flag -= Scene::DeltaTime();
	}

	int8 p1 = getData().p1_data.role;
	int8 p2 = getData().p2_data.role;

	//Player1のカーソル処理
	if (getData().p1_input.Left.down() and (p1_cursor.x == 0 or p1_cursor.x == -1))
	{
		p1_cursor.x = -1;
		cursor.playOneShot();
	}
	else if (p1_cursor.x == -1 and getData().p1_input.Confirm.down())
	{
		confirm.playOneShot();
		bgm.stop(1s);
		changeScene(State::Title);
	}
	else if (getData().p1_input.Right.down() and (p1_cursor.x == 2 or p1_cursor.x == 3) and p1 != -1 and p2 != -1 and p1 == !p2)
	{
		p1_cursor.x = 3;
		cursor.playOneShot();
	}
	else if (p1_cursor.x == 3 and getData().p1_input.Confirm.down())
	{
		if (p1 != -1 and p2 != -1 and p1 == !p2)
		{
			start.playOneShot();
			bgm.stop(1s);
			changeScene(State::Game);
		}
	}
	else
	{
		if (getData().p1_input.Up.down()) {
			p1_cursor.y = Clamp(--(p1_cursor.y), 0, 1);
			cursor.playOneShot();
		}

		if (getData().p1_input.Down.down()) {
			p1_cursor.y = Clamp(++(p1_cursor.y), 0, 1);
			cursor.playOneShot();
		}

		if (getData().p1_input.Left.down()) {
			p1_cursor.x = Clamp(--(p1_cursor.x), 0, 2);
			cursor.playOneShot();
		}

		if (getData().p1_input.Right.down()) {
			p1_cursor.x = Clamp(++(p1_cursor.x), 0, 2);
			cursor.playOneShot();
		}

		if (getData().p1_input.Confirm.down() and !(getData().p1_data.role == p1_cursor.y and getData().p1_data.eqid == p1_cursor.x)) {
			getData().p1_data.role = p1_cursor.y;
			getData().p1_data.eqid = p1_cursor.x;
			select.playOneShot();
		}
	}

	//Player2のカーソル処理
	if (!getData().use_controller or getData().p2_data.conindex != -1)
	{
		if (getData().p2_input.Up.down()) {
			p2_cursor.y = Clamp(--(p2_cursor.y), 0, 1);
			cursor.playOneShot();
		}

		if (getData().p2_input.Down.down()) {
			p2_cursor.y = Clamp(++(p2_cursor.y), 0, 1);
			cursor.playOneShot();
		}

		if (getData().p2_input.Left.down()) {
			p2_cursor.x = Clamp(--(p2_cursor.x), 0, 2);
			cursor.playOneShot();
		}

		if (getData().p2_input.Right.down()) {
			p2_cursor.x = Clamp(++(p2_cursor.x), 0, 2);
			cursor.playOneShot();
		}

		if (getData().p2_input.Confirm.down() and !(getData().p2_data.role == p2_cursor.y and getData().p2_data.eqid == p2_cursor.x)) {
			getData().p2_data.role = p2_cursor.y;
			getData().p2_data.eqid = p2_cursor.x;
			select.playOneShot();
		}
	}
	else {
		// P2コントローラ設定
		Array<int8> inputdata = getWherePush();
		if (inputdata[0] != -1 and inputdata[1] != -1 and inputdata[0] != getData().p1_data.conindex)
		{
			getData().p2_data.conindex = inputdata[0];
			getData().p2_input.Up = Gamepad(inputdata[0]).povUp;
			getData().p2_input.Down = Gamepad(inputdata[0]).povDown;
			getData().p2_input.Left = Gamepad(inputdata[0]).povLeft;
			getData().p2_input.Right = Gamepad(inputdata[0]).povRight;
			getData().p2_input.Confirm = Gamepad(inputdata[0]).buttons[inputdata[1]];
		}
	}
}

void SelectEquipment::draw() const
{
	if (flag == 2.0) {
		loading.drawAt(Scene::Center());
		return;
	}

	// アニメーションの経過時間
	const double t = Scene::Time() / 2;

	// 経過時間と各フレームのディレイに基づいて、何番目のフレームを描けばよいかを計算
	const size_t frameIndex = AnimatedGIFReader::GetFrameIndex(t, delays);

	textures[frameIndex].drawAt(Scene::Center());
	bg.drawAt(Scene::Center());

	// 2Pのコントローラー設定の催促
	if (getData().use_controller and getData().p2_data.conindex == -1)
	{
		FontAsset(U"NormalFont")(U"2Pの人はコントローラーを接続して").drawAt(Scene::Center() + Point{ 0, 155 }, ColorF{ 1.0, 1.0, 1.0 });
		FontAsset(U"NormalFont")(U"任意のボタン(決定ボタン)を教えてください").drawAt(Scene::Center() + Point{ 0, 185 }, ColorF{ 1.0, 1.0, 1.0 });
	}

	int8 p1 = getData().p1_data.role;
	int8 p2 = getData().p2_data.role;

	// サムネ用の下地を出力
	for (auto i : step(3)) {
		Rect{ Arg::center(Scene::Center() + Point{(i - 1) * 220, -220}), 210 }.rounded(10).draw(ColorF{ 1.0, 1.0, 1.0 });
		Rect{ Arg::center(Scene::Center() + Point{(i - 1) * 220, 0}), 210 }.rounded(10).draw(ColorF{ 1.0, 1.0, 1.0 });
	}

	// タイトルに戻るボタン
	Rect{ Arg::center(Scene::Center() + Point{-500, -110}), 230, 70 }.rounded(10).draw(ColorF{ 0.8, 0.8, 0.8 });
	FontAsset(U"NormalFont")(U"タイトルに戻る").drawAt(Scene::Center() + Point{ -500, -110 });

	// ゲームを始めるボタン
	if (p1 != -1 and p2 != -1 and p1 == !p2)
	{
		Rect{ Arg::center(Scene::Center() + Point{500, -110}), 230, 70 }.rounded(10).draw(ColorF{ 0.8, 0.8, 0.8 });
		FontAsset(U"NormalFont")(U"ゲームを始める").drawAt(Scene::Center() + Point{ 500, -110 }, ColorF{ 1.0, 1.0, 1.0 });
		FontAsset(U"LargeFont")(U"ゲームを始めよう！").drawAt(Scene::Center() + Point{ 0, 170 }, ColorF{ 1.0, 1.0, 1.0 });
	}
	else
	{
		Rect{ Arg::center(Scene::Center() + Point{500, -110}), 230, 70 }.rounded(10).draw(ColorF{ 0.8, 0.8, 0.8, 0.5 });
		FontAsset(U"NormalFont")(U"ゲームを始める").drawAt(Scene::Center() + Point{ 500, -110 }, ColorF{ 1.0, 1.0, 1.0, 0.5 });
		FontAsset(U"LargeFont")(U"キャラクターを選択しよう！").drawAt(Scene::Center() + Point{ 0, 170 }, ColorF{ 1.0, 1.0, 1.0 });
	}

	// カーソル
	if (p1_cursor.x == -1)
	{
		Rect{ Arg::center(Scene::Center() + Point{-500, -110}), 230, 70 }.rounded(10).drawFrame(10, 0, ColorF{ Palette::Blue, 0.5 });
	}
	else if (p1_cursor.x == 3)
	{
		Rect{ Arg::center(Scene::Center() + Point{500, -110}), 230, 70 }.rounded(10).drawFrame(10, 0, ColorF{ Palette::Blue, 0.5 });
	}
	else
	{
		Rect{ Arg::center(Scene::Center() + Point{(p1_cursor.x - 1) * 220, (p1_cursor.y - 1) * 220}), 210 }.rounded(10).drawFrame(10, 0, ColorF{ Palette::Blue, 0.5 });
	}
	Rect{ Arg::center(Scene::Center() + Point{(p2_cursor.x - 1) * 220, (p2_cursor.y - 1) * 220}), 210 }.rounded(10).drawFrame(10, 0, ColorF{ Palette::Red, 0.5 });

	// サムネ一覧の表示
	for (auto i : step(3)) {
		seme_soubi[i].resized(192).drawAt(Scene::Center() + Point((i - 1) * 220, -220));
		nige_soubi[i].resized(192).drawAt(Scene::Center() + Point((i - 1) * 220, 0));
	}

	// 選択中の装備表示
	if (getData().p1_data.role == 0) {
		seme_soubi[getData().p1_data.eqid].resized(256).drawAt(Scene::Center() + Point(-500, 220));
	}
	else if (getData().p1_data.role == 1) {
		nige_soubi[getData().p1_data.eqid].resized(256).drawAt(Scene::Center() + Point(-500, 220));
	}

	if (getData().p2_data.role == 0) {
		seme_soubi[getData().p2_data.eqid].resized(256).drawAt(Scene::Center() + Point(500, 220));
	}
	else if (getData().p2_data.role == 1) {
		nige_soubi[getData().p2_data.eqid].resized(256).drawAt(Scene::Center() + Point(500, 220));
	}

	if (flag > 0) {
		loading.drawAt(Scene::Center(), ColorF{ 1.0, 1.0, 1.0, flag });
	}
}

