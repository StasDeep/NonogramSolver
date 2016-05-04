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
#define WINWIDTH MENUWIDTH
#define WINHEIGHT MENUHEIGHT
#define DELETE_2D_ARRAY(arr, height) for (int i = 0; i < height; ++i)\
	delete[] arr[i];\
	delete[] arr;
#define DELETE_1D_ARRAY(arr) delete[] arr;


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

		/*case 2:
		{
			cellsprite.setTextureRect(IntRect(0, 0, 128, 128));
			state = PWHITE;
			return 0;
			break;
		}*/

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
	bool win;
	bool solved;			/* If solved (true) nonogram cannot be changed. */
	bool mousepressed;		/* If true, then mouse motion should change cells' state. */
	bool middlepressed;		/* If true, then mouse motion changes view. */
	int button;				/* Remembers new state of the cell. */
	int mxpos;				/* Stores x-position of the last middle-button click. */
	int mypos;				/* Stores y-position of the last middle-button click. */
	int fieldx;				/* X-shift. */
	int fieldy;				/* Y-shift. */

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
	Texture hamtex;			/* Texture with hamburger icon. */
	Sprite ham;				/* "Hamburger" which calls menu. */
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
		win = false;
		solved = false;
		mousepressed = false;
		middlepressed = false;
		fieldx = 0;
		fieldy = 0;

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
			ErrorHint();
			std::cout << "Incorrect input.";
			getchar();
		}

	}

	/* Constructor for manual input. */
	BWNonogram(int x, int y)
	{
		answer = 'm';
		button = 0;
		win = false;
		solved = false;
		mousepressed = false;
		middlepressed = false;
		fieldx = 0;
		fieldy = 0;

		width = x;
		height = y;
		hindex = (width + 1) / 2;
		vindex = (height + 1) / 2;
		hstart = 0;
		vstart = 0;
		
		cellsize = 32;
		
		horizontal = CreateArr(height, hindex);
		vertical = CreateArr(vindex, width);
		hcurrdescr = CreateArr(height, hindex);
		vcurrdescr = CreateArr(vindex, width);

		CreateField();

		wx = (width + hindex - hstart) * cellsize + 1;
		wy = (height + vindex - vstart) * cellsize + 1;
	}

	/* Destructor. */
	/*~BWNonogram()
	{		
		DELETE_2D_ARRAY(hcurrdescr, height);
		DELETE_2D_ARRAY(vcurrdescr, vindex);	
		DELETE_2D_ARRAY(horizontal, height);
		DELETE_2D_ARRAY(vertical, vindex);		
		DELETE_2D_ARRAY(cellarr, height);
		DELETE_2D_ARRAY(hortext, height);
		DELETE_2D_ARRAY(vertext, vindex);
		DELETE_1D_ARRAY(hchange);
		DELETE_1D_ARRAY(vchange);
		DELETE_1D_ARRAY(hline);
		DELETE_1D_ARRAY(vline);
	}*/

	/* Create an array of cells and a grid with lines.. */
	void CreateField()
	{
		/* Array of cells. */
		cellarr = new BWCell *[height];
		for (int i = 0; i < height; i++)
			cellarr[i] = new BWCell[width];

		font.loadFromFile("Proxima Nova Regular.otf");

		/* Array of numbers for drawing horizontal blocks description. */
		hortext = new Text *[height];
		{
			for (int i = 0; i < height; i++)
				hortext[i] = new Text[hindex];
			for (int i = 0; i < height; i++)
				for (int j = hstart; j < hindex; j++)
				{
					hortext[i][j].setFont(font);
					hortext[i][j].setCharacterSize(cellsize - cellsize / 3);
					hortext[i][j].setColor(Color(0, 0, 0));
					hortext[i][j].setString(N2S(horizontal[i][j]));

					FloatRect textRect = hortext[i][j].getLocalBounds();
					int x = textRect.left + textRect.width / 2;
					int y = textRect.top + textRect.height / 2;
					hortext[i][j].setOrigin(x, y);
					hortext[i][j].setPosition((j - hstart) * cellsize + cellsize / 2, (i + vindex - vstart) * cellsize + cellsize / 2);
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
					vertext[i][j].setCharacterSize(cellsize - cellsize / 3);
					vertext[i][j].setColor(Color(0, 0, 0));
					vertext[i][j].setString(N2S(vertical[i][j]));

					FloatRect textRect = vertext[i][j].getLocalBounds();
					int x = textRect.left + textRect.width / 2;
					int y = textRect.top + textRect.height / 2;
					vertext[i][j].setOrigin(x, y);
					vertext[i][j].setPosition((j + hindex - hstart) * cellsize + cellsize / 2, (i - vstart) * cellsize + cellsize / 2);
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

		hamtex.loadFromFile("Images/Hamburger.png");
		ham.setTexture(hamtex);
		ham.setPosition(0, 0);
		ham.setColor(Color(255, 255, 255, 100));

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
				cellarr[i][j].ChangeStateSolve(0);
				cellarr[i][j].black = false;
				cellarr[i][j].white = false;
			}


	}

	/* Read blocks description from a file. */
	void ReadDescription()
	{				
		std::ifstream Descr(name);
		Descr >> width;
		Descr >> height;
		Descr >> vstart;
		Descr >> hstart;
		
		hindex = (width + 1) / 2;
		vindex = (height + 1) / 2;
		hstart = hindex - hstart;
		vstart = vindex - vstart;

		cellsize = 32;

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
	void SaveDrawnDescription()
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

	/* Writes current description to NewNonogram.txt. */
	void SaveManualDescription()
	{
		int verstart = 0;
		int horstart = 0;
		/* Count the amount of empty lines in vertical blocks description. */
		for (int i = 0; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (vertical[i][j] != 0)
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
				if (horizontal[i][j] != 0)
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
				Descr << vertical[i][j] << " ";
			Descr << "\n";
		}

		/* Write horizontal blocks description. */
		for (int i = 0; i < height; i++)
		{
			for (int j = horstart; j < hindex; j++)
				Descr << horizontal[i][j] << " ";
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

	/* The last check for correct solve. */
	bool LastCheck()
	{
		int blcount = 0;	/* Amount of blocks in a line/column. */
		int blsize = 0;		/* Amount of black cells in a block. */

		/* Walk through horizontal blocks. */
		for (int i = 0; i < height; i++)
		{
			blcount = 0;
			blsize = 0;
			for (int j = width - 1; j >= 0; j--)
			{
				if (cellarr[i][j].state == DBLACK)
				{
					blsize++;
					if (j == 0 || cellarr[i][j - 1].state == PWHITE || cellarr[i][j - 1].state == DWHITE)
					{
						blcount++;
						hcurrdescr[i][hindex - blcount] = blsize;
						blsize = 0;
					}
				}
			}
		}

		/* Walk through vertical blocks. */
		for (int j = 0; j < width; j++)
		{
			blcount = 0;
			blsize = 0;
			for (int i = height - 1; i >= 0; i--)
			{
				if (cellarr[i][j].state == DBLACK)
				{
					blsize++;
					if (i == 0 || cellarr[i - 1][j].state == PWHITE || cellarr[i - 1][j].state == DWHITE)
					{
						blcount++;
						vcurrdescr[vindex - blcount][j] = blsize;
						blsize = 0;
					}
				}
			}
		}
	
		return CheckUser();
	}

	/* Call main algorithm and check if the solving is correct. */
	int Solve(bool first)
	{
		if (answer != 's' && answer != 'm' || solved == true)
			return 0;

		/* The easiest check for incorrect input. Works only once. */
		if (first)
		{
			int sum = 0;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < hindex; j++)
					sum += horizontal[i][j];
			for (int i = 0; i < vindex; i++)
				for (int j = 0; j < width; j++)
					sum -= vertical[i][j];
			if (sum != 0)
				return 2;
		}
		
		/* Check if the algorithm has not solved but stopped. */
		if (answer == 'm')
		{
			bool solvingfreeze = true;;
			for (int j = 0; j < height; j++)
				if (hchange[j] == true)
				{
					solvingfreeze = false;
					break;
				}
			if (solvingfreeze)
				for (int j = 0; j < width; j++)
					if (vchange[j] == true)
					{
						solvingfreeze = false;
						break;
					}
			if (solvingfreeze)
				return 2;
		}

		CheckHor();
		for (int j = 0; j < height; j++)
			hchange[j] = false;
		CheckVert();
		for (int j = 0; j < width; j++)
			vchange[j] = false;		

		/* Detects solved nonogram by finding 0 PWHITE cells. */
		bool checkifsolved = true;
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
			if (LastCheck() == false)
			{
				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
					{
						cellarr[i][j].ChangeStateSolve(0);
						cellarr[i][j].black = false;
						cellarr[i][j].white = false;
					}
				return 2;
			}
			std::system("cls");
			std::cout << "CORRECT!\nCONGRATULATIONS!";
			solved = true;
			return 1;
		}

		return 0;

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

	/* Window showing main shortcuts for Play/Solve mode. */
	void PlaySolveHint()
	{
		RenderWindow window(VideoMode(626, 345), "PSHint", Style::Close);

		Sprite hint;
		Texture hinttex;
		hinttex.loadFromFile("Images/PSHint.png");
		hint.setTexture(hinttex);
		hint.setPosition(0, 0);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)				
					window.close();				
			}		
			window.clear(Color(37, 37, 37));

			window.draw(hint);

			window.display();
		}
	}

	/* Window showing main shortcuts for Manual input mode. */
	void ManualHint()
	{
		RenderWindow window(VideoMode(626, 377), "MHint", Style::Close);

		Sprite hint;
		Texture hinttex;
		hinttex.loadFromFile("Images/ManHint.png");
		hint.setTexture(hinttex);
		hint.setPosition(0, 0);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
			}
			window.clear(Color(37, 37, 37));

			window.draw(hint);

			window.display();
		}
	}

	/* Window showing error. */
	void ErrorHint()
	{
		RenderWindow window(VideoMode(373, 301), "Error", Style::Close);

		Sprite hint;
		Texture hinttex;
		hinttex.loadFromFile("Images/Error.png");
		hint.setTexture(hinttex);
		hint.setPosition(0, 0);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
			}
			window.clear(Color(37, 37, 37));

			window.draw(hint);

			window.display();
		}
	}

	/* Window showing congratulations. */
	void WinHint()
	{
		RenderWindow window(VideoMode(367, 261), "Congratulations!", Style::Close);
		Sprite hint;
		Texture hinttex;
		hinttex.loadFromFile("Images/Win.png");
		hint.setTexture(hinttex);
		hint.setPosition(0, 0);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
			}
			window.clear(Color(37, 37, 37));

			window.draw(hint);

			window.display();
		}
	}

	/* Event switch case. */
	void EventReaction(Event event, View &view)
	{
		int xpos, ypos;

		switch (event.type)
		{
		case Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == Mouse::Middle || event.mouseButton.button == Mouse::Left && Keyboard::isKeyPressed(Keyboard::LAlt))
			{
				mxpos = event.mouseButton.x;
				mypos = event.mouseButton.y;
				middlepressed = true;
				break;
			}

			if (answer == 'p' && solved == false)
			{
				xpos = (event.mouseButton.x - fieldx - (hindex - hstart) * cellsize);
				ypos = (event.mouseButton.y - fieldy - (vindex - vstart) * cellsize);

				/* Breaks if the click is not within the field. */
				if (xpos < 0 || ypos < 0)
					break;

				xpos /= cellsize;
				ypos /= cellsize;

				/* If mouse button has been pressed, then no cells have been changed yet. */
				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
						cellarr[i][j].changed = false;

				/* Remember new state to change every cell under dragging cursor to this state. */
				button = cellarr[ypos][xpos].ChangeStateClick(event.mouseButton.button);

				UpdateDescription(xpos, ypos);

				if (CheckUser() && solved == false)
				{
					std::system("cls");
					std::cout << "CORRECT!\nCONGRATULATIONS!";
					solved = true;
				}

				mousepressed = true;
			}
			break;
		}

		case Event::MouseMoved:
		{
			/* Works only when the mouse button is pressed.*/
			if (mousepressed)
			{
				xpos = (event.mouseMove.x - fieldx - (hindex - hstart) * cellsize) / cellsize;
				ypos = (event.mouseMove.y - fieldy - (vindex - vstart) * cellsize) / cellsize;

				/* Breaks if the click is not within the field. */
				if (xpos < 0 || ypos < 0 || xpos >= width || ypos >= height)
					break;

				/* If thе cell hasn't been changed it is changed to remembered state. */
				if (cellarr[ypos][xpos].changed == false)
					cellarr[ypos][xpos].ChangeStateSolve(button);

				UpdateDescription(xpos, ypos);

				if (CheckUser() && solved == false)
				{
					std::system("cls");
					std::cout << "CORRECT!\nCONGRATULATIONS!";
					solved = true;
				}
			}

			if (middlepressed)
			{
				if (wx > WINWIDTH)
				{					
					if (fieldx - mxpos + event.mouseMove.x <= 0 && fieldx - mxpos + event.mouseMove.x >= WINWIDTH - wx)
					{
						view.move(mxpos - event.mouseMove.x, 0);
						fieldx -= mxpos - event.mouseMove.x;
						mxpos = event.mouseMove.x;
					}
					else
					{
						if (fieldx - mxpos + event.mouseMove.x > 0)
						{
							view.move(fieldx, 0);
							fieldx = 0;
						}
						if (fieldx - mxpos + event.mouseMove.x < WINWIDTH - wx)
						{
							view.move(fieldx - WINWIDTH + wx, 0);
							fieldx = WINWIDTH - wx;
						}
					}
				}
				if (wy > WINHEIGHT)
				{
					if (fieldy - mypos + event.mouseMove.y <= 0 && fieldy - mypos + event.mouseMove.y >= WINHEIGHT - wy)
					{
						view.move(0, mypos - event.mouseMove.y);
						fieldy -= mypos - event.mouseMove.y;
						mypos = event.mouseMove.y;
					}
					else
					{
						if (fieldy - mypos + event.mouseMove.y > 0)
						{
							view.move(0, fieldy);
							fieldy = 0;
						}
						if (fieldy - mypos + event.mouseMove.y < WINHEIGHT - wy)
						{
							view.move(0, fieldy - WINHEIGHT + wy);
							fieldy = WINHEIGHT - wy;
						}
					}
				}
			
				ham.setPosition(-fieldx, -fieldy);
			}

			if (event.mouseMove.x > 0 && event.mouseMove.x < 63 && event.mouseMove.y > 0 && event.mouseMove.y < 63)
				ham.setColor(Color(255, 255, 255, 200));
			else
				ham.setColor(Color(255, 255, 255, 100));

			break;
		}
			
		case Event::MouseButtonReleased:
		{
			/* If mouse button is not pressed anymore, mouse movement shouldn't cause cell change. */
			mousepressed = false;
			middlepressed = false;

			if (event.mouseButton.x > 0 && event.mouseButton.x < 63 && event.mouseButton.y > 0 && event.mouseButton.y < 63)
				PlaySolveHint();

			break;
		}

		case Event::KeyPressed:
		{
			/* Stop solving. */
			if (event.key.code == Keyboard::Space && answer == 's')
				solved = true;

			if (event.key.code == Keyboard::H && answer == 'p')
				ShowDescription();

			if (event.key.code == Keyboard::S && answer == 'p')
				SaveDrawnDescription();

			break;
		}

		default:
			break;
		}
	}

	/* Event for manual mode. */
	void ManEventReaction(Event event, View &view)
	{		
		switch (event.type)
		{
		case Event::MouseMoved:
		{
			if (middlepressed)
			{
				if (wx > WINWIDTH)
				{
					if (fieldx - mxpos + event.mouseMove.x <= 0 && fieldx - mxpos + event.mouseMove.x >= WINWIDTH - wx)
					{
						view.move(mxpos - event.mouseMove.x, 0);
						fieldx -= mxpos - event.mouseMove.x;
						mxpos = event.mouseMove.x;
					}
					else
					{
						if (fieldx - mxpos + event.mouseMove.x > 0)
						{
							view.move(fieldx, 0);
							fieldx = 0;
						}
						if (fieldx - mxpos + event.mouseMove.x < WINWIDTH - wx)
						{
							view.move(fieldx - WINWIDTH + wx, 0);
							fieldx = WINWIDTH - wx;
						}
					}
				}
				if (wy > WINHEIGHT)
				{
					if (fieldy - mypos + event.mouseMove.y <= 0 && fieldy - mypos + event.mouseMove.y >= WINHEIGHT - wy)
					{
						view.move(0, mypos - event.mouseMove.y);
						fieldy -= mypos - event.mouseMove.y;
						mypos = event.mouseMove.y;
					}
					else
					{
						if (fieldy - mypos + event.mouseMove.y > 0)
						{
							view.move(0, fieldy);
							fieldy = 0;
						}
						if (fieldy - mypos + event.mouseMove.y < WINHEIGHT - wy)
						{
							view.move(0, fieldy - WINHEIGHT + wy);
							fieldy = WINHEIGHT - wy;
						}
					}
				}
						
				ham.setPosition(-fieldx, -fieldy);
			}

			if (event.mouseMove.x > 0 && event.mouseMove.x < 63 && event.mouseMove.y > 0 && event.mouseMove.y < 63)
				ham.setColor(Color(255, 255, 255, 200));
			else
				ham.setColor(Color(255, 255, 255, 100));

			break;
		}

		case Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == Mouse::Middle || event.mouseButton.button == Mouse::Left && Keyboard::isKeyPressed(Keyboard::LAlt))
			{
				mxpos = event.mouseButton.x;
				mypos = event.mouseButton.y;
				middlepressed = true;
			}
			break;
		}

		case Event::MouseButtonReleased:
		{
			middlepressed = false;

			if (event.mouseButton.x > 0 && event.mouseButton.x < 63 && event.mouseButton.y > 0 && event.mouseButton.y < 63)
				ManualHint();

			if (event.mouseButton.button == Mouse::Middle)
				break;

			int xpos = event.mouseButton.x, ypos = event.mouseButton.y;

			/* Vertical blocks description handling. */
			if (ypos - fieldy < vindex * cellsize && xpos - fieldx > hindex * cellsize)
			{
				xpos = (xpos - fieldx) / cellsize - hindex;
				ypos = (ypos - fieldy) / cellsize;

				if (event.mouseButton.button == Mouse::Left && vertical[ypos][xpos] < height)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
					{
						vertical[ypos][xpos] += 5;
						if (vertical[ypos][xpos] > height)
							vertical[ypos][xpos] = height;
						vertext[ypos][xpos].setString(N2S(vertical[ypos][xpos]));
					}
					else
						vertext[ypos][xpos].setString(N2S(++vertical[ypos][xpos]));
				}

				if (event.mouseButton.button == Mouse::Right && vertical[ypos][xpos] > 0)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
					{
						vertical[ypos][xpos] -= 5;
						if (vertical[ypos][xpos] < 0)
							vertical[ypos][xpos] = 0;
						vertext[ypos][xpos].setString(N2S(vertical[ypos][xpos]));
					}
					else
						vertext[ypos][xpos].setString(N2S(--vertical[ypos][xpos]));
				}

				/* Align. */
				FloatRect textRect = vertext[ypos][xpos].getLocalBounds();
				int x = textRect.left + textRect.width / 2;
				int y = textRect.top + textRect.height / 2;
				vertext[ypos][xpos].setOrigin(x, y);
				vertext[ypos][xpos].setPosition((xpos + hindex - hstart) * cellsize + cellsize / 2, (ypos - vstart) * cellsize + cellsize / 2);
			}

			/* Horizontal blocks description handling. */
			if (ypos - fieldy > vindex * cellsize && xpos - fieldx < hindex * cellsize)
			{
				xpos = (xpos - fieldx) / cellsize;
				ypos = (ypos - fieldy) / cellsize - vindex;

				if (event.mouseButton.button == Mouse::Left && horizontal[ypos][xpos] < width)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
					{
						horizontal[ypos][xpos] += 5;
						if (horizontal[ypos][xpos] > width)
							horizontal[ypos][xpos] = width;
						hortext[ypos][xpos].setString(N2S(horizontal[ypos][xpos]));
					}
					else
						hortext[ypos][xpos].setString(N2S(++horizontal[ypos][xpos]));
				}

				if (event.mouseButton.button == Mouse::Right && horizontal[ypos][xpos] > 0)
				{
					if (Keyboard::isKeyPressed(Keyboard::LControl))
					{
						horizontal[ypos][xpos] -= 5;
						if (horizontal[ypos][xpos] < 0)
							horizontal[ypos][xpos] = 0;
						hortext[ypos][xpos].setString(N2S(horizontal[ypos][xpos]));
					}
					else
						hortext[ypos][xpos].setString(N2S(--horizontal[ypos][xpos]));
				}

				/* Align. */
				FloatRect textRect = hortext[ypos][xpos].getLocalBounds();
				int x = textRect.left + textRect.width / 2;
				int y = textRect.top + textRect.height / 2;
				hortext[ypos][xpos].setOrigin(x, y);
				hortext[ypos][xpos].setPosition((xpos - hstart) * cellsize + cellsize / 2, (ypos + vindex - vstart) * cellsize + cellsize / 2);
			}

			break;
		}

		case Event::KeyPressed:
		{
			if (event.key.code == Keyboard::S)
				SaveManualDescription();
		}
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

		window.draw(ham);
	}

	/* Solving window. */
	bool CreateMainWindow() 
	{
		int x, y;
		x = WINWIDTH > wx ? wx : WINWIDTH;
		y = WINHEIGHT > wy ? wy : WINHEIGHT;

		/* Create a window. */
		RenderWindow window(VideoMode(x, y), "Field2", Style::Close);
		View view;
		view.reset(FloatRect(0, 0, x, y));
		view.setViewport(FloatRect(0, 0, 1, 1));
		window.setView(view);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					window.close();
					return 1;
				}
				else
				{
					EventReaction(event, view);
					window.setView(view);
				}
			}

			Draw(window);

			window.display();
			
			if (solved && !win && answer == 'p')
			{
				WinHint();
				win = true;
			}

			if (answer == 's')
				Solve(false);

		}
		return 0;
	}

	/* Manual input window. */
	bool CreateManualWindow()
	{
		int x, y;
		x = WINWIDTH > wx ? wx : WINWIDTH;
		y = WINHEIGHT > wy ? wy : WINHEIGHT;
		RenderWindow window(VideoMode(x, y), "Field", Style::Close);
		View view;
		view.reset(FloatRect(0, 0, x, y));
		view.setViewport(FloatRect(0, 0, 1, 1));
		window.setView(view);

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::KeyPressed)
				{
					if (event.key.code == Keyboard::Return)
					{
						/* Reset fields changed by the previous solving. */
						{
							for (int i = 0; i < width; i++)
								vchange[i] = true;
							for (int i = 0; i < height; i++)
								hchange[i] = true;
							for (int i = 0; i < height; i++)
								for (int j = 0; j < width; j++)
								{
									cellarr[i][j].ChangeStateSolve(0);
									cellarr[i][j].black = false;
									cellarr[i][j].white = false;
								}
						}

						if (Solve(true) == 2)
						{
							ErrorHint();
							break;
						}

						bool incorr = false;
						while (!solved)
						{
							switch (Solve(false))
							{
							case 1:
								fieldx = fieldy = 0;
								ham.setPosition(0, 0);
								window.close();
								break;
							case 2:
								ErrorHint();
								incorr = true;
								for (int i = 0; i < height; i++)
									for (int j = 0; j < width; j++)
									{
										cellarr[i][j].ChangeStateSolve(0);
										cellarr[i][j].black = false;
										cellarr[i][j].white = false;
									}
								break;
							}
							if (incorr)
								break;
						}
					}
				}
				if (event.type == Event::Closed)
					return 1;
				else
				{
					ManEventReaction(event, view);
					window.setView(view);
				}
			}

			Draw(window);

			window.display();
		}
		
		return 0;
	}
};

