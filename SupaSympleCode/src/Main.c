#include "SupaSyC/Common.h"

static void PrintTokens(const Token *);
static void PrintAst(const AstNode *);
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

	AstNode *ast = Parse(token);
	SetConsoleColor(ConsoleColor_Yellow);
	printf("Ast from '%s':\n", file->Name);
	PrintAst(ast);

	PrintEval(ast);

	CloseFile(file);
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

static void PrintAst(const AstNode *ast)
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