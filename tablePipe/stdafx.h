// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE
//#if _MSC_VER>=1900
//#include "stdio.h" 
//_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
//#ifdef __cplusplus 
//extern "C"
//#endif 
//FILE* __cdecl __iob_func(unsigned i) {
//	return __acrt_iob_func(i);
//}
//#endif
#include <Windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <cassert>
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"


extern fz_context * ctx;

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include <array>
#include <map>
#include <vector>
#include <utility>
const int MAXINDENT = 30;