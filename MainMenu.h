#pragma once
#include "Funcs.h"

using namespace sf;

class MainMenu
{
public:
	int name = 0;
	bool bdiff[3];

	int width = 40;
	int height = 40;
	bool hint;
	bool activeleft;
	bool activeright;
	RectangleShape cap;

	Sprite play;
	Sprite solve;
	Sprite exit;
	Texture titlemenutex;

	Sprite play_bg;
	Sprite diff[3];
	Sprite lr_arrow[2];
	Sprite start;
	Sprite names;
	Texture play_bgtex;
	Texture difftex;
	Texture lr_arrowtex;
	Texture starttex;
	Texture namestex;

	Sprite solve_bg;
	Sprite back;
	Sprite OK;
	Sprite ud_arrow[4];
	Sprite ctrl;
	Texture solve_bgtex;
	Texture backtex;
	Texture OKtex;
	Texture ud_arrowtex;
	Texture ctrltex;
	Font proxima;
	Text size[2];

	MainMenu(int width, int height);

	void DrawTitle(RenderWindow &window);

	void DrawPlay(RenderWindow &window);

	void DrawSolve(RenderWindow &window);

	char TitleEventReaction(Event event, int width, int height);

	int PlayEventReaction(Event event);

	int SolveEventReaction(Event event);

	char CreateMenuWindow();
};