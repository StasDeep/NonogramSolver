#include "stdafx.h"
#include "MainMenu.h"

MainMenu::MainMenu(int width, int height)
{
	/* Load textures. */
	{
		titlemenutex.loadFromFile("Images/menu.png");

		play_bgtex.loadFromFile("Images/pmenutop.png");
		difftex.loadFromFile("Images/settings2.png");
		lr_arrowtex.loadFromFile("Images/arrowbuttons.png");
		starttex.loadFromFile("Images/startbutton.png");
		namestex.loadFromFile("Images/names.png");

		solve_bgtex.loadFromFile("Images/smenutop.png");
		backtex.loadFromFile("Images/backtomenu.png");
		OKtex.loadFromFile("Images/okbutton.png");
		ud_arrowtex.loadFromFile("Images/updownarrowbuttons.png");
		ctrltex.loadFromFile("Images/Ctrl.png");
		proxima.loadFromFile("Proxima Nova Bold.otf");
	}

	/* Set title menu sprites.*/
	{

		play.setTexture(titlemenutex);
		play.setTextureRect(IntRect(0, 0, 270, 100));
		play.setOrigin(135, 50);
		play.setPosition(width / 2, height / 2 - 100);

		solve.setTexture(titlemenutex);
		solve.setTextureRect(IntRect(270, 0, 270, 100));
		solve.setOrigin(135, 50);
		solve.setPosition(width / 2, height / 2);

		exit.setTexture(titlemenutex);
		exit.setTextureRect(IntRect(540, 0, 270, 100));
		exit.setOrigin(135, 50);
		exit.setPosition(width / 2, height / 2 + 100);
	}

	/* Set play menu sprites. */
	{
		play_bg.setTexture(play_bgtex);
		play_bg.setTextureRect(IntRect(0, 0, 1280, 130));
		play_bg.setPosition(0, 0);

		diff[0].setTexture(difftex);
		diff[1].setTexture(difftex);
		diff[2].setTexture(difftex);

		lr_arrow[0].setTexture(lr_arrowtex);
		lr_arrow[1].setTexture(lr_arrowtex);

		start.setTexture(starttex);
		start.setTextureRect(IntRect(0, 0, 290, 44));
		start.setPosition(495, 470);

		names.setTexture(namestex);
	}

	/* Set solve menu sprites. */
	{
		solve_bg.setTexture(solve_bgtex);
		solve_bg.setPosition(0, 0);

		back.setTexture(backtex);
		back.setTextureRect(IntRect(0, 0, 200, 70));
		back.setPosition(60, 50);

		OK.setTexture(OKtex);
		OK.setTextureRect(IntRect(0, 0, 290, 44));
		OK.setPosition(512, 510);

		ud_arrow[0].setTexture(ud_arrowtex);
		ud_arrow[0].setTextureRect(IntRect(0, 0, 35, 30));
		ud_arrow[0].setPosition(432, 329);

		ud_arrow[1].setTexture(ud_arrowtex);
		ud_arrow[1].setTextureRect(IntRect(70, 0, 35, 30));
		ud_arrow[1].setPosition(432, 363);

		ud_arrow[2].setTexture(ud_arrowtex);
		ud_arrow[2].setTextureRect(IntRect(0, 0, 35, 30));
		ud_arrow[2].setPosition(432, 395);

		ud_arrow[3].setTexture(ud_arrowtex);
		ud_arrow[3].setTextureRect(IntRect(70, 0, 35, 30));
		ud_arrow[3].setPosition(432, 429);

		size[0].setFont(proxima);
		size[0].setCharacterSize(40);
		size[0].setColor(Color(189, 189, 189));
		size[0].setPosition(375, 337);
		size[0].setString(N2S(this->width));

		size[1].setFont(proxima);
		size[1].setCharacterSize(40);
		size[1].setColor(Color(189, 189, 189));
		size[1].setPosition(375, 402);
		size[1].setString(N2S(this->height));

		ctrl.setTexture(ctrltex);
		ctrl.setPosition(486, 368);
		hint = false;

		cap.setFillColor(Color(39, 39, 39, 200));
		cap.setSize(Vector2f(596, 306));
		cap.setPosition(684, 170);
		activeleft = true;
		activeright = false;
	}
}

