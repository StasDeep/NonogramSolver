#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp> 
#include <SFML\Window.hpp>

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
	int ChangeStateClick(int butt);

	/* Used when dragged or while solving.*/
	void ChangeStateSolve(int newstate);

};