#define MEDIUMSTART 3
#define HARDSTART 5

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

	MainMenu(int width, int height)
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

		window.draw(play_bg);
		window.draw(diff[0]);
		window.draw(diff[1]);
		window.draw(diff[2]);
		window.draw(lr_arrow[0]);
		window.draw(lr_arrow[1]);
		window.draw(start);
		window.draw(names);
	}

	void DrawSolve(RenderWindow &window)
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

	int PlayEventReaction(Event event)
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

				if (xpos > 800 && xpos < 840 && ypos > 360 - 60 && ypos < 410 - 60 && name < 6)
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

				if (xpos > 800 && xpos < 840 && ypos > 360 - 60 && ypos < 410 - 60 && name < 6)
				{
					names.setTextureRect(IntRect(++name * 290, 0, 290, 44));
					if (name == 6)
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
	
	int SolveEventReaction(Event event)
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

					if (xpos > 1137 && xpos < 1177 && ypos > 403 && ypos < 453 && name < 6)
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
					if (xpos > 1137 && xpos < 1177 && ypos > 403 && ypos < 453 && name < 6)
					{
						names.setTextureRect(IntRect(++name * 290, 0, 290, 44));
						if (name == 6)
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

	char CreateMenuWindow()
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
};

bool Launch()
{
	char ans;

	MainMenu Menu(MENUWIDTH, MENUHEIGHT);
	ans = Menu.CreateMenuWindow();
	if (ans == 'e')
		return 0;

	BWNonogram ManField(Menu.width, Menu.height);
	if (ans == 'm' && ManField.CreateManualWindow())
		return 1;

	BWNonogram Field(ans, Menu.name);
	if (ans == 'm')
		Field = ManField;	
	if (Field.CreateMainWindow())
		return 1;
}

int main()
{	
	while(Launch()){}
	return 0;
}
