#include "SupaSyC/Common.h"

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	puts(file->Source);
	getchar();
}