#include "SupaSyC/Common.h"

static void PrintTokens(Token *);
static void PrintAst(AstNode *);

int main()
{
	File *file = OpenFile("sy/Main.sy", FILEMODE_READ);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Source of '%s':\n", file->Name);
	SetConsoleColor(ConsoleColor_DarkCyan);
	puts(file->Source);
	putchar('\n');

	SetConsoleColor(ConsoleColor_Yellow);
	printf("Tokens from '%s':\n", file->Name);
	Token *token = Lex(file);
	PrintTokens(token);

	SetConsoleColor(ConsoleColor_Yellow);
	printf("Ast from '%s':\n", file->Name);
	AstNode *ast = Parse(token);
	PrintAst(ast);

	int32_t eval = Evaluate(ast);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Evaluation: ");
	SetConsoleColor(ConsoleColor_Green);
	printf("%i\n", eval);

	CloseFile(file);
	getchar();
}

static void PrintTokens(Token *tok)
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

static void PrintAst(AstNode *ast)
{
	while (ast)
	{
		SetConsoleColor(ConsoleColor_Cyan);
		printf("%s", ast->Next ? "|--" : "L--");
		SetConsoleColor(ConsoleColor_Green);
		PrintAstNode(ast);

		ast = ast->Next;
	}
}