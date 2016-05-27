#include "stdafx.h"
#include "BWCell.h"

int BWCell::ChangeStateClick(int butt)
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
void BWCell::ChangeStateSolve(int newstate)
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