void MainMenu::DrawTitle(RenderWindow &window)
{
	window.clear(Color(39, 39, 39));

	window.draw(play);
	window.draw(solve);
	window.draw(exit);
}

void MainMenu::DrawPlay(RenderWindow &window)
{
	window.clear(Color(39, 39, 39));

	window.draw(play_bg);
	window.draw(diff[0]);
	window.draw(diff[1]);
	window.draw(diff[2]);
	window.draw(lr_arrow[0]);
	window.draw(lr_arrow[1]);
	window.draw(start);
	window.draw(names);
}

void MainMenu::DrawSolve(RenderWindow &window)
{
	window.clear(Color(39, 39, 39));

	window.draw(solve_bg);
	window.draw(diff[0]);
	window.draw(diff[1]);
	window.draw(diff[2]);
	window.draw(back);
	window.draw(OK);
	window.draw(size[0]);
	window.draw(size[1]);
	window.draw(ud_arrow[0]);
	window.draw(ud_arrow[1]);
	window.draw(ud_arrow[2]);
	window.draw(ud_arrow[3]);
	window.draw(lr_arrow[0]);
	window.draw(lr_arrow[1]);
	window.draw(names);
	if (hint)
		window.draw(ctrl);
	window.draw(cap);
}

char MainMenu::TitleEventReaction(Event event, int width, int height)
{
	if (event.type == Event::MouseMoved)
	{
		int xpos = event.mouseMove.x;
		int ypos = event.mouseMove.y;

		if (xpos > width / 2 - 135 && xpos < width / 2 + 135)
		{
			if (ypos > height / 2 - 150 && ypos < height / 2 - 50)
				play.setTextureRect(IntRect(0 + 810, 0, 270, 100));
			else
				play.setTextureRect(IntRect(0, 0, 270, 100));
			if (ypos > height / 2 - 50 && ypos < height / 2 + 50)
				solve.setTextureRect(IntRect(270 + 810, 0, 270, 100));
			else
				solve.setTextureRect(IntRect(270, 0, 270, 100));

			if (ypos > height / 2 + 50 && ypos < height / 2 + 150)
				exit.setTextureRect(IntRect(540 + 810, 0, 270, 100));
			else
				exit.setTextureRect(IntRect(540, 0, 270, 100));
		}
		else
		{
			play.setTextureRect(IntRect(0, 0, 270, 100));
			solve.setTextureRect(IntRect(270, 0, 270, 100));
			exit.setTextureRect(IntRect(540, 0, 270, 100));
		}
	}

	if (event.type == Event::MouseButtonReleased)
	{
		int xpos = event.mouseButton.x;
		int ypos = event.mouseButton.y;

		if (xpos > width / 2 - 135 && xpos < width / 2 + 135)
		{
			/* Play. */
			if (ypos > height / 2 - 150 && ypos < height / 2 - 50)
			{
				lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
				lr_arrow[0].setPosition(440, 360 - 60);
				lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
				lr_arrow[1].setPosition(800, 360 - 60);

				diff[0].setTextureRect(IntRect(220, 0, 110, 50));
				diff[0].setPosition(420, 260 - 60);
				bdiff[0] = true;
				diff[1].setTextureRect(IntRect(330, 0, 150, 50));
				diff[1].setPosition(560, 260 - 60);
				bdiff[1] = false;
				diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				diff[2].setPosition(730, 260 - 60);
				bdiff[2] = false;

				name = 0;
				names.setTextureRect(IntRect(0, 0, 290, 44));
				names.setPosition(495, 364 - 60);

				return 'p';
			}

			/* Solve. */
			if (ypos > height / 2 - 50 && ypos < height / 2 + 50)
			{
				lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
				lr_arrow[0].setPosition(777, 403);
				lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
				lr_arrow[1].setPosition(1137, 403);

				diff[0].setTextureRect(IntRect(220, 0, 110, 50));
				diff[0].setPosition(420 + 350, 330);
				bdiff[0] = true;
				diff[1].setTextureRect(IntRect(330, 0, 150, 50));
				diff[1].setPosition(560 + 350, 330);
				bdiff[1] = false;
				diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				diff[2].setPosition(730 + 350, 330);
				bdiff[2] = false;

				name = 0;
				names.setTextureRect(IntRect(0, 0, 290, 44));
				names.setPosition(832, 407);

				cap.setFillColor(Color(39, 39, 39, 200));
				cap.setPosition(684, 170);
				activeleft = true;
				activeright = false;

				return 's';
			}

			/* Exit. */
			if (ypos > height / 2 + 50 && ypos < height / 2 + 150)
				return 'e';
		}
	}

	return '0';
}

