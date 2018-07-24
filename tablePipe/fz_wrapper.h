#pragma once
#include"stdafx.h"
using std::string;
bool t_pdf_is_name(pdf_obj * obj);
bool t_pdf_is_indirect(pdf_obj * obj);
bool t_pdf_is_dict(pdf_obj * obj);
bool t_pdf_is_int(pdf_obj * obj);

int t_pdf_to_num(pdf_obj * obj);
int t_pdf_to_gen(pdf_obj * obj);
enum class obj_type
{
	obj_null, obj_bool, obj_int, obj_real, obj_name,
	obj_string, obj_dict, obj_indirect, obj_array, obj_stream,
	obj_default
};
obj_type t_check_pdf_type(pdf_obj * obj);
string t_pdf_to_name(pdf_obj * obj);
bool t_pdf_to_bool(pdf_obj * obj);
int t_pdf_to_int(pdf_obj * obj);
string t_pdf_to_string(pdf_obj * obj);
size_t t_pdf_dict_len(pdf_obj * obj);
size_t t_pdf_array_len(pdf_obj * obj);
pdf_obj * t_pdf_array_get(pdf_obj *array, int i);
pdf_obj * t_pdf_dict_get_key(pdf_obj *dict, int idx);
pdf_obj * t_pdf_dict_gets(pdf_obj *dict, const char *key);
pdf_obj * t_pdf_dict_getp(pdf_obj * dict, const char * path);
pdf_obj *t_pdf_dict_get(pdf_obj *dict, pdf_obj *key);


