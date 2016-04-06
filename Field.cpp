/*
 * Field.
 * Constructor added.
 */

#include "stdafx.h"
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp> 
#include <SFML\Window.hpp>

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

enum color
{
	PWHITE, /* 0 Probably White. */
	DBLACK, /* 1 Definitely Black. */
	DWHITE  /* 2 Definitely White. */
};

using namespace sf;

class Cell
{
public:
	Sprite cellsprite;
	color state;
	bool black;
	bool white;

	void ChangeStateClick(int butt)
	{
		switch (butt)
		{
		case 0:
		{
			cellsprite.setTextureRect(IntRect(32, 0, 32, 32));
			state = DBLACK;
			break;
		}

		case 1:
		{
			if (state == 0 || state == 1)
			{
				cellsprite.setTextureRect(IntRect(64, 0, 32, 32));
				state = DWHITE;
			}
			else
			{
				cellsprite.setTextureRect(IntRect(0, 0, 32, 32));
				state = PWHITE;
			}
			break;
		}

		case 2:
		{
			cellsprite.setTextureRect(IntRect(0, 0, 32, 32));
			state = PWHITE;
			break;
		}

		default:
			break;

		}
	}

	void ChangeStateSolve(color newstate)
	{
		switch (newstate)
		{
		case 0:
			cellsprite.setTextureRect(IntRect(0, 0, 32, 32));
			break;

		case 1:
			cellsprite.setTextureRect(IntRect(32, 0, 32, 32));
			break;

		case 2:
			cellsprite.setTextureRect(IntRect(64, 0, 32, 32));
			break;

		default:
			break;
		}
		state = newstate;
	}
};

class Nonogram
{
public:
	int width;				/* Width of the field, in cells. */
	int height;				/* Height of the field, in cells. */
	int cellsize;			/* Size of the cell, in pixels. */
	int hindex;				/* Maximum amount of horizontal blocks. */
	int vindex;				/* Maximum amount of vertical blocks. */
	Cell **cellarr;			/* Array of cells. */
	int **horizontal;		/* Description of horizontal blocks. */
	int **vertical;			/* Description of vertical blocks. */
	int hstart;				/* Amount of empty columns in the desctription. */
	int vstart;				/* Amount of empty lines in the desctription. */
	Texture celltex;		/* Texture with an image, containing all states. */
	RectangleShape *hline;	/* Array of horizontal lines for grid. */
	RectangleShape *vline;	/* Array of vertical lines for grid. */

