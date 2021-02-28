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

static void EmitExpr(Emitter *);
static void EmitBinExpr(Emitter *, const char *op);

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
	do
		EmitStmt(this);
	while (body = body->Next);
}

static void EmitStmt(Emitter *this)
{
	if (AstIsExpr(this->node))
		EmitExpr(this);
}

static void EmitExpr(Emitter *this)
{
	switch (this->node->Kind)
	{
	case AST_Addition:
		EmitBinExpr(this, "add");
		break;
	}
}

static void EmitBinExpr(Emitter *this, const char *op)
{
	EmitExpr(this);
	Emitf(this, "\tpush    %%eax");
	EmitExpr(this);
	Emitf(this, "\tpop     %%eax");
	Emitf(this, "\t%-7.s %%edx, %%eax", op);
}