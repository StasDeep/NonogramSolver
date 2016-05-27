#pragma once

#include "BWCell.h"
#include "Funcs.h"

#define DELETE_2D_ARRAY(arr, height) for (int i = 0; i < height; ++i)\
	delete[] arr[i];\
	delete[] arr;
#define DELETE_1D_ARRAY(arr) delete[] arr;

using namespace sf;

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
	BWNonogram(char ans, int nonogname);

	/* Constructor for manual input. */
	BWNonogram(int x, int y);

	/* Destructor. */
	~BWNonogram();

	/* Create an array of cells and a grid with lines.. */
	void CreateField();

	/* Read blocks description from a file. */
	void ReadDescription();

	/* Walk through blocks, counting their amount and size. */
	void UpdateDescription(int xpos, int ypos);

	/* Show the description in console. */
	void ShowDescription();

	/* Writes current description to NewNonogram.txt. */
	void SaveDrawnDescription();

	/* Writes current description to NewNonogram.txt. */
	void SaveManualDescription();

	/* Check if the user's solving is correct. */
	bool CheckUser();

	/* The last check for correct solve. */
	bool LastCheck();

	/* Call main algorithm and check if the solving is correct. */
	int Solve(bool first);

	/* Check all horizontal lines. */
	void CheckHor();

	/* Check all vertical lines. */
	void CheckVert();

	/* Check each possible horizontal combination. */
	bool TryHorBlock(int theblock /* Number of the block in horizontal array. */, int thestart /* Position check starts with. */, int line);

	/* Check each possible vertical combination. */
	bool TryVertBlock(int theblock, int thestart, int col);

	/* Window showing main shortcuts for Play/Solve mode. */
	void PlaySolveHint();

	/* Window showing main shortcuts for Manual input mode. */
	void ManualHint();

	/* Window showing error. */
	void ErrorHint();

	/* Window showing congratulations. */
	void WinHint();

	/* Event switch case. */
	void EventReaction(Event event, View &view);

	/* Event for manual mode. */
	void ManEventReaction(Event event, View &view);

	/* Draw field, grid and description. */
	void Draw(RenderWindow &window);

	/* Solving window. */
	bool CreateMainWindow();

	/* Manual input window. */
	bool CreateManualWindow();
};
