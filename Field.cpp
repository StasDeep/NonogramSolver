/*
 * Field.
 * Simple solving algo added.
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
	
	void ChangeState(int butt)
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
	int width;			/* Width of the field, in cells. */
	int height;			/* Height of the field, in cells. */
	int cellsize;		/* Size of the cell, in pixels. */
	int hindex;			/* Maximum amount of horizontal blocks. */
	int vindex;			/* Maximum amount of vertical blocks. */
	Cell **cellarr;		/* Array of cells. */
	int **horizontal;	/* Description of horizontal blocks. */
	int **vertical;		/* Description of vertical blocks. */
	int hstart;			/* Amount of empty columns in the desctription. */
	int vstart;			/* Amount of empty lines in the desctription. */
	Texture celltex;	/* Texture with an image, containing all states. */

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
		celltex.loadFromFile("images/cells.jpg");
		celltex.setSmooth(true);
		
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				cellarr[i][j].cellsprite.setPosition(j * cellsize, i * cellsize);
				cellarr[i][j].cellsprite.setTexture(celltex);
				cellarr[i][j].cellsprite.scale((float) cellsize / 32, (float) cellsize / 32);
				cellarr[i][j].ChangeState(2);
			}
		}
	}

	bool TryBlock(int theblock /*номер блока в массиве*/, int thestart /*позиция, с которой начинается проверка*/)
	{
		bool result;

		/* проверить, возможно ли поместить данный блок на данную позицию. */
		for (int i = thestart; i < thestart + horizontal[0][theblock]; i++)		
			if (cellarr[0][i].state == DWHITE)
				return false;

		if (horizontal[0][theblock - 1] == 0)
			for (int i = 0; i < thestart; i++)
				cellarr[0][i].white = true;

		/* обработка случая, когда блок не является последним в ряду. */
		if (theblock < hindex - 1)
		{
			result = false;

			/* цикл начинается с самой левой позиции, соответствующей положению следующего блока, т.е. на 2 клетки правее конца текущего блока. */
			/* цикл заканчивается на последней позиции, начиная с которой можно вставить следующий блок. */
			for (int startnext = thestart + horizontal[0][theblock] + 1; startnext < width - horizontal[0][theblock + 1] + 1; startnext++)
			{
				/* небольшая оптимизация. */
				/* если клетка, которая должна быть промежутком, оказывается черной, нет смысла продолжать перебор для этого расположения текущего блока.*/
				if (cellarr[0][startnext].state == DBLACK)
					break;

				/* проверить расположение следующего блока для позиции startnext. */
				if (TryBlock(theblock + 1, startnext))
				{
					for (int i = thestart; i < thestart + horizontal[0][theblock]; i++)
						cellarr[0][i].black = true;
					for (int i = thestart + horizontal[0][theblock]; i < startnext; i++)
						cellarr[0][i].white = true;

					result = true;
				}
			}

			return result;
		
		}
		else /* текущий блок - последний в ряду. */
		{
			for (int i = thestart + horizontal[0][theblock]; i < width + 1; i++)
			{
				if (cellarr[0][i].state == DBLACK)
					return false;

				for (int j = thestart; j < thestart + horizontal[0][theblock]; j++)
					cellarr[0][j].black = true;
				for (int j = thestart + horizontal[0][theblock]; j < width; j++)
					cellarr[0][j].white = true;
			}

			return true;

		}

	}

};


int main()
{
	/*
	 * X and Y coordinates of the mouse click 
	 * relatively to the window.
	 */
	int xpos;	
	int ypos;

	int sum;

	Nonogram Field;
	Field.width = 8;
	Field.height = 15;
	Field.cellsize = 32;
	Field.CreateField();
	Field.SetCells();
	Field.CreateDescription();

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
				xpos = Mouse::getPosition(window).x / Field.cellsize;
				ypos = Mouse::getPosition(window).y / Field.cellsize;

				/* 
				 * As the "button" is enumeration, 
				 * 0 is sent if Left, 
				 * 1 is sent if Right, 
				 * 2 if Middle button is pressed. 
				 */
				/*Field.cellarr[ypos][xpos].ChangeState(event.mouseButton.button);	

				Field.ResetDescription(xpos, ypos);
				Field.UpdateDescription(xpos, ypos);
				Field.CountEmptyDescription();
				Field.ShowDescription();	*/

				/* обнулить значения флагов. */
				for (int i = 0; i < Field.width; i++)
				{
					Field.cellarr[0][i].black = false;
					Field.cellarr[0][i].white = false;
				}
				/* описание блоков (для проверки). */
				Field.horizontal[0][2] = 3;
				Field.horizontal[0][3] = 3;
				

				/* минимальное количество клеток, которые занимаются блоками и промежутками между ними. */
				sum = 0;
				for (int i = 0; i < Field.hindex; i++)
				{
					sum += Field.horizontal[0][i];
					if (Field.horizontal[0][i] != 0)
						sum++;
				}

				/* проверочки-проверочки. */
				for (int i = 0; i < Field.width - sum + 2; i++)
				{
					if (!Field.TryBlock(2, i))
						std::cout << "Cannot build.\n";
				}
				if (Field.width == sum - 2)
					std::cout << "INCORR INPUT.\n";

				/* проверка на совпадение значения в клетке во всех случаях. */
				for (int i = 0; i < Field.width; i++)
				{
					std::cout << i << ". ";
					if (Field.cellarr[0][i].white ^ Field.cellarr[0][i].black)
					{
						if (Field.cellarr[0][i].white)
						{
							Field.cellarr[0][i].ChangeStateSolve(DWHITE);
							std::cout << "White.\n";

						}
						if (Field.cellarr[0][i].black)
						{
							Field.cellarr[0][i].ChangeStateSolve(DBLACK);
							std::cout << "Black.\n";
						}
					}
					else
					{
						std::cout << "Undef.\n";
					}
				}

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
		
		window.display();
	}		

	return 0;
}
