#include "stdafx.h"
#include "Launch.h"

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