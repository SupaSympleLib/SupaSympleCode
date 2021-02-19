#pragma once

typedef struct
{
	uint32_t Number;
	const char *Name;
	char *Source;

	FILE *Stream;
} File;

typedef enum
{
	FILEMODE_READ,        // Read Only
	FILEMODE_WRITE,       // Write Only
	FILEMODE_APPEND,      // Append Only
	FILEMODE_READ_WRITE,  // Read and Write
	FILEMODE_READ_APPEND, // Read and Append
} FileMode;

File *sscOpenFile(const char *name, FileMode mode);
void sscCloseFile(const File *);