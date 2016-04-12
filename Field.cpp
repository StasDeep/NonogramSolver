/*
* Field.
*/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp> 
#include <SFML\Window.hpp>

#define MENUWIDTH 1200
#define MENUHEIGHT 600

/* Initialize dynamic 2D array of integer numbers. */
int **CreateArr(int m, int n)
{
	int **arr = new int *[m];
	for (int i = 0; i < m; i++)
		arr[i] = new int[n];
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			arr[i][j] = 0;
	return arr;
}

/* Number to string function. */
std::string N2S(int a)
{
	std::stringstream ss;
	ss << a;
	std::string str;
	ss >> str;
	return str;
}

enum color
{
	PWHITE, /* 0 Probably White. */
	DBLACK, /* 1 Definitely Black. */
	DWHITE  /* 2 Definitely White. */
};

using namespace sf;

class BWCell
{
public:
	Sprite cellsprite;
	color state;
	bool black;
	bool white;
	bool changed;

	/* When clicked, returns new state of the changed cell. */
	int ChangeStateClick(int butt)
	{
		changed = true;
		switch (butt)
		{
		case 0:
		{
			if (state == 0 || state == 2)
			{
				cellsprite.setTextureRect(IntRect(128, 0, 128, 128));
				state = DBLACK;
				return 1;
			}
			else
			{
				cellsprite.setTextureRect(IntRect(0, 0, 128, 128));
				state = PWHITE;
				return 0;
			}
			break;
		}

		case 1:
		{
			if (state == 0 || state == 1)
			{
				cellsprite.setTextureRect(IntRect(256, 0, 128, 128));
				state = DWHITE;
				return 2;
			}
			else
			{
				cellsprite.setTextureRect(IntRect(0, 0, 128, 128));
				state = PWHITE;
				return 0;
			}
			break;
		}

		case 2:
		{
			cellsprite.setTextureRect(IntRect(0, 0, 128, 128));
			state = PWHITE;
			return 0;
			break;
		}

		default:
			break;

		}

	}

	/* Used when dragged or while solving.*/
	void ChangeStateSolve(int newstate)
	{
		switch (newstate)
		{
		case 0:
			cellsprite.setTextureRect(IntRect(0, 0, 128, 128));
			state = PWHITE;
			break;

		case 1:
			cellsprite.setTextureRect(IntRect(128, 0, 128, 128));
			state = DBLACK;
			break;

		case 2:
			cellsprite.setTextureRect(IntRect(256, 0, 128, 128));
			state = DWHITE;
			break;

		default:
			break;
		}
	}

};

class BWNonogram
{
public:
	char name[30] = "Nonograms/Nonogram18x18.txt";
	char answer;			/* Solve or Play? */
	bool solved;			/* If solved (true) nonogram cannot be changed. */
	bool mousepressed;		/* If true, then mouse motion should change cells' state. */
	int button;				/* Remembers new state of the cell. */

	int width;				/* Width of the field, in cells. */
	int height;				/* Height of the field, in cells. */
	int wx;					/* Width of the window. */
	int wy;					/* Height of the window. */
	int cellsize;			/* Size of the cell, in pixels. */
	int hindex;				/* Maximum amount of horizontal blocks. */
	int vindex;				/* Maximum amount of vertical blocks. */
	int hstart;				/* Amount of empty columns in the desctription. */
	int vstart;				/* Amount of empty lines in the desctription. */
	int **horizontal;		/* Description of horizontal blocks from a file. */
	int **vertical;			/* Description of vertical blocks from a file. */
	int **hcurrdescr;		/* Current description of the horizontal blocks, drawn by a user. */
	int **vcurrdescr;		/* Current description of the vertical blocks, drawn by a user. */
	bool *hchange;			/* Detects if the line has changed since last check. */
	bool *vchange;			/* Detects if the column has changed since last check. */
	BWCell **cellarr;		/* Array of cells. */
	Texture celltex;		/* Texture with an image, containing all states. */
	RectangleShape *hline;	/* Array of horizontal lines for grid. */
	RectangleShape *vline;	/* Array of vertical lines for grid. */
	Text **hortext;			/* Array of numbers with horizontal blocks description. */
	Text **vertext;			/* Array of numbers with vertical blocks description. */
	Font font;				/* Font of the description numbers. */

