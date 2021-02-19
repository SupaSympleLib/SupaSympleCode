#include "SupaSyC/Common.h"

static uint32_t sNumFiles;

File *OpenFile(const char *name, FileMode mode)
{
	File *file = Alloc(1, File);
	file->Number = sNumFiles;
	file->Name = name;
	sNumFiles++;

	bool canRead;
	const char *strMode;
	switch (mode)
	{
	case FILEMODE_READ:
		strMode = "rb";
		canRead = true;
		break;
	case FILEMODE_WRITE:
		strMode = "wb";
		canRead = false;
		break;
	case FILEMODE_APPEND:
		strMode = "ab";
		canRead = false;
		break;
	case FILEMODE_READ_WRITE:
		strMode = "wb+";
		canRead = true;
		break;
	case FILEMODE_READ_APPEND:
		strMode = "ab+";
		canRead = true;
		break;
	default:
		strMode = "";
		canRead = false;
		break;
	}

	if (canRead)
	{
		file->Stream = fopen(name, strMode);
		fseek(file->Stream, 0, SEEK_END);
		uint32_t buffSz = ftell(file->Stream) + 1;
		file->Source = Alloc(buffSz, char);
	}
	else
		file->Source = "";
}

void CloseFile(const File *file)
{
	fclose(file->Stream);
	Free(file);
}