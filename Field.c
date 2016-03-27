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
		PWHITE,
		DBLACK,
		DWHITE
	} state;

	void CellPos(int x, int y)
	{	
		cellsprite.setPosition(x * 32, y * 32);
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
	const int width = 25;
	const int height = 25;

	RenderWindow window(sf::VideoMode(width*32, height*32), "Field", Style::Close);	
	Texture celltex;
	celltex.loadFromFile("images/cells.jpg"); 

	/* Create sprite and set its position, texture and state. */
	Cell OneCell[width][height];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
		OneCell[i][j].CellPos(i, j);
		OneCell[i][j].cellsprite.setTexture(celltex);
		OneCell[i][j].ChangeState(2);
		}
	}
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
				OneCell[xpos][ypos].ChangeState(event.mouseButton.button);
				break;
			}

			default:
				break;

			}

		}

		window.clear();		
		
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				window.draw(OneCell[i][j].cellsprite);
			}
		}
		
		
		window.display();
	}

	return 0;
}
