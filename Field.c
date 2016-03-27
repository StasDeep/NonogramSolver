/*
 * Field with Vertical and Horizontal Counter of Blocks.
 * Console output without useless zero lines and columns.
 * Unnecessary grid array is removed.
 */

#include "stdafx.h"
#include <iostream>
#include <SFML/Graphics.hpp>

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
	const int width = 15;
	const int height = 15;

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
				
				/* As the "button" is enumeration, we send 0 if Left and 1 if Right. */
				OneCell[ypos][xpos].ChangeState(event.mouseButton.button);

				/* Change the description of the line and the column.*/
				{
					/* Reset the description of the line and the column. */
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
					}

					/* Walk through blocks, counting their amount and size. */
					{
						int blcount = 0; /* Amount of blocks in a line/column. */
						int blsize = 0; /* Amount of black cell in a block. */

						/* Walk through horizontal blocks. */
						for (int j = width - 1; j >= 0; j--)
						{
							if (OneCell[ypos][j].state == DBLACK)
							{
								blsize++;
								if (OneCell[ypos][j - 1].state == PWHITE || OneCell[ypos][j - 1].state == DWHITE || j == 0)
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
							if (OneCell[i][xpos].state == DBLACK)
							{
								blsize++;
								if (OneCell[i - 1][xpos].state == PWHITE || OneCell[i - 1][xpos].state == DWHITE || i == 0)
								{
									blcount++;
									vertical[vindex - blcount][xpos] = blsize;
									blsize = 0;
								}
							}
						}
					}
					
					/* Count the amount of empty lines in vertical blocks description. */
					int vstart = 0;
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
					int hstart = 0;
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
					
					/* Show blocks. */
					{
						system("cls");
						/* Show vertical blocks. */
						for (int i = vstart; i < vindex; i++)
						{
							for (int j = 0; j < width; j++)
							{
								std::cout << vertical[i][j];
								std::cout << " ";
							}
							std::cout << "\n";
						}
						std::cout << "-\n";
						/* Show horizontal blocks. */
						for (int i = 0; i < height; i++)
						{
							for (int j = hstart; j < hindex; j++)
							{
								std::cout << horizontal[i][j];
								std::cout << " ";
							}
							std::cout << "\n";
						}
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
