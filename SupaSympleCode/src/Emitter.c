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

	Emitf(this, "_SSyc.fmt:");
	Emitf(this, "\t.string \"Value: %%f\\n\"");
	Emitf(this);

	Emitf(this, "_SSyc.print:");
	Emitf(this, "\tsub $12, %%esp");
	Emitf(this, "\tcvtss2sd %%xmm0, %%xmm0");
	Emitf(this, "\tmovsd %%xmm0, 4(%%esp)");
	Emitf(this, "\tmov $_SSyc.fmt, %%eax");
	Emitf(this, "\tmov %%eax, (%%esp)");
	Emitf(this, "\tcall _printf");
	Emitf(this, "\tadd $12, %%esp");
	Emitf(this, "\tret");
	Emitf(this);

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

	Emitf(this, "\tmovss %%xmm0, -4(%%esp)");
	Emitf(this, "\tmov -4(%%esp), %%eax");
	Emitf(this, "\tret");
}

static void EmitStmt(Emitter *this)
{
	if (AstIsExpr(this->node))
	{
		EmitExpr(this);
		Emitf(this, "\tcall _SSyc.print");
		Emitf(this);
	}

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
		union
		{
			float fval;
			int ival;
		} val;
		val.fval = strtof(num->Text, null);
		Emitf(this, "\tmovl $0x%x, -4(%%esp) # %f", val.ival, val.fval);
		Emitf(this, "\tmovss -4(%%esp), %%xmm0");
		break;
	}
	case AST_Addition:
		EmitBinExpr(this, "add");
		break;
	case AST_Subtraction:
		EmitBinExpr(this, "sub");
		break;
	case AST_Multiplication:
		EmitBinExpr(this, "mul");
		break;
	case AST_Division:
		EmitBinExpr(this, "div");
		break;
	case AST_Modulo:
		ErrorAt(this->node->Token, "Modulo is not supported!");
		break;
	}
}

static void EmitBinExpr(Emitter *this, const char *op)
{
	Next(this);

	EmitExpr(this);
	Emitf(this, "\tsub $4, %%esp");
	Emitf(this, "\tmovss %%xmm0, (%%esp)");
	EmitExpr(this);
	Emitf(this, "\tmovss (%%esp), %%xmm1");
	Emitf(this, "\tadd $4, %%esp");
	Emitf(this, "\t%sss %%xmm1, %%xmm0", op);
}

static const AstNode *Next(Emitter *this)
{
	const AstNode *node = this->node;
	if (this->node->Next)
		this->node = this->node->Next;

	return node;
}