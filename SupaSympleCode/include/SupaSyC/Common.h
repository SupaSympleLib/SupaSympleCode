#pragma once
#pragma warning(disable: 26812 6031 4090 28182)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "SupaSyC/Console.h"
#include "SupaSyC/File.h"
#include "SupaSyC/Token.h"
#include "SupaSyC/Ast.h"
#include "SupaSyC/Evaluator.h"

bool StringStartsWith(const char *string, const char *start);

__declspec(noreturn) void Error(const char *fmt, ...);
__declspec(noreturn) void ErrorAt(const Token *, const char *fmt, ...);

Token* Lex(const File* file);
AstNode* Parse(const Token* tokens);

#define Alloc(count, ty) (ty*)_Alloc(count, sizeof(ty))
#define Free(ptr) free((void*)ptr)

void *_Alloc(uint32_t count, uint32_t sz);