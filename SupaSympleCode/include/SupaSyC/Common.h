#pragma once
#pragma warning(disable: 26812 6031)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "SupaSyC/File.h"
#include "SupaSyC/Console.h"
#include "SupaSyC/Token.h"

Token* Lex(const File* file);

#define Alloc(count, ty) (ty*)_Alloc(count, sizeof(ty))
#define Free(ptr) free((void*)ptr)

inline void *_Alloc(uint32_t count, uint32_t sz)
{
	void *ptr = calloc(count, sz);
	assert(ptr);
	return ptr;
}