int MainMenu::PlayEventReaction(Event event)
{
	if (event.type == Event::MouseMoved)
	{
		int xpos = event.mouseMove.x;
		int ypos = event.mouseMove.y;

		/* Check mouse movement. */
		{
			if (xpos < 265 && xpos > 55 && ypos > 50 && ypos < 120)
				play_bg.setTextureRect(IntRect(0, 130, 1280, 130));
			else
				play_bg.setTextureRect(IntRect(0, 0, 1280, 130));

			if (bdiff[0] == false)
				if (xpos > 420 && xpos < 530 && ypos > 260 - 60 && ypos < 310 - 60)
					diff[0].setTextureRect(IntRect(110, 0, 110, 50));
				else
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));

			if (bdiff[1] == false)
				if (xpos > 560 && xpos < 710 && ypos > 260 - 60 && ypos < 310 - 60)
					diff[1].setTextureRect(IntRect(480, 0, 150, 50));
				else
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));

			if (bdiff[2] == false)
				if (xpos > 730 && xpos < 840 && ypos > 260 - 60 && ypos < 310 - 60)
					diff[2].setTextureRect(IntRect(890, 0, 110, 50));
				else
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));

			if (xpos > 440 && xpos < 480 && ypos > 360 - 60 && ypos < 410 - 60 && name > 0)
				lr_arrow[0].setTextureRect(IntRect(40, 0, 40, 50));
			else
				lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));

			if (xpos > 800 && xpos < 840 && ypos > 360 - 60 && ypos < 410 - 60 && name < NUMOFZERO)
				lr_arrow[1].setTextureRect(IntRect(120, 0, 40, 50));
			else
				lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));

			if (xpos > 495 && xpos < 785 && ypos > 470 && ypos < 514)
				start.setTextureRect(IntRect(290, 0, 290, 44));
			else
				start.setTextureRect(IntRect(0, 0, 290, 44));
		}

	}

	if (event.type == Event::MouseButtonReleased)
	{
		int xpos = event.mouseButton.x;
		int ypos = event.mouseButton.y;

		/* Check mouse click. */
		{
			if (xpos < 265 && xpos > 55 && ypos > 50 && ypos < 120)
			{
				play_bg.setTextureRect(IntRect(0, 0, 1280, 130));
				return 0;
			}

			if (xpos > 420 && xpos < 530 && ypos > 260 - 60 && ypos < 310 - 60)
			{
				name = 0;
				names.setTextureRect(IntRect(0, 0, 290, 44));
				bdiff[0] = true;
				bdiff[1] = false;
				bdiff[2] = false;
				diff[0].setTextureRect(IntRect(220, 0, 110, 50));
				diff[1].setTextureRect(IntRect(330, 0, 150, 50));
				diff[2].setTextureRect(IntRect(780, 0, 110, 50));
			}

			if (xpos > 560 && xpos < 710 && ypos > 260 - 60 && ypos < 310 - 60)
			{
				name = MEDIUMSTART;
				names.setTextureRect(IntRect(MEDIUMSTART * 290, 0, 290, 44));
				bdiff[0] = false;
				bdiff[1] = true;
				bdiff[2] = false;
				diff[0].setTextureRect(IntRect(0, 0, 110, 50));
				diff[1].setTextureRect(IntRect(630, 0, 150, 50));
				diff[2].setTextureRect(IntRect(780, 0, 110, 50));
			}


			if (xpos > 730 && xpos < 840 && ypos > 260 - 60 && ypos < 310 - 60)
			{
				name = HARDSTART;
				names.setTextureRect(IntRect(HARDSTART * 290, 0, 290, 44));
				bdiff[0] = false;
				bdiff[1] = false;
				bdiff[2] = true;
				diff[0].setTextureRect(IntRect(0, 0, 110, 50));
				diff[1].setTextureRect(IntRect(330, 0, 150, 50));
				diff[2].setTextureRect(IntRect(1000, 0, 110, 50));
			}

			if (xpos > 440 && xpos < 480 && ypos > 360 - 60 && ypos < 410 - 60 && name > 0)
			{
				names.setTextureRect(IntRect(--name * 290, 0, 290, 44));
				if (name == 0)
					lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
				if (name < HARDSTART)
				{
					bdiff[0] = false;
					bdiff[1] = true;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(630, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}
				if (name < MEDIUMSTART)
				{
					bdiff[0] = true;
					bdiff[1] = false;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(220, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}

			}

			if (xpos > 800 && xpos < 840 && ypos > 360 - 60 && ypos < 410 - 60 && name < NUMOFZERO)
			{
				names.setTextureRect(IntRect(++name * 290, 0, 290, 44));
				if (name == NUMOFZERO)
					lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
				if (name >= MEDIUMSTART)
				{
					bdiff[0] = false;
					bdiff[1] = true;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(630, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}
				if (name >= HARDSTART)
				{
					bdiff[0] = false;
					bdiff[1] = false;
					bdiff[2] = true;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(1000, 0, 110, 50));
				}
			}

			if (xpos > 495 && xpos < 785 && ypos > 470 && ypos < 514)
				return 1;
		}
	}

	return 2;
}

int MainMenu::SolveEventReaction(Event event)
{
	if (event.type == Event::MouseMoved)
	{
		int xpos = event.mouseMove.x;
		int ypos = event.mouseMove.y;

		/* OK and Back buttons. */
		{
			if (xpos > 512 && xpos < 803 && ypos > 510 && ypos < 554)
				OK.setTextureRect(IntRect(290, 0, 290, 44));
			else
				OK.setTextureRect(IntRect(0, 0, 290, 44));

			if (xpos < 265 && xpos > 55 && ypos > 50 && ypos < 120)
				back.setTextureRect(IntRect(200, 0, 200, 70));
			else
				back.setTextureRect(IntRect(0, 0, 200, 70));
		}

		if (activeright)
		{
			/* Left&Right buttons. */
			{
				if (xpos > 777 && xpos < 817 && ypos > 403 && ypos < 453 && name > 0)
					lr_arrow[0].setTextureRect(IntRect(40, 0, 40, 50));
				else
					lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));

				if (xpos > 1137 && xpos < 1177 && ypos > 403 && ypos < 453 && name < NUMOFZERO)
					lr_arrow[1].setTextureRect(IntRect(120, 0, 40, 50));
				else
					lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
			}

			/* Difficulty buttons. */
			{
				if (bdiff[0] == false)
					if (xpos > 420 + 350 && xpos < 530 + 350 && ypos > 330 && ypos < 380)
						diff[0].setTextureRect(IntRect(110, 0, 110, 50));
					else
						diff[0].setTextureRect(IntRect(0, 0, 110, 50));

				if (bdiff[1] == false)
					if (xpos > 560 + 350 && xpos < 710 + 350 && ypos > 330 && ypos < 380)
						diff[1].setTextureRect(IntRect(480, 0, 150, 50));
					else
						diff[1].setTextureRect(IntRect(330, 0, 150, 50));

				if (bdiff[2] == false)
					if (xpos > 730 + 350 && xpos < 840 + 350 && ypos > 330 && ypos < 380)
						diff[2].setTextureRect(IntRect(890, 0, 110, 50));
					else
						diff[2].setTextureRect(IntRect(780, 0, 110, 50));
			}

			/* Left part. */
			{
				if (xpos > 93 && xpos < 518 && ypos > 183 && ypos < 303)
					cap.setFillColor(Color(39, 39, 39, 100));
				else
					cap.setFillColor(Color(39, 39, 39, 200));
			}
		}
		if (activeleft)
		{
			/* Up&down buttons. */
			if (xpos > 432 && xpos < 467 && ypos > 329 && ypos < 393 + 66)
			{
				hint = true;

				if (xpos > 432 && xpos < 467 && ypos > 329 && ypos < 359 && width < 99)
					ud_arrow[0].setTextureRect(IntRect(35, 0, 35, 30));
				else
					ud_arrow[0].setTextureRect(IntRect(0, 0, 35, 30));

				if (xpos > 432 && xpos < 467 && ypos > 363 && ypos < 393 && width > 3)
					ud_arrow[1].setTextureRect(IntRect(105, 0, 35, 30));
				else
					ud_arrow[1].setTextureRect(IntRect(70, 0, 35, 30));

				if (xpos > 432 && xpos < 467 && ypos > 329 + 66 && ypos < 359 + 66 && height < 99)
					ud_arrow[2].setTextureRect(IntRect(35, 0, 35, 30));
				else
					ud_arrow[2].setTextureRect(IntRect(0, 0, 35, 30));

				if (xpos > 432 && xpos < 467 && ypos > 363 + 66 && ypos < 393 + 66 && height > 3)
					ud_arrow[3].setTextureRect(IntRect(105, 0, 35, 30));
				else
					ud_arrow[3].setTextureRect(IntRect(70, 0, 35, 30));
			}
			else
			{
				hint = false;
				ud_arrow[0].setTextureRect(IntRect(0, 0, 35, 30));
				ud_arrow[1].setTextureRect(IntRect(70, 0, 35, 30));
				ud_arrow[2].setTextureRect(IntRect(0, 0, 35, 30));
				ud_arrow[3].setTextureRect(IntRect(70, 0, 35, 30));
			}

			/* Right part. */
			{
				if (xpos > 778 && xpos < 1142 && ypos > 191 && ypos < 301)
					cap.setFillColor(Color(39, 39, 39, 100));
				else
					cap.setFillColor(Color(39, 39, 39, 200));
			}
		}
	}

	if (event.type == Event::MouseButtonReleased)
	{
		int xpos = event.mouseButton.x;
		int ypos = event.mouseButton.y;

		/* OK and Back buttons. */
		{
			if (xpos > 512 && xpos < 803 && ypos > 510 && ypos < 554)
				if (activeright)
					return 1;
				else
					return 2;

			if (xpos < 265 && xpos > 55 && ypos > 50 && ypos < 120)
			{
				back.setTextureRect(IntRect(0, 0, 200, 70));
				return 0;
			}
		}

		if (activeright)
		{
			/* Left&Right buttons. */
			{
				if (xpos > 777 && xpos < 817 && ypos > 403 && ypos < 453 && name > 0)
				{
					names.setTextureRect(IntRect(--name * 290, 0, 290, 44));
					if (name == 0)
						lr_arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
					if (name < HARDSTART)
					{
						bdiff[0] = false;
						bdiff[1] = true;
						bdiff[2] = false;
						diff[0].setTextureRect(IntRect(0, 0, 110, 50));
						diff[1].setTextureRect(IntRect(630, 0, 150, 50));
						diff[2].setTextureRect(IntRect(780, 0, 110, 50));
					}
					if (name < MEDIUMSTART)
					{
						bdiff[0] = true;
						bdiff[1] = false;
						bdiff[2] = false;
						diff[0].setTextureRect(IntRect(220, 0, 110, 50));
						diff[1].setTextureRect(IntRect(330, 0, 150, 50));
						diff[2].setTextureRect(IntRect(780, 0, 110, 50));
					}
				}
				if (xpos > 1137 && xpos < 1177 && ypos > 403 && ypos < 453 && name < NUMOFZERO)
				{
					names.setTextureRect(IntRect(++name * 290, 0, 290, 44));
					if (name == NUMOFZERO)
						lr_arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
					if (name >= MEDIUMSTART)
					{
						bdiff[0] = false;
						bdiff[1] = true;
						bdiff[2] = false;
						diff[0].setTextureRect(IntRect(0, 0, 110, 50));
						diff[1].setTextureRect(IntRect(630, 0, 150, 50));
						diff[2].setTextureRect(IntRect(780, 0, 110, 50));
					}
					if (name >= HARDSTART)
					{
						bdiff[0] = false;
						bdiff[1] = false;
						bdiff[2] = true;
						diff[0].setTextureRect(IntRect(0, 0, 110, 50));
						diff[1].setTextureRect(IntRect(330, 0, 150, 50));
						diff[2].setTextureRect(IntRect(1000, 0, 110, 50));
					}
				}
			}

			/* Difficulty buttons. */
			{
				if (xpos > 420 + 350 && xpos < 530 + 350 && ypos > 330 && ypos < 380)
				{
					name = 0;
					names.setTextureRect(IntRect(0, 0, 290, 44));
					bdiff[0] = true;
					bdiff[1] = false;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(220, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}

				if (xpos > 560 + 350 && xpos < 710 + 350 && ypos > 330 && ypos < 380)
				{
					name = MEDIUMSTART;
					names.setTextureRect(IntRect(MEDIUMSTART * 290, 0, 290, 44));
					bdiff[0] = false;
					bdiff[1] = true;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(630, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}

				if (xpos > 730 + 350 && xpos < 840 + 350 && ypos > 330 && ypos < 380)
				{
					name = HARDSTART;
					names.setTextureRect(IntRect(HARDSTART * 290, 0, 290, 44));
					bdiff[0] = false;
					bdiff[1] = false;
					bdiff[2] = true;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(1000, 0, 110, 50));
				}
			}

			/* Left part. */
			{
				if (xpos > 93 && xpos < 518 && ypos > 183 && ypos < 303)
				{
					cap.setFillColor(Color(39, 39, 39, 200));
					cap.setPosition(684, 170);
					activeleft = true;
					activeright = false;
				}
			}
		}
		if (activeleft)
		{
			/* Up&down buttons. */
			{
				if (xpos > 432 && xpos < 467 && ypos > 329 && ypos < 359 && width < 99)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
						if (width > 93)
							width = 99;
						else
							width += 5;
					else
						width++;
					size[0].setString(N2S(width));
					if (width == 99)
						ud_arrow[0].setTextureRect(IntRect(0, 0, 35, 40));
				}

				if (xpos > 432 && xpos < 467 && ypos > 363 && ypos < 393 && width > 3)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
						if (width > 8)
							width -= 5;
						else
							width = 3;
					else
						width--;
					size[0].setString(N2S(width));
					if (width == 3)
						ud_arrow[1].setTextureRect(IntRect(70, 0, 35, 40));
				}

				if (xpos > 432 && xpos < 467 && ypos > 329 + 66 && ypos < 359 + 66 && height < 99)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
						if (height > 93)
							height = 99;
						else
							height += 5;
					else
						height++;
					size[1].setString(N2S(height));
					if (height == 99)
						ud_arrow[2].setTextureRect(IntRect(0, 0, 35, 40));
				}

				if (xpos > 432 && xpos < 467 && ypos > 363 + 66 && ypos < 393 + 66 && height > 3)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
					{
						if (height > 8)
							height -= 5;
						else
							height = 3;
					}
					else
						height--;
					size[1].setString(N2S(height));
					if (height == 3)
						ud_arrow[3].setTextureRect(IntRect(70, 0, 35, 40));
				}
			}

			/* Right part. */
			{
				if (xpos > 778 && xpos < 1142 && ypos > 191 && ypos < 301)
				{
					cap.setFillColor(Color(39, 39, 39, 200));
					cap.setPosition(73, 170);
					activeleft = false;
					activeright = true;
				}
			}
		}
	}

	return 3;
}

