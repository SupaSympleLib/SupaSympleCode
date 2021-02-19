#include "SupaSyC/Common.h"

int main()
{
	File *file = sscOpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Green);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	sscCloseFile(file);
	getchar();
}