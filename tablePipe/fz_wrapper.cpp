#include"stdafx.h"
#include "fz_wrapper.h"
bool t_pdf_is_name(pdf_obj * obj)
{
	bool isName = false;
	fz_try(ctx)
	{
		if (pdf_is_name(ctx, obj))
		{
			isName = true;
		}
	}
	fz_catch(ctx)
	{
		isName = false;
		fprintf(stderr, "[ERROR]check if pdf is name Failed: %s", fz_caught_message(ctx));
	}
	return isName;
}


bool t_pdf_is_indirect(pdf_obj * obj)
{
	bool is_indirect = false;
	fz_try(ctx)
	{
		if (pdf_is_indirect(ctx, obj))
		{
			is_indirect = true;
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]check if pdf is indirect Failed: %s", fz_caught_message(ctx));
		return false;
	}
	return is_indirect;
}

bool t_pdf_is_dict(pdf_obj * obj)
{
	bool is_dict = false;
	fz_try(ctx)
	{
		if (pdf_is_dict(ctx, obj))
		{
			is_dict = true;
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]check if pdf is dict Failed: %s", fz_caught_message(ctx));
		return false;
	}
	return is_dict;
}

bool t_pdf_is_int(pdf_obj * obj)
{
	bool is_int = false;
	fz_try(ctx)
	{
		if (pdf_is_int(ctx, obj))
		{
			is_int = true;
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]check if pdf is int Failed: %s", fz_caught_message(ctx));
		return false;
	}
	return is_int;
}


int t_pdf_to_num(pdf_obj * obj)
{
	int ret = -1;
	fz_try(ctx)
	{
		ret = pdf_to_num(ctx, obj);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]object id num query failed: %s", fz_caught_message(ctx));
		return -1;
	}
	return ret;
}

int t_pdf_to_gen(pdf_obj * obj)
{
	int ret = -1;
	fz_try(ctx)
	{
		ret = pdf_to_gen(ctx, obj);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]object id gen query failed: %s", fz_caught_message(ctx));
		return -1;
	}
	return ret;
}



obj_type t_check_pdf_type(pdf_obj * obj)
{
	obj_type type = obj_type::obj_default;
	fz_try(ctx)
	{
		if (pdf_is_name(ctx, obj))
		{
			type = obj_type::obj_name;
			break;
		}
		if (pdf_is_string(ctx, obj))
		{
			type = obj_type::obj_string;
			break;
		}
		if (pdf_is_int(ctx, obj))
		{
			type = obj_type::obj_int;
			break;
		}
		if (pdf_is_bool(ctx, obj))
		{
			type = obj_type::obj_bool;
			break;
		}
		//if (pdf_is_indirect(ctx, pObject))
		//{
		//	fz_try(ctx)
		//	{
		//		assert(pdf_is_dict(ctx, pObject));
		//	}
		//	fz_catch(ctx)
		//	{

		//	}
		//	type = obj_type::obj_indirect;
		//	break;
		//}
		//if (pdf_is_stream(ctx, pObject))
		//{
		//	type = obj_type::obj_stream;
		//	break;
		//}
		if (pdf_is_dict(ctx, obj))
		{
			type = obj_type::obj_dict;
			break;
		}
		if (pdf_array_len(ctx, obj))
		{
			type = obj_type::obj_array;
		}
		if (pdf_is_null(ctx, obj))
		{
			type = obj_type::obj_null;
			break;
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]object type detection failed: %s", fz_caught_message(ctx));
	}
	return type;
}


string t_pdf_to_name(pdf_obj * obj)
{
	string name;
	fz_try(ctx)
	{
		name = string(pdf_to_name(ctx, obj));
	}
	fz_catch(ctx)
	{
		name = "<InValid Name>";
		fprintf(stderr, "[ERROR]Read Pdf Object As Name Failed: %s", fz_caught_message(ctx));
	}
	return name;
}

bool t_pdf_to_bool(pdf_obj * obj)
{
	bool bo;
	fz_try(ctx)
	{
		bo = pdf_to_bool(ctx, obj);
	}
	fz_catch(ctx)
	{
		bo = false;
		fprintf(stderr, "[ERROR]Read Pdf Object As Int Failed: %s", fz_caught_message(ctx));
	}
	return bo;
}


int t_pdf_to_int(pdf_obj * obj)
{
	int ret = -1;
	fz_try(ctx)
	{
		ret = pdf_to_int(ctx, obj);
	}
	fz_catch(ctx)
	{
		ret = -1;
		fprintf(stderr, "[ERROR]Read Pdf Object As Int Failed: %s", fz_caught_message(ctx));
	}
	return ret;
}

string t_pdf_to_string(pdf_obj * obj)
{
	string ret;
	fz_try(ctx)
	{
		ret = pdf_to_string(ctx, obj, nullptr);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object As String Failed: %s", fz_caught_message(ctx));
	}
	return ret;
}


size_t t_pdf_dict_len(pdf_obj * obj)
{
	size_t size = 0;
	fz_try(ctx)
	{
		size = pdf_dict_len(ctx, obj);
	}
	fz_catch(ctx)
	{
		size = 0;
		fprintf(stderr, "[ERROR]Read Pdf Object As Dict Failed: %s", fz_caught_message(ctx));
	}
	return size;
}

size_t t_pdf_array_len(pdf_obj * obj)
{
	size_t size = 0;
	fz_try(ctx)
	{
		size = pdf_array_len(ctx, obj);
	}
	fz_catch(ctx)
	{
		size = 0;
		fprintf(stderr, "[ERROR]Read Pdf Object As Array Failed: %s", fz_caught_message(ctx));
	}
	return size;
}


pdf_obj * t_pdf_array_get(pdf_obj *array, int i)
{
	pdf_obj * tmp = nullptr;
	fz_try(ctx)
	{
		tmp = pdf_array_get(ctx, array, i);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object As Array Failed: %s", fz_caught_message(ctx));
	}
	return tmp;
}

pdf_obj * t_pdf_dict_get_key(pdf_obj *dict, int idx)
{
	pdf_obj * tmp = nullptr;
	fz_try(ctx)
	{
		tmp = pdf_dict_get_key(ctx, dict, idx);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object Using Dict Index Failed: %s", fz_caught_message(ctx));
	}
	return tmp;
}

pdf_obj * t_pdf_dict_gets(pdf_obj *dict, const char *key)
{
	pdf_obj * tmp = nullptr;
	fz_try(ctx)
	{
		tmp = pdf_dict_gets(ctx, dict, key);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object Using Dict Key Failed: %s", fz_caught_message(ctx));
	}
	return tmp;
}

pdf_obj * t_pdf_dict_getp(pdf_obj * dict, const char * path)
{
	pdf_obj * tmp = nullptr;
	fz_try(ctx)
	{
		tmp = pdf_dict_getp(ctx, dict, path);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object Using a Path Failed: %s", fz_caught_message(ctx));
	}
	return tmp;
}

pdf_obj *t_pdf_dict_get(pdf_obj *dict, pdf_obj *key)
{
	pdf_obj * tmp = nullptr;
	fz_try(ctx)
	{
		tmp = pdf_dict_get(ctx, dict, key);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]Read Pdf Object Using Dict Key Object Failed: %s", fz_caught_message(ctx));
	}
	return tmp;
}