#include "SupaSyC/Common.h"

static void PrintTokens(const Token *);
static void PrintAst(const AstNode *, uint32_t indents);
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
	PrintAst(ast, 0);

	PrintEval(ast);

	DeleteAstNode(ast, true);
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

static void PrintAst(const AstNode *ast, uint32_t indents)
{
	while (ast)
	{
		SetConsoleColor(ConsoleColor_Cyan);
		for (uint32_t i = 0; i < indents; i++)
			printf("\t");
		printf("%s", ast->Next ? "|--" : "L--");
		SetConsoleColor(ConsoleColor_Green);
		PrintAstNode(ast);

		switch (ast->Kind)
		{
		case AST_Addition:
		case AST_Subtraction:
		case AST_Multiplication:
		case AST_Division:
		case AST_Modulo:
		{
			const AstNode *left = ast->Next;
			const AstNode *right = left->Next;

			printf("Left = "); PrintAst(left, indents + 1);
			printf("Right = "); PrintAst(right, indents + 1);

			ast = right->Next;
			break;
		}
		default:
			ast = ast->Next;
			break;
		}

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