#pragma once

typedef enum
{
	ConsoleColor_Black,
	ConsoleColor_DarkBlue,
	ConsoleColor_DarkGreen,
	ConsoleColor_DarkCyan,
	ConsoleColor_DarkRed,
	ConsoleColor_DarkMagenta,
	ConsoleColor_DarkYellow,
	ConsoleColor_Grey,
	ConsoleColor_DarkGrey,
	ConsoleColor_Blue,
	ConsoleColor_Green,
	ConsoleColor_Cyan,
	ConsoleColor_Red,
	ConsoleColor_Magenta,
	ConsoleColor_Yellow,
	ConsoleColor_White,

	ConsoleColor_Reset = ConsoleColor_White,
} ConsoleColor;

void SetConsoleColor(ConsoleColor c);
ConsoleColor GetConsoleColor();
void ResetConsoleColor();