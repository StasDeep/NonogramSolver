#include "stdafx.h"
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

class Cell
{
public:
	Sprite cellsprite;
	enum color
	{
		PWHITE, /* 0 Probably White. */
		DBLACK, /* 1 Definitely Black. */
		DWHITE  /* 2 Definitely White. */
	} state;

	void CellPos(int x, int y)
	{	
		cellsprite.setPosition(x, y);
	}
	
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
};

int main()
{
	const int width = 5;
	const int height = 5;

	/* Create a window. */
	RenderWindow window(sf::VideoMode(width*32, height*32), "Field", Style::Close);	
	
	/* Create a texture for sprite. */
	Texture celltex;
	celltex.loadFromFile("images/cells.jpg"); 

	/* Create a grid of cells and set their own position, texture and state. */
	Cell OneCell[height][width];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
		OneCell[i][j].CellPos(j * 32, i * 32);
		OneCell[i][j].cellsprite.setTexture(celltex);
		OneCell[i][j].ChangeState(2);
		}
	}

	
	/* Create an array of 0 and 1, where 0 is white and 1 is black.*/
	int grid[height][width];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			grid[i][j] = 0;

	/* Create an array with nonogram description of horizontal blocks.*/
	const int hindex = (width + 1) / 2;
	int horizontal[height][hindex];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < hindex; j++)
			horizontal[i][j] = 0;

	/* Create an array with nonogram description of vertical blocks.*/
	const int vindex = (height + 1) / 2;
	int vertical[vindex][width];
	for (int i = 0; i < vindex; i++)
		for (int j = 0; j < width; j++)
			vertical[i][j] = 0;
	


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
			{
				int xpos = Mouse::getPosition(window).x / 32;
				int ypos = Mouse::getPosition(window).y / 32;

				/* Change the numbergrid cell value. */
				if (event.mouseButton.button == 0)
					grid[ypos][xpos] = 1;
				if (event.mouseButton.button == 1 || event.mouseButton.button == 2)
					grid[ypos][xpos] = 0;

				/* As the "button" is enumeration, we send 0 if Left and 1 if Right. */
				OneCell[ypos][xpos].ChangeState(event.mouseButton.button);

				/* Change the description of the line and the column.*/
				{
					/* Reset the line description. */
					for (int j = 0; j < hindex; j++)
					{
						horizontal[ypos][j] = 0;
					}

					/* Reset the column description. */
					for (int i = 0; i < vindex; i++)
					{
						vertical[i][xpos] = 0;
					}

					int blcount = 0; /* Amount of blocks in a line/column. */
					int blsize = 0; /* Amount of black cell in a block. */

					/* Walk through horizontal blocks. */
					for (int j = width - 1; j >= 0; j--)
					{
						if (grid[ypos][j] == 1)
						{
							blsize++;
							if (grid[ypos][j - 1] == 0 || j == 0)
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
						if (grid[i][xpos] == 1)
						{
							blsize++;
							if (grid[i - 1][xpos] == 0 || i == 0)
							{
								blcount++;
								vertical[vindex - blcount][xpos] = blsize;
								blsize = 0;
							}
						}
					}

					system("cls");	
					/* Vertical blocks. */
					for (int i = 0; i < vindex; i++)
					{
						for (int j = 0; j < width; j++)
						{
							std::cout << vertical[i][j];
							std::cout << " ";
						}
						std::cout << "\n";
					}
					std::cout << "-\n";
					/* Horizontal blocks. */
					for (int i = 0; i < height; i++)
					{
						for (int j = 0; j < hindex; j++)
						{
							std::cout << horizontal[i][j];
							std::cout << " ";
						}
						std::cout << "\n";
					}							
				}
				break;
			}
			
			default:
				break;
			} /* Switch end. */
		} /* Event cycle end. */

		window.clear();		
		
		/* Draw the field. */
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				window.draw(OneCell[i][j].cellsprite);
			}
		}		
		
		window.display();
	}		

	return 0;
}
