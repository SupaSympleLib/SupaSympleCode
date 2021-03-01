#include "SupaSyC/Common.h"

static void PrintTokens(const Token *);
static void PrintAst(const AstObject *);
static void PrintEval(const AstNode *);

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	putchar('\n');

	Token *token = Lex(file);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Tokens from '%s':\n", file->Name);
	PrintTokens(token);

	AstObject *ast = Parse(token);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Ast from '%s':\n", file->Name);
	PrintAst(ast);

	//PrintEval(ast);

	File *out = OpenFile("sy/Main.S", FILEMODE_READ_WRITE);
	Emit(out, ast);

	CloseFile(out);
	CloseFile(file);

	SetConsoleColor(ConsoleColor_Reset);
	int comp = system("clang -m32 sy/Main.S -o sy/Main.exe --debug -l legacy_stdio_definitions");
	if (!comp)
	{
		SetConsoleColor(ConsoleColor_Yellow);
		puts("Starting program...");
		SetConsoleColor(ConsoleColor_Reset);
		int ec = system("sy\\Main.exe");
		printf("Value: %f\n", *(float*)&ec);
		SetConsoleColor(ConsoleColor_Yellow);
		printf("\nProgram exited with code %i (0x%x)\n", ec, ec);
	}

	//puts("Compiled Succesfully!");
	getchar();
}

static void PrintTokens(const Token *tok)
{
	while (tok)
	{
		SetConsoleColor(ConsoleColor_Cyan);
		printf("%s", tok->Next ? "|--" : "L--");
		SetConsoleColor(ConsoleColor_Green);
		PrintToken(tok);

		tok = tok->Next;
	}
}

static void PrintAst(const AstObject *obj)
{
	while (obj)
	{
		SetConsoleColor(ConsoleColor_Cyan);
		printf("%s", obj->Next ? "|--" : "L--");
		SetConsoleColor(ConsoleColor_Green);
		PrintAstObject(obj);

		obj = obj->Next;
	}
}

static void PrintEval(const AstNode *ast)
{
	while (ast)
	{
		uint32_t ln = ast->Token->DisplayLine;
		int32_t eval = Evaluate(ast, &ast);
		SetConsoleColor(ConsoleColor_Yellow);
		printf("Evaluation (Line %u): ", ln);
		SetConsoleColor(ConsoleColor_Green);
		printf("%i\n", eval);

		if (!ast->Next)
			break;
	}
}