#include "stdafx.h"
#include "pdf_xref.h"

void pdf_type(fz_context * ctx, pdf_obj * obj)
{
	if (pdf_is_null(ctx, obj)) cout << "null";
	if (pdf_is_bool(ctx, obj)) cout << "bool";
	if (pdf_is_int(ctx, obj)) cout << "int";
	if (pdf_is_real(ctx, obj)) cout << "real";
	if (pdf_is_number(ctx, obj)) cout << "number";
	if (pdf_is_name(ctx, obj)) cout << "name";
	if (pdf_is_string(ctx, obj)) cout << "string";
	if (pdf_is_array(ctx, obj)) cout << "array";
	if (pdf_is_dict(ctx, obj)) cout << "dict";
	if (pdf_is_indirect(ctx, obj)) cout << "indirect";
	if (pdf_is_stream(ctx, obj)) cout << "stream";
	if (pdf_array_len(ctx, obj)) cout << "array";
}