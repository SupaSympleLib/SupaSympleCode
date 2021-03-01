#include <stdint.h>

typedef int8_t int8, byte;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8, ubyte;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef struct
{
	uint32 Number;
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

File *OpenFile(const char *name, FileMode mode);
void CloseFile(const File *);

typedef struct
{
	const char *String;
	uint32 Length;
} StringView;

typedef enum
{
	TK_EndOfFile,

	TK_Identifier,
} TokenKind;

typedef struct Token Token;
struct Token
{
	const TokenKind Kind;
	const StringView Text;

	Token *Next;
};

Token *Lex(const char *file);