	/* Constructor. */
	BWNonogram()
	{
		button = 0;
		solved = false;
		mousepressed = false;

		ReadDescription();
		CreateField();

		wx = (width + hindex - hstart) * cellsize + 1;
		wy = (height + vindex - vstart) * cellsize + 1;

		/* Check for correct input. */
		int sum = 0;
		for (int i = 0; i < height; i++)
			for (int j = 0; j < hindex; j++)
				sum += horizontal[i][j];

		for (int i = 0; i < vindex; i++)
			for (int j = 0; j < width; j++)
				sum -= vertical[i][j];

		if (sum != 0)
		{
			std::cout << "INCORRECT INPUT.";
			getchar();
		}

	}

	/* Create an array of cells and a grid with lines.. */
	void CreateField()
	{
		/* Array of cells. */
		cellarr = new BWCell *[height];
		for (int i = 0; i < height; i++)
			cellarr[i] = new BWCell[width];

		font.loadFromFile("arial.ttf");

		/* Array of numbers for drawing horizontal blocks description. */
		hortext = new Text *[height];
		{
			for (int i = 0; i < height; i++)
				hortext[i] = new Text[hindex];
			for (int i = 0; i < height; i++)
				for (int j = hstart; j < hindex; j++)
				{
					hortext[i][j].setFont(font);
					hortext[i][j].setCharacterSize(cellsize - cellsize / 5);
					hortext[i][j].setColor(Color(0, 0, 0));
					if (horizontal[i][j] < 10)
						hortext[i][j].setPosition((j - hstart) * cellsize + cellsize / 5, (i + vindex - vstart) * cellsize);
					else
						hortext[i][j].setPosition((j - hstart) * cellsize, (i + vindex - vstart) * cellsize);
					hortext[i][j].setString(N2S(horizontal[i][j]));
				}
		}

		/* Array of numbers for drawing vertical blocks description. */
		vertext = new Text *[vindex];
		{
			for (int i = 0; i < vindex; i++)
				vertext[i] = new Text[width];
			for (int i = vstart; i < vindex; i++)
				for (int j = 0; j < width; j++)
				{
					vertext[i][j].setFont(font);
					vertext[i][j].setCharacterSize(cellsize - cellsize / 5);
					vertext[i][j].setColor(Color(0, 0, 0));
					if (vertical[i][j] < 10)
						vertext[i][j].setPosition((j + hindex - hstart) * cellsize + cellsize / 5, (i - vstart) * cellsize);
					else
						vertext[i][j].setPosition((j + hindex - hstart) * cellsize, (i - vstart) * cellsize);
					vertext[i][j].setString(N2S(vertical[i][j]));
				}
		}

		/* Horizontal lines. */
		hline = new RectangleShape[height + vindex - vstart + 1];
		for (int i = 0; i < height + vindex - vstart + 1; i++)
		{
			hline[i].setFillColor(Color(24, 24, 24, 200));

			if ((i - vindex + vstart) % 5 == 0)
				hline[i].setSize(Vector2f((width + hindex - hstart) * cellsize, 2));
			else
				hline[i].setSize(Vector2f((width + hindex - hstart) * cellsize, 1));

			if (i < vindex - vstart)
			{
				hline[i].setSize(Vector2f(width * cellsize, 1));
				hline[i].setPosition((hindex - hstart) * cellsize, i * cellsize - 1);
			}
			else
				hline[i].setPosition(0, i * cellsize - 1);
		}

		/* Vertical lines. */
		vline = new RectangleShape[width + hindex - hstart + 1];
		for (int i = 0; i < width + hindex - hstart + 1; i++)
		{
			vline[i].setFillColor(Color(24, 24, 24, 200));

			if ((i - hindex + hstart) % 5 == 0)
				vline[i].setSize(Vector2f(2, (height + vindex - vstart) * cellsize));
			else
				vline[i].setSize(Vector2f(1, (height + vindex - vstart) * cellsize));

			if (i < hindex - hstart)
			{
				vline[i].setSize(Vector2f(1, height * cellsize));
				vline[i].setPosition(i * cellsize - 1, (vindex - vstart) * cellsize);
			}
			else
				vline[i].setPosition(i * cellsize - 1, 0);
		}

		/* Array of bools, detecting if the line has changed since last check. */
		hchange = new bool[height];
		for (int i = 0; i < height; i++)
			hchange[i] = true;

		/* Array of bools, detecting if the column has changed since last check. */
		vchange = new bool[width];
		for (int i = 0; i < width; i++)
			vchange[i] = true;

		/* Set cells' texture. */
		celltex.loadFromFile("Images/cells128.png");
		celltex.setSmooth(true);

		/* Set position and state of cells. */
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				cellarr[i][j].cellsprite.setPosition((j + hindex - hstart) * cellsize, (i + vindex - vstart) * cellsize);
				cellarr[i][j].cellsprite.setTexture(celltex);
				cellarr[i][j].cellsprite.scale((float)cellsize / 128, (float)cellsize / 128);
				cellarr[i][j].ChangeStateClick(2);
				cellarr[i][j].black = false;
				cellarr[i][j].white = false;
			}


	}

	/* Read blocks description from a file. */
	void ReadDescription()
	{
#pragma warning (disable: 4996)					
		std::ifstream Descr(name);
		Descr >> width;
		Descr >> height;
		Descr >> vstart;
		Descr >> hstart;

		hindex = (width + 1) / 2;
		vindex = (height + 1) / 2;
		hstart = hindex - hstart;
		vstart = vindex - vstart;

		cellsize = 600 / (height + vindex - vstart);

		horizontal = CreateArr(height, hindex);
		vertical = CreateArr(vindex, width);
		hcurrdescr = CreateArr(height, hindex);
		vcurrdescr = CreateArr(vindex, width);

		/* Read vertical blocks description. */
		for (int i = vstart; i < vindex; i++)
			for (int j = 0; j < width; j++)
				Descr >> vertical[i][j];

		/* Read horizontal blocks description. */
		for (int i = 0; i < height; i++)
			for (int j = hstart; j < hindex; j++)
				Descr >> horizontal[i][j];

		Descr.close();

	}

	/* Walk through blocks, counting their amount and size. */
	void UpdateDescription(int xpos, int ypos)
	{
		/* Reset the description of the line and the column. */
		for (int j = 0; j < hindex; j++)
			hcurrdescr[ypos][j] = 0;
		for (int i = 0; i < vindex; i++)
			vcurrdescr[i][xpos] = 0;


		int blcount = 0;	/* Amount of blocks in a line/column. */
		int blsize = 0;		/* Amount of black cells in a block. */

							/* Walk through horizontal blocks. */
		for (int j = width - 1; j >= 0; j--)
		{
			if (cellarr[ypos][j].state == DBLACK)
			{
				blsize++;
				if (j == 0 || cellarr[ypos][j - 1].state == PWHITE || cellarr[ypos][j - 1].state == DWHITE)
				{
					blcount++;
					hcurrdescr[ypos][hindex - blcount] = blsize;
					blsize = 0;
				}
			}
		}

		blcount = 0;
		blsize = 0;

		/* Walk through vertical blocks. */
		for (int i = height - 1; i >= 0; i--)
		{
			if (cellarr[i][xpos].state == DBLACK)
			{
				blsize++;
				if (i == 0 || cellarr[i - 1][xpos].state == PWHITE || cellarr[i - 1][xpos].state == DWHITE)
				{
					blcount++;
					vcurrdescr[vindex - blcount][xpos] = blsize;
					blsize = 0;
				}
			}
		}
	}

	/* Show the description in console. */
	void ShowDescription()
	{
		int verstart = 0;
		int horstart = 0;
		/* Count the amount of empty lines in vertical blocks description. */
		for (int i = 0; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (vcurrdescr[i][j] != 0)
					break;
				else
					if (j == width - 1)
						verstart++;
			}
			if (verstart != i + 1)
				break;
		}

		/* Count the amount of empty columns in horizontal blocks description. */
		for (int j = 0; j < hindex; j++)
		{
			for (int i = 0; i < height; i++)
			{
				if (hcurrdescr[i][j] != 0)
					break;
				else
					if (i == height - 1)
						horstart++;
			}
			if (horstart != j + 1)
				break;
		}

		system("cls");
		/* Show vertical blocks. */
		for (int i = verstart; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				std::cout << vcurrdescr[i][j] << " ";
			}
			std::cout << "\n";
		}
		std::cout << "-\n";
		/* Show horizontal blocks. */
		for (int i = 0; i < height; i++)
		{
			for (int j = horstart; j < hindex; j++)
			{
				std::cout << hcurrdescr[i][j] << " ";
			}
			std::cout << "\n";
		}
	}

	/* Writes current description to NewNonogram.txt. */
	void SaveDescription()
	{
		int verstart = 0;
		int horstart = 0;
		/* Count the amount of empty lines in vertical blocks description. */
		for (int i = 0; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (vcurrdescr[i][j] != 0)
					break;
				else
					if (j == width - 1)
						verstart++;
			}
			if (verstart != i + 1)
				break;
		}

		/* Count the amount of empty columns in horizontal blocks description. */
		for (int j = 0; j < hindex; j++)
		{
			for (int i = 0; i < height; i++)
			{
				if (hcurrdescr[i][j] != 0)
					break;
				else
					if (i == height - 1)
						horstart++;
			}
			if (horstart != j + 1)
				break;
		}

		/* Write current description to the new file. */
		std::ofstream Descr;
		Descr.open("Nonograms/NewNonogram.txt");
		Descr << width << " " << height << " ";
		Descr << vindex - verstart << " " << hindex - horstart << "\n";

		/* Write vertical blocks description. */
		for (int i = verstart; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
				Descr << vcurrdescr[i][j] << " ";
			Descr << "\n";
		}

		/* Write horizontal blocks description. */
		for (int i = 0; i < height; i++)
		{
			for (int j = horstart; j < hindex; j++)
				Descr << hcurrdescr[i][j] << " ";
			Descr << "\n";
		}
		Descr.close();

	}

	/* Check if the user's solving is correct. */
	bool CheckUser()
	{
		/* Vertical check. */
		for (int i = 0; i < vindex; i++)
			for (int j = 0; j < width; j++)
				if (vertical[i][j] != vcurrdescr[i][j])
					return false;

		/* Horizontal check.*/
		for (int i = 0; i < height; i++)
			for (int j = 0; j < hindex; j++)
				if (horizontal[i][j] != hcurrdescr[i][j])
					return false;

		return true;
	}

	/* Call main algorithm and check if the solving is correct. */
	bool Solve()
	{
		if (answer != 's' || solved == true)
			return false;

		bool checkifsolved = true;

		CheckHor();
		for (int j = 0; j < height; j++)
			hchange[j] = false;
		CheckVert();
		for (int j = 0; j < width; j++)
			vchange[j] = false;

		/* Detects solved nonogram by finding 0 PWHITE cells. */
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				if (cellarr[i][j].state == PWHITE)
				{
					checkifsolved = false;
					break;
				}
			if (!checkifsolved)
				break;
		}

		if (checkifsolved)
		{
			std::system("cls");
			std::cout << "CORRECT!\nCONGRATULATIONS!";
			solved = true;
		}

		return checkifsolved;

	}

	/* Check all horizontal lines. */
	void CheckHor()
	{
		int sum;			/* Minimum amount of cells, used for the blocks and gaps between. */
		int nonzero;		/* Number of the block, counting starts from.*/
		int maxcell;		/* Length of the longest block in the line. */
		bool emptyline;		/* True if the line is empty. */

		for (int j = 0; j < height; j++)
		{
			/* Immediately goes to the next line if the current one hasn't changed since last check. */
			if (hchange[j] == false)
				continue;

			emptyline = true;
			/* Check if the line is empty. */
			for (int i = 0; i < width; i++)
				if (cellarr[j][i].state != PWHITE)
				{
					emptyline = false;
					break;
				}

			/* Check if there are any intersecting blocks in the empty line. */
			if (emptyline)
			{
				maxcell = 0;
				sum = 0;
				for (int i = 0; i < hindex; i++)
				{
					sum += horizontal[j][i];
					if (horizontal[j][i] != 0)
						sum++;
					if (horizontal[j][i] > maxcell)
						maxcell = horizontal[j][i];
				}

				if (sum == 0)
				{
					for (int i = 0; i < width; i++)
						cellarr[j][i].ChangeStateSolve(2);
					continue;
				}

				if ((width - sum + 1 + maxcell) / 2 >= maxcell)
					continue;

			}

			/* Reset flags. */
			for (int i = 0; i < width; i++)
			{
				cellarr[j][i].white = false;
				cellarr[j][i].black = false;
			}

			sum = 0;
			nonzero = -1;
			/* Count the maximum value in the description line. */
			for (int i = 0; i < hindex; i++)
			{
				sum += horizontal[j][i];
				if (horizontal[j][i] != 0)
				{
					if (nonzero == -1)
						nonzero = i;
					sum++;
				}
			}

			/* If the line is empty, go to the next one. */
			if (sum == 0)
				continue;

			/* Recursive procedure of finding each possible combination. */
			for (int i = 0; i < width - sum + 2; i++)
				TryHorBlock(nonzero, i, j);

			if (width <= sum - 2)
				std::cout << "INCORR INPUT.\n";

			/* Main check. */
			for (int i = 0; i < width; i++)
			{
				if (cellarr[j][i].white ^ cellarr[j][i].black)
				{
					/* If the state wasn't definite, then it has changed. */
					if (cellarr[j][i].state == PWHITE)
						vchange[i] = true;

					if (cellarr[j][i].white)
						cellarr[j][i].ChangeStateSolve(DWHITE);

					if (cellarr[j][i].black)
						cellarr[j][i].ChangeStateSolve(DBLACK);

				}
			}
		}


	}

	/* Check all vertical lines. */
	void CheckVert()
	{
		int sum;			/* Minimum amount of cells, used for the blocks and gaps between. */
		int nonzero;		/* Number of the block, counting starts from.*/
		int maxcell;		/* Length of the longest block in the line. */
		bool emptycol;		/* True if the column is empty. */
		for (int j = 0; j < width; j++)
		{
			/* Immediately goes to the next column if the current one hasn't changed since last check. */
			if (vchange[j] == false)
				continue;

			emptycol = true;
			/* Check if the column is empty. */
			for (int i = 0; i < height; i++)
				if (cellarr[i][j].state != PWHITE)
				{
					emptycol = false;
					break;
				}

			/* Check if there are any intersecting blocks in the empty column. */
			if (emptycol)
			{
				maxcell = 0;
				sum = 0;
				for (int i = 0; i < vindex; i++)
				{
					sum += vertical[i][j];
					if (vertical[i][j] != 0)
						sum++;
					if (vertical[i][j] > maxcell)
						maxcell = vertical[i][j];
				}

				if (sum == 0)
				{
					for (int i = 0; i < height; i++)
						cellarr[i][j].ChangeStateSolve(2);
					continue;
				}

				if ((height - sum + 1 + maxcell) / 2 >= maxcell)
					continue;

			}

			/* Reset flags. */
			for (int i = 0; i < height; i++)
			{
				cellarr[i][j].white = false;
				cellarr[i][j].black = false;
			}

			sum = 0;
			nonzero = -1;
			/* Count the maximum value in the description line. */
			for (int i = 0; i < vindex; i++)
			{
				sum += vertical[i][j];
				if (vertical[i][j] != 0)
				{
					if (nonzero == -1)
						nonzero = i;
					sum++;
				}
			}

			/* If the column is empty, go to the next one. */
			if (sum == 0)
				continue;

			/* Recursive procedure of finding each possible combination. */
			for (int i = 0; i < height - sum + 2; i++)
				TryVertBlock(nonzero, i, j);

			if (height <= sum - 2)
				std::cout << "INCORR INPUT.\n";

			/* Main check. */
			for (int i = 0; i < height; i++)
			{
				if (cellarr[i][j].white ^ cellarr[i][j].black)
				{
					/* If the state wasn't definite, then it has changed. */
					if (cellarr[i][j].state == PWHITE)
						hchange[i] = true;

					if (cellarr[i][j].white)
						cellarr[i][j].ChangeStateSolve(DWHITE);

					if (cellarr[i][j].black)
						cellarr[i][j].ChangeStateSolve(DBLACK);

				}
			}
		}

	}

	/* Check each possible horizontal combination. */
	bool TryHorBlock(int theblock /* Number of the block in horizontal array. */, int thestart /* Position check starts with. */, int line)
	{
		bool result;

		/* Check if it's possible to place the block on this position. */
		for (int i = thestart; i < thestart + horizontal[line][theblock]; i++)
			if (cellarr[line][i].state == DWHITE)
				return false;

		/* Process the case, when the block is the first, but is placed not on the first cell. */
		if ((theblock != 0 && horizontal[line][theblock - 1] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				if (cellarr[line][i].state == DBLACK)
					return false;
		if ((theblock != 0 && horizontal[line][theblock - 1] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				cellarr[line][i].white = true;

		/* Process the case, when the block is not the last in the line. */
		if (theblock < hindex - 1)
		{
			result = false;

			/*
			* Cycle starts from the very left position of the next block.
			* That is 2 cells to the right of the last cell of the current block.
			*
			* Cycle ends on the last position starting from which it is possible to place the next block.
			*/
			for (int startnext = thestart + horizontal[line][theblock] + 1; startnext < width - horizontal[line][theblock + 1] + 1; startnext++)
			{
				/* If the gap cell is already black, there is no reason to continue checking */
				if (cellarr[line][startnext - 1].state == DBLACK)
					break;

				/* Recurrent check of the next block on the 'startnext' position. */
				if (TryHorBlock(theblock + 1, startnext, line))
				{
					for (int i = thestart; i < thestart + horizontal[line][theblock]; i++)
						cellarr[line][i].black = true;
					for (int i = thestart + horizontal[line][theblock]; i < startnext; i++)
						cellarr[line][i].white = true;

					result = true;
				}
			}

			return result;

		}
		else /* Current block is the last. */
		{
			for (int i = thestart + horizontal[line][theblock]; i < width; i++)
				if (cellarr[line][i].state == DBLACK)
					return false;


			for (int j = thestart; j < thestart + horizontal[line][theblock]; j++)
				cellarr[line][j].black = true;
			for (int j = thestart + horizontal[line][theblock]; j < width; j++)
				cellarr[line][j].white = true;

			return true;

		}

	}

	/* Check each possible vertical combination. */
	bool TryVertBlock(int theblock, int thestart, int col)
	{
		bool result;

		/* Check if it's possible to place the block on this position. */
		for (int i = thestart; i < thestart + vertical[theblock][col]; i++)
			if (cellarr[i][col].state == DWHITE)
				return false;

		/* Process the case, when the block is the first, but is placed not on the first cell. */
		if ((theblock != 0 && vertical[theblock - 1][col] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				if (cellarr[i][col].state == DBLACK)
					return false;
		if ((theblock != 0 && vertical[theblock - 1][col] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				cellarr[i][col].white = true;

		/* Process the case, when the block is not the last in the column. */
		if (theblock < vindex - 1)
		{
			result = false;

			/*
			* Cycle starts from the very top of the next block.
			* That is 2 cells down of the last cell of the current block.
			*
			* Cycle ends on the last position starting from which it is possible to place the next block.
			*/
			for (int startnext = thestart + vertical[theblock][col] + 1; startnext < height - vertical[theblock + 1][col] + 1; startnext++)
			{
				/* If the gap cell is already black, there is no reason to continue checking */
				if (cellarr[startnext - 1][col].state == DBLACK)
					break;

				/* Recurrent check of the next block on the 'startnext' position. */
				if (TryVertBlock(theblock + 1, startnext, col))
				{
					for (int i = thestart; i < thestart + vertical[theblock][col]; i++)
						cellarr[i][col].black = true;
					for (int i = thestart + vertical[theblock][col]; i < startnext; i++)
						cellarr[i][col].white = true;

					result = true;
				}
			}

			return result;

		}
		else /* Current block is the last. */
		{

			for (int i = thestart + vertical[theblock][col]; i < height; i++)
				if (cellarr[i][col].state == DBLACK)
					return false;


			for (int j = thestart; j < thestart + vertical[theblock][col]; j++)
				cellarr[j][col].black = true;
			for (int j = thestart + vertical[theblock][col]; j < height; j++)
				cellarr[j][col].white = true;

			return true;

		}

	}

	/* Event switch case. */
	void EventReaction(Event event)
	{
		int xpos, ypos;

		switch (event.type)
		{
		case Event::MouseButtonPressed:
			if (answer == 'p' && solved == false)
			{
				xpos = (event.mouseButton.x - (hindex - hstart) * cellsize) / cellsize;
				ypos = (event.mouseButton.y - (vindex - vstart) * cellsize) / cellsize;

				/* Breaks if the click is not within the field. */
				if (xpos < 0 || ypos < 0)
					break;

				/* If mouse button has been pressed, then no cells have been changed yet. */
				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
						cellarr[i][j].changed = false;

				/* Remember new state to change every cell under dragging cursor to this state. */
				button = cellarr[ypos][xpos].ChangeStateClick(event.mouseButton.button);

				UpdateDescription(xpos, ypos);

				if (CheckUser())
				{
					std::system("cls");
					std::cout << "CORRECT!\nCONGRATULATIONS!";
					solved = true;
				}

				mousepressed = true;
			}
			break;

		case Event::MouseMoved:
			/* Works only when the mouse button is pressed.*/
			if (mousepressed)
			{
				xpos = (event.mouseMove.x - (hindex - hstart) * cellsize) / cellsize;
				ypos = (event.mouseMove.y - (vindex - vstart) * cellsize) / cellsize;

				/* Breaks if the click is not within the field. */
				if (xpos < 0 || ypos < 0 || xpos >= width || ypos >= height)
					break;

				/* If thе cell hasn't been changed it is changed to remembered state. */
				if (cellarr[ypos][xpos].changed == false)
					cellarr[ypos][xpos].ChangeStateSolve(button);

				UpdateDescription(xpos, ypos);

				if (CheckUser())
				{
					std::system("cls");
					std::cout << "CORRECT!\nCONGRATULATIONS!";
					solved = true;
				}
			}

			break;

		case Event::MouseButtonReleased:
			/* If mouse button is not pressed anymore, mouse movement shouldn't cause cell change. */
			mousepressed = false;
			break;

		case Event::KeyPressed:
			/* Stop solving. */
			if (event.key.code == Keyboard::Space && answer == 's')
				solved = true;

			if (event.key.code == Keyboard::H && answer == 'p')
				ShowDescription();

			if (event.key.code == Keyboard::S && answer == 'p')
				SaveDescription();

			break;

		default:
			break;
		}
	}

	/* Draw field, grid and description. */
	void Draw(RenderWindow &window)
	{
		window.clear(Color(173, 173, 173));

		/* Field. */
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				window.draw(cellarr[i][j].cellsprite);

		/* Grid. */
		for (int i = 0; i < height + vindex - vstart + 1; i++)
			window.draw(hline[i]);
		for (int i = 0; i < width + hindex - hstart + 1; i++)
			window.draw(vline[i]);

		/* Horizontal description. */
		for (int i = 0; i < height; i++)
			for (int j = hstart; j < hindex; j++)
				if (horizontal[i][j])
					window.draw(hortext[i][j]);


		/* Vertical description. */
		for (int i = vstart; i < vindex; i++)
			for (int j = 0; j < width; j++)
				if (vertical[i][j])
					window.draw(vertext[i][j]);
	}
};

class MainMenu
{
public:
	Sprite play;
	Sprite solve;
	Sprite exit;
	bool p;
	bool s;
	bool e;
	Texture menutex;

	MainMenu(int width, int height)
	{
		menutex.loadFromFile("Images/menu.png");
		menutex.setSmooth(true);
		p = s = e = false;

		play.setTexture(menutex);
		play.setTextureRect(IntRect(0, 0, 270, 100));
		play.setOrigin(135, 50);
		play.setPosition(width / 2, height / 2 - 100);

		solve.setTexture(menutex);
		solve.setTextureRect(IntRect(270, 0, 270, 100));
		solve.setOrigin(135, 50);
		solve.setPosition(width / 2, height / 2);

		exit.setTexture(menutex);
		exit.setTextureRect(IntRect(540, 0, 270, 100));
		exit.setOrigin(135, 50);
		exit.setPosition(width / 2, height / 2 + 100);
	}

	void DrawMain(RenderWindow &window)
	{
		window.clear(Color(39, 39, 39));

		window.draw(play);
		window.draw(solve);
		window.draw(exit);
	}

	char EventReaction(Event event, int width, int height)
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
				if (ypos > height / 2 - 150 && ypos < height / 2 - 50)
					return 'p';
				if (ypos > height / 2 - 50 && ypos < height / 2 + 50)
					return 's';
				if (ypos > height / 2 + 50 && ypos < height / 2 + 150)
					return 'e';
			}
		}
		return '0';
	}
};


int main()
{
	bool menu = true;

	/* Constructing the field. */
	BWNonogram Field;

	/* Constructing the menu. */
	MainMenu Menu(MENUWIDTH, MENUHEIGHT);

	RenderWindow menuwindow(VideoMode(MENUWIDTH, MENUHEIGHT), "Menu", Style::Close);

	while (menuwindow.isOpen() && menu == true)
	{
		Event event;
		while (menuwindow.pollEvent(event))
		{
			/* Exit from the solving. */
			/*if (menu == false &&
			event.type == Event::MouseButtonReleased &&
			event.mouseButton.x < (Field.hindex - Field.hstart) * Field.cellsize &&
			event.mouseButton.y < (Field.vindex - Field.vstart) * Field.cellsize)
			{
			menu = true;
			Field.mousepressed = false;
			}*/

			if (event.type == Event::Closed)
			{
				menuwindow.close();
				return 0;
			}
			else
				Field.answer = (Menu.EventReaction(event, MENUWIDTH, MENUHEIGHT));

			if (Field.answer == 'p' || Field.answer == 's')
			{
				menuwindow.close();
				menu = false;
			}

			if (Field.answer == 'e')
			{
				menuwindow.close();
				return 0;
			}
		}

		Menu.DrawMain(menuwindow);

		menuwindow.display();

	}

	/* Create a window. */
	RenderWindow window(VideoMode(Field.wx, Field.wy), "Field", Style::Close);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			/* Exit from the solving. */
			/*if (menu == false &&
				event.type == Event::MouseButtonReleased &&
				event.mouseButton.x < (Field.hindex - Field.hstart) * Field.cellsize &&
				event.mouseButton.y < (Field.vindex - Field.vstart) * Field.cellsize)
			{
				menu = true;
				Field.mousepressed = false;
			}*/

			if (event.type == Event::Closed)
				window.close();
			else
				Field.EventReaction(event);
		}

		
		Field.Draw(window);

		window.display();

		Field.Solve();

	}

	return 0;
}
