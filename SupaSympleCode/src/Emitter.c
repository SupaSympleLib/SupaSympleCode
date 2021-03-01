#include "SupaSyC/Common.h"

typedef struct
{
	File *file;
	const AstObject *ast;
	const AstNode *node;
} Emitter;

#define Emitf(this, fmt, ...) (void)fprintf(this->file->Stream, fmt "\n", ##__VA_ARGS__)

static void EmitObj(Emitter *);
static void EmitFunc(Emitter *);

static void EmitStmt(Emitter *);
static void EmitBlockStmt(Emitter *);

static void EmitExpr(Emitter *);
static void EmitBinExpr(Emitter *, const char *op);

static const AstNode *Next(Emitter *);

void Emit(File *file, const AstObject *ast)
{
	Emitter *this = &(Emitter) { file, ast };

	while (this->ast)
		EmitObj(this);
}

static void EmitObj(Emitter *this)
{
	if (this->ast->IsFunction)
		EmitFunc(this);

	this->ast = this->ast->Next;
}

static void EmitFunc(Emitter *this)
{
	const Token *name = this->ast->Token;

	Emitf(this, ".global  %.*s", name->Length, name->Text);
	Emitf(this, "%.*s:", name->Length, name->Text);

	const AstNode *body = this->ast->Body;
	assert(body);
	this->node = body;
	EmitStmt(this);

	Emitf(this, "\tret");
}

static void EmitStmt(Emitter *this)
{
	if (AstIsExpr(this->node))
		EmitExpr(this);

	switch (this->node->Kind)
	{
	case AST_Block:
		EmitBlockStmt(this);
		break;
	}
}

static void EmitBlockStmt(Emitter *this)
{
	Next(this);

	while (this->node->Kind != AST_EndBlock)
		EmitStmt(this);

	Next(this);
}

static void EmitExpr(Emitter *this)
{
	switch (this->node->Kind)
	{
	case AST_Number:
	{
		const Token *num = Next(this)->Token;
		Emitf(this, "\tmov     $%.*s, %%eax", num->Length, num->Text);
		break;
	}
	case AST_Addition:
		EmitBinExpr(this, "add");
		break;
	case AST_Subtraction:
		EmitBinExpr(this, "sub");
		break;
	case AST_Multiplication:
		EmitBinExpr(this, "imul");
		break;
	case AST_Division:
		Next(this);

		EmitExpr(this);
		Emitf(this, "\tpush    %%eax");
		EmitExpr(this);
		Emitf(this, "\tpop     %%ecx");
		Emitf(this, "\tcltd");
		Emitf(this, "\tidiv    %%ecx");
		Emitf(this);
		break;
	case AST_Modulo:
		Next(this);

		EmitExpr(this);
		Emitf(this, "\tpush    %%eax");
		EmitExpr(this);
		Emitf(this, "\tpop     %%ecx");
		Emitf(this, "\tcltd");
		Emitf(this, "\tidiv    %%ecx");
		Emitf(this, "\tmov     %%edx, %%eax");
		Emitf(this);
		break;
	}
}

static void EmitBinExpr(Emitter *this, const char *op)
{
	Next(this);

	EmitExpr(this);
	Emitf(this, "\tpush    %%eax");
	EmitExpr(this);
	Emitf(this, "\tpop     %%edx");
	Emitf(this, "\t%-7s %%edx, %%eax", op);
	Emitf(this);
}

static const AstNode *Next(Emitter *this)
{
	const AstNode *node = this->node;
	if (this->node->Next)
		this->node = this->node->Next;

	return node;
}