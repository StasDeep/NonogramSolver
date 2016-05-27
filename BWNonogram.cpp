#include "stdafx.h"
#include "BWNonogram.h"

/* Constructor. */
BWNonogram::BWNonogram(char ans, int nonogname)
{
	char numb[3];
	sprintf_s(numb, "%d", nonogname);
	strcpy_s(name, 45, "Nonograms/");
	strcat_s(name, numb);
	strcat_s(name, ".nng");

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

	/*if (sum != 0)
	{
	ErrorHint();
	std::cout << "Incorrect input.";
	getchar();
	}*/

}

/* Constructor for manual input. */
BWNonogram::BWNonogram(int x, int y)
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
BWNonogram::~BWNonogram()
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
}

/* Create an array of cells and a grid with lines.. */
void BWNonogram::CreateField()
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
void BWNonogram::ReadDescription()
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
void BWNonogram::UpdateDescription(int xpos, int ypos)
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
void BWNonogram::ShowDescription()
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
void BWNonogram::SaveDrawnDescription()
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
void BWNonogram::SaveManualDescription()
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
bool BWNonogram::CheckUser()
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
bool BWNonogram::LastCheck()
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
int BWNonogram::Solve(bool first)
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
void BWNonogram::CheckHor()
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
void BWNonogram::CheckVert()
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
bool BWNonogram::TryHorBlock(int theblock /* Number of the block in horizontal array. */, int thestart /* Position check starts with. */, int line)
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

				if ((theblock != 0 && horizontal[line][theblock - 1] == 0) || theblock == 0)
					for (int i = 0; i < thestart; i++)
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

		if ((theblock != 0 && horizontal[line][theblock - 1] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				cellarr[line][i].white = true;

		for (int j = thestart; j < thestart + horizontal[line][theblock]; j++)
			cellarr[line][j].black = true;
		for (int j = thestart + horizontal[line][theblock]; j < width; j++)
			cellarr[line][j].white = true;

		return true;

	}

}

/* Check each possible vertical combination. */
bool BWNonogram::TryVertBlock(int theblock, int thestart, int col)
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

				if ((theblock != 0 && vertical[theblock - 1][col] == 0) || theblock == 0)
					for (int i = 0; i < thestart; i++)
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

		if ((theblock != 0 && vertical[theblock - 1][col] == 0) || theblock == 0)
			for (int i = 0; i < thestart; i++)
				cellarr[i][col].white = true;

		for (int j = thestart; j < thestart + vertical[theblock][col]; j++)
			cellarr[j][col].black = true;
		for (int j = thestart + vertical[theblock][col]; j < height; j++)
			cellarr[j][col].white = true;

		return true;

	}

}

/* Window showing main shortcuts for Play/Solve mode. */
void BWNonogram::PlaySolveHint()
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
void BWNonogram::ManualHint()
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
void BWNonogram::ErrorHint()
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
void BWNonogram::WinHint()
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
void BWNonogram::EventReaction(Event event, View &view)
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

			/* If thå cell hasn't been changed it is changed to remembered state. */
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
void BWNonogram::ManEventReaction(Event event, View &view)
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
void BWNonogram::Draw(RenderWindow &window)
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
bool BWNonogram::CreateMainWindow()
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
			if (event.key.code == Keyboard::Escape)
				return 1;
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
bool BWNonogram::CreateManualWindow()
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
				if (event.key.code == Keyboard::Escape)
					return 1;
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