	/* Constructor. */
	Nonogram(char answer, int size = 16, int x = 15, int y = 15 )
	{
		if (answer == 's')
			ReadDescription();
		else
		{
			width = x;
			height = y;
			CreateDescription();
		}

		cellsize = size;
		CreateField();
		SetCells();
		CreateGrid();


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

	/* Create a grid with lines.*/
	void CreateGrid()
	{
		/* Horizontal lines. */
		hline = new RectangleShape[height + 1];
		for (int i = 0; i < height + 1; i++)
		{
			hline[i].setFillColor(Color(24, 24, 24, 200));
			if (i % 5 == 0)
				hline[i].setSize(Vector2f(width * cellsize, 3));
			else
				hline[i].setSize(Vector2f(width * cellsize, 2));
			hline[i].setPosition(0, i *  cellsize - 1);
		}

		/* Vertical lines. */
		vline = new RectangleShape[width + 1];
		for (int i = 0; i < width + 1; i++)
		{
			vline[i].setFillColor(Color(24, 24, 24, 200));
			if (i % 5 == 0)
				vline[i].setSize(Vector2f(3, height * cellsize));
			else
				vline[i].setSize(Vector2f(2, height * cellsize));
			vline[i].setPosition(i *  cellsize - 1, 0);
		}
	}

	/* Create an array of cells. */
	void CreateField()
	{
		cellarr = new Cell *[height];
		for (int i = 0; i < height; i++)
			cellarr[i] = new Cell[width];
	}

	/* Initialize array with the description of lines and columns. */
	void CreateDescription()
	{
		hindex = (width + 1) / 2;
		vindex = (height + 1) / 2;
		horizontal = CreateArr(height, hindex);
		vertical = CreateArr(vindex, width);
	}

	/* Reset the description of the line and the column. */
	void ResetDescription(int xpos, int ypos)
	{
		for (int j = 0; j < hindex; j++)
			horizontal[ypos][j] = 0;
		for (int i = 0; i < vindex; i++)
			vertical[i][xpos] = 0;
	}

	/* Walk through blocks, counting their amount and size. */
	void UpdateDescription(int xpos, int ypos)
	{
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
					horizontal[ypos][hindex - blcount] = blsize;
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
					vertical[vindex - blcount][xpos] = blsize;
					blsize = 0;
				}
			}
		}
	}

	/* Count the amount of empty lines and columns in the description. */
	void CountEmptyDescription()
	{
		vstart = 0;
		hstart = 0;

		/* Count the amount of empty lines in vertical blocks description. */
		for (int i = 0; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (vertical[i][j] != 0)
					break;
				else
					if (j == width - 1)
						vstart++;
			}
			if (vstart != i + 1)
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
						hstart++;
			}
			if (hstart != j + 1)
				break;
		}
	}

	/* Show the description in console. */
	void ShowDescription()
	{
		system("cls");
		/* Show vertical blocks. */
		for (int i = vstart; i < vindex; i++)
		{
			for (int j = 0; j < width; j++)
			{
				std::cout << vertical[i][j] << " ";
			}
			std::cout << "\n";
		}
		std::cout << "-\n";
		/* Show horizontal blocks. */
		for (int i = 0; i < height; i++)
		{
			for (int j = hstart; j < hindex; j++)
			{
				std::cout << horizontal[i][j] << " ";
			}
			std::cout << "\n";
		}
	}

	/* Set cells' own position, texture and state. */
	void SetCells()
	{
		celltex.loadFromFile("images/cells.png");
		celltex.setSmooth(true);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				cellarr[i][j].cellsprite.setPosition(j * cellsize, i * cellsize);
				cellarr[i][j].cellsprite.setTexture(celltex);
				cellarr[i][j].cellsprite.scale((float)cellsize / 32, (float)cellsize / 32);
				cellarr[i][j].ChangeStateClick(2);
				cellarr[i][j].black = false;
				cellarr[i][j].white = false;
			}
		}
	}

	/* Read description from a file. */
	void ReadDescription()
	{
#pragma warning (disable : 4996)
		FILE *Descr;
		Descr = fopen("Nonogram30x42.txt", "r");
		fscanf(Descr, "%d", &width);
		fscanf(Descr, "%d", &height);
		fscanf(Descr, "%d", &vstart);
		fscanf(Descr, "%d", &hstart);
		hindex = (width + 1) / 2;
		vindex = (height + 1) / 2;
		horizontal = CreateArr(height, hindex);
		vertical = CreateArr(vindex, width);

		for (int i = vindex - vstart; i < vindex; i++)
			for (int j = 0; j < width; j++)
				fscanf(Descr, "%d ", &vertical[i][j]);

		for (int i = 0; i < height; i++)
			for (int j = hindex - hstart; j < hindex; j++)
				fscanf(Descr, "%d ", &horizontal[i][j]);

		fclose(Descr);
	}

	/* Check all horizontal lines. */
	void CheckHor()
	{
		int sum;
		int nonzero;
		for (int j = 0; j < height; j++)
		{
			/* Reset flags. */
			for (int i = 0; i < width; i++)
			{
				cellarr[j][i].white = false;
				cellarr[j][i].black = false;
			}

			/* Sum is a minimum amount of cells, used for the blocks and gaps between. */
			/* Nonzero is a number of the block, counting starts from. */
			sum = 0;
			nonzero = -1;
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

			/* Check if it is possible to place the blocks. */
			for (int i = 0; i < width - sum + 2; i++)
			{
				TryHorBlock(nonzero, i, j);
			}
			if (width <= sum - 2)
				std::cout << "INCORR INPUT.\n";

			/* Main check. */
			for (int i = 0; i < width; i++)
			{
				if (cellarr[j][i].white ^ cellarr[j][i].black)
				{
					if (cellarr[j][i].white)
					{
						cellarr[j][i].ChangeStateSolve(DWHITE);
					}
					if (cellarr[j][i].black)
					{
						cellarr[j][i].ChangeStateSolve(DBLACK);
					}
				}
			}
		}
	}

	/* Check all vertical lines. */
	void CheckVert()
	{
		int sum;
		int nonzero;

		for (int j = 0; j < width; j++)
		{
			/* Reset flags. */
			for (int i = 0; i < height; i++)
			{
				cellarr[i][j].white = false;
				cellarr[i][j].black = false;
			}

			/* Sum is a minimum amount of cells, used for the blocks and gaps between. */
			/* Nonzero is a number of the block, counting starts from.*/
			sum = 0;
			nonzero = -1;
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

			/* Check if it is possible to place the blocks. */
			for (int i = 0; i < height - sum + 2; i++)
			{
				TryVertBlock(nonzero, i, j);
			}
			if (height <= sum - 2)
				std::cout << "INCORR INPUT.\n";

			/* Main check. */
			for (int i = 0; i < height; i++)
			{
				if (cellarr[i][j].white ^ cellarr[i][j].black)
				{
					if (cellarr[i][j].white)
					{
						cellarr[i][j].ChangeStateSolve(DWHITE);
					}
					if (cellarr[i][j].black)
					{
						cellarr[i][j].ChangeStateSolve(DBLACK);
					}
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

};


int main()
{
	int xpos;
	int ypos;
	char answer = 'p';	
	bool click = true;		/* If true then click will call CheckHor. If false - CheckVert. */
	bool solved = false;

	/* Constructing the field. */
	Nonogram Field(answer);	

	/* Create a window. */
	RenderWindow window(VideoMode(Field.width*Field.cellsize, Field.height*Field.cellsize), "Field", Style::Close);

	/*
	* Main cycle.
	* Works until the window is closed.
	*/
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;

			case Event::MouseButtonPressed:
				if (answer == 'p')
				{
					xpos = Mouse::getPosition(window).x / Field.cellsize;
					ypos = Mouse::getPosition(window).y / Field.cellsize;

					/*
					 * As the "button" is enumeration,
					 * 0 is sent if Left,
					 * 1 is sent if Right,
					 * 2 if Middle button is pressed.
					 */
					Field.cellarr[ypos][xpos].ChangeStateClick(event.mouseButton.button);

					Field.ResetDescription(xpos, ypos);
					Field.UpdateDescription(xpos, ypos);
					Field.CountEmptyDescription();
					Field.ShowDescription();
				}

				break;

			case Event::KeyPressed:
				solved = true;
				break;

			default:
				break;
			}

		} /* Event cycle end. */

		window.clear();

		/* Draw the field. */
		for (int i = 0; i < Field.height; i++)
		{
			for (int j = 0; j < Field.width; j++)
			{
				window.draw(Field.cellarr[i][j].cellsprite);
			}
		}

		if (!solved)
		{
			for (int i = 0; i < Field.height + 1; i++)
				window.draw(Field.hline[i]);
			for (int i = 0; i < Field.width + 1; i++)
				window.draw(Field.vline[i]);
		}

		window.display();

		if (answer == 's' && solved == false)
		{
			Field.CheckHor();
			Field.CheckVert();
		}

	}

	return 0;
}