char MainMenu::CreateMenuWindow()
{
	bool title = true;
	bool play = false;
	bool solve = false;
	char ans;

	RenderWindow menuwindow(VideoMode(MENUWIDTH, MENUHEIGHT), "Menu", Style::Close);

	/* Main menu. */
	while (menuwindow.isOpen())
	{
		Event event;
		while (menuwindow.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				menuwindow.close();
				return 'e';
			}

			if (title)
			{
				ans = TitleEventReaction(event, MENUWIDTH, MENUHEIGHT);

				switch (ans)
				{
				case 'e':
					menuwindow.close();
					return 'e';
					break;
				case 'p':
					title = false;
					play = true;
					continue;
				case 's':
					title = false;
					solve = true;
					continue;
				}
			}

			if (play)
			{
				switch (PlayEventReaction(event))
				{
				case 0:
					play = false;
					title = true;
					break;
				case 1:
					menuwindow.close();
					break;
				}
			}

			if (solve)
			{
				switch (SolveEventReaction(event))
				{
				case 0:
					solve = false;
					title = true;
					break;
				case 1:
					menuwindow.close();
					break;
				case 2:
					ans = 'm';
					menuwindow.close();
					break;
				}
			}
		}

		if (title)
			DrawTitle(menuwindow);
		if (play)
			DrawPlay(menuwindow);
		if (solve)
			DrawSolve(menuwindow);

		menuwindow.display();
	}

	return ans;
}