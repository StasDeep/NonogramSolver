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

#define MENUWIDTH 1280
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
	char name[44];
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
	BWNonogram(char ans, int nonogname)
	{
		switch (nonogname)
		{
		case 0:
			strcpy_s(name, 45, "Nonograms/10x10littlehouse.txt");
			break;
		case 1:
			strcpy_s(name, 45, "Nonograms/11x13skull.txt");
			break;
		case 2:
			strcpy_s(name, 45, "Nonograms/14x17candies.txt");
			break;
		case 3:
			strcpy_s(name, 45, "Nonograms/15x15elephant.txt");
			break;
		case 4:
			strcpy_s(name, 45, "Nonograms/18x18eye.txt");
			break;
		case 5:
			strcpy_s(name, 45, "Nonograms/30x42dancer.txt");
			break;
		case 6:
			strcpy_s(name, 45, "Nonograms/56x26turtle.txt");
			break;
		}
		answer = ans;
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
	Sprite tops;
	Sprite color[2];
	Sprite diff[3];
	Sprite arrow[2];
	Sprite start;
	Sprite names;
	int name = 0;
	bool bcolor[2];
	bool bdiff[3];
	bool p;
	bool s;
	bool e;
	Texture menutex;
	Texture top;
	Texture colorbw;
	Texture diffic;
	Texture arrows;
	Texture tstart;
	Texture tnames;

	MainMenu(int width, int height)
	{
		/* Load textures. */
		{
			menutex.loadFromFile("Images/menu.png");
			colorbw.loadFromFile("Images/settings.png");
			diffic.loadFromFile("Images/settings2.png");
			arrows.loadFromFile("Images/arrowbuttons.png");
			tstart.loadFromFile("Images/startbutton.png");
			tnames.loadFromFile("Images/names.png");
			top.loadFromFile("Images/pmenutop.png");
		}
		
		/* Set title menu sprites.*/
		{
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

		/* Set play menu sprites. */
		{
			tops.setTexture(top);
			tops.setTextureRect(IntRect(0, 0, 1280, 130));
			tops.setPosition(0, 0);

			color[0].setTexture(colorbw);
			color[0].setTextureRect(IntRect(0, 0, 240, 50));
			color[0].setPosition(420, 160);
			bcolor[0] = false;
			color[1].setTexture(colorbw);
			color[1].setTextureRect(IntRect(720, 0, 150, 50));
			color[1].setPosition(687, 160);
			bcolor[1] = false;

			diff[0].setTexture(diffic);
			diff[0].setTextureRect(IntRect(0, 0, 110, 50));
			diff[0].setPosition(420, 260);
			bdiff[0] = false;
			diff[1].setTexture(diffic);
			diff[1].setTextureRect(IntRect(330, 0, 150, 50));
			diff[1].setPosition(560, 260);
			bdiff[1] = false;
			diff[2].setTexture(diffic);
			diff[2].setTextureRect(IntRect(780, 0, 110, 50));
			diff[2].setPosition(730, 260);
			bdiff[2] = false;

			arrow[0].setTexture(arrows);
			arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
			arrow[0].setPosition(440, 360);
			arrow[1].setTexture(arrows);
			arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
			arrow[1].setPosition(800, 360);

			start.setTexture(tstart);
			start.setTextureRect(IntRect(0, 0, 290, 44));
			start.setPosition(495, 470);

			names.setTexture(tnames);
			names.setTextureRect(IntRect(0, 0, 290, 44));
			names.setPosition(495, 364);
		}
	}

	void DrawTitle(RenderWindow &window)
	{
		window.clear(Color(39, 39, 39));

		window.draw(play);
		window.draw(solve);
		window.draw(exit);
	}

	void DrawPlay(RenderWindow &window)
	{
		window.clear(Color(39, 39, 39));

		window.draw(tops);
		window.draw(color[0]);
		window.draw(color[1]);
		window.draw(diff[0]);
		window.draw(diff[1]);
		window.draw(diff[2]);
		window.draw(arrow[0]);
		window.draw(arrow[1]);
		window.draw(start);
		window.draw(names);
	}

	char TitleEventReaction(Event event, int width, int height)
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

	int PlayEventReaction(Event event)
	{
		if (event.type == Event::MouseMoved)
		{
			int xpos = event.mouseMove.x;
			int ypos = event.mouseMove.y;
			
			/* Check mouse movement. */
			{
				if (xpos < 265 && xpos > 55 && ypos > 50 && ypos < 120)
					tops.setTextureRect(IntRect(0, 130, 1280, 130));
				else
					tops.setTextureRect(IntRect(0, 0, 1280, 130));

				if (bcolor[0] == false)
					if (xpos > 420 && xpos < 660 && ypos > 160 && ypos < 210)
						color[0].setTextureRect(IntRect(240, 0, 240, 50));
					else
						color[0].setTextureRect(IntRect(0, 0, 240, 50));

				if (bcolor[1] == false)
					if (xpos > 687 && xpos < 837 && ypos > 160 && ypos < 210 )
						color[1].setTextureRect(IntRect(870, 0, 150, 50));
					else
						color[1].setTextureRect(IntRect(720, 0, 150, 50));
				
				if (bdiff[0] == false)
					if (xpos > 420 && xpos < 530 && ypos > 260 && ypos < 310)
						diff[0].setTextureRect(IntRect(110, 0, 110, 50));
					else
						diff[0].setTextureRect(IntRect(0, 0, 110, 50));

				if (bdiff[1] == false)
					if (xpos > 560 && xpos < 710 && ypos > 260 && ypos < 310)
						diff[1].setTextureRect(IntRect(480, 0, 150, 50));
					else
						diff[1].setTextureRect(IntRect(330, 0, 150, 50));

				if (bdiff[2] == false)
					if (xpos > 730 && xpos < 840 && ypos > 260 && ypos < 310)
						diff[2].setTextureRect(IntRect(890, 0, 110, 50));
					else
						diff[2].setTextureRect(IntRect(780, 0, 110, 50));

				if (xpos > 440 && xpos < 480 && ypos > 360 && ypos < 410 && name > 0)
					arrow[0].setTextureRect(IntRect(40, 0, 40, 50));
				else
					arrow[0].setTextureRect(IntRect(0, 0, 40, 50));

				if (xpos > 800 && xpos < 840 && ypos > 360 && ypos < 410 && name < 6)
					arrow[1].setTextureRect(IntRect(120, 0, 40, 50));
				else
					arrow[1].setTextureRect(IntRect(80, 0, 40, 50));

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
					tops.setTextureRect(IntRect(0, 0, 1280, 130));
					return 0;
				}

				if (xpos > 420 && xpos < 660 && ypos > 160 && ypos < 210)
				{
					bcolor[0] = true;
					bcolor[1] = false;
					color[0].setTextureRect(IntRect(480, 0, 240, 50));
					color[1].setTextureRect(IntRect(720, 0, 150, 50));
				}

				if (xpos > 687 && xpos < 837 && ypos > 160 && ypos < 210)
				{
					bcolor[1] = true;
					bcolor[0] = false;
					color[1].setTextureRect(IntRect(1020, 0, 150, 50));
					color[0].setTextureRect(IntRect(0, 0, 240, 50));
				}

				if (xpos > 420 && xpos < 530 && ypos > 260 && ypos < 310)
				{
					bdiff[0] = true;
					bdiff[1] = false;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(220, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}

				if (xpos > 560 && xpos < 710 && ypos > 260 && ypos < 310)
				{
					bdiff[0] = false;
					bdiff[1] = true;
					bdiff[2] = false;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(630, 0, 150, 50));
					diff[2].setTextureRect(IntRect(780, 0, 110, 50));
				}


				if (xpos > 730 && xpos < 840 && ypos > 260 && ypos < 310)
				{
					bdiff[0] = false;
					bdiff[1] = false;
					bdiff[2] = true;
					diff[0].setTextureRect(IntRect(0, 0, 110, 50));
					diff[1].setTextureRect(IntRect(330, 0, 150, 50));
					diff[2].setTextureRect(IntRect(1000, 0, 110, 50));
				}

				if (xpos > 440 && xpos < 480 && ypos > 360 && ypos < 410 && name > 0)
				{
					names.setTextureRect(IntRect(--name * 290, 0, 290, 44));
					if (name == 0)
						arrow[0].setTextureRect(IntRect(0, 0, 40, 50));
				}

				if (xpos > 800 && xpos < 840 && ypos > 360 && ypos < 410 && name < 6)
				{
					names.setTextureRect(IntRect(++name * 290, 0, 290, 44));
					if (name == 6)
						arrow[1].setTextureRect(IntRect(80, 0, 40, 50));
				}

				if (xpos > 495 && xpos < 785 && ypos > 470 && ypos < 514)
					return 1;
			}
		}
	}
};


int main()
{
	bool title = true;
	bool play = false;
	char ans;
	
	/* Constructing the menu. */
	MainMenu Menu(MENUWIDTH, MENUHEIGHT);

	RenderWindow menuwindow(VideoMode(MENUWIDTH, MENUHEIGHT), "Menu", Style::Close);

	while (menuwindow.isOpen())
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
			
			if (title)
			{
				ans = Menu.TitleEventReaction(event, MENUWIDTH, MENUHEIGHT);

				switch (ans)
				{
				case 'e':
					menuwindow.close();
					return 0;
					break;
				case 'p':
					title = false;
					play = true;
					continue;
				case 's':
					title = false;
					play = true;
					continue;
				}
			}

			if (play)
			{
				switch (Menu.PlayEventReaction(event))
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

			
		}

		if (title)
			Menu.DrawTitle(menuwindow);

		if (play)
			Menu.DrawPlay(menuwindow);

		menuwindow.display();

	}

	

	/* Constructing the field. */
	BWNonogram Field(ans, Menu.name);
	
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
