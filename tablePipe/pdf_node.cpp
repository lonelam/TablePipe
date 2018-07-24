#include "stdafx.h"
#include "pdf_node.h"
#include "fz_wrapper.h"
#include "pdf_dict.h"
#include "pdf_structure.h"
using std::pair;
//shared_ptr<PdfNode> CreateStream(pdf_obj * pObject)
//{
//	if (pObject == nullptr) return make_shared<PdfNull>(nullptr);
//	static map<pair<int, int>, shared_ptr<PdfNode> >PdfNodeCache;
//
//	if (t_pdf_is_indirect(pObject))
//	{
//		pair<int, int> id = { t_pdf_to_num(pObject), t_pdf_to_gen(pObject) };
//
//		if (PdfNodeCache.find(id) != PdfNodeCache.end())
//		{
//			return PdfNodeCache[id];
//		}
//	}
//
//	return make_shared<PdfStream>(pObject);
//}

shared_ptr<PdfNode> CreateNode(pdf_obj * pObject)
{
	if (pObject == nullptr) return make_shared<PdfNull>(nullptr);
	static map<pair<int, int>, shared_ptr<PdfNode> >PdfNodeCache;

	if (t_pdf_is_indirect(pObject))
	{
		pair<int, int> id = { t_pdf_to_num(pObject), t_pdf_to_gen(pObject) };
		
		if (PdfNodeCache.find(id) != PdfNodeCache.end())
		{
			return PdfNodeCache[id];
		}
	}
	obj_type type = t_check_pdf_type(pObject);
	switch (type)
	{
	case obj_type::obj_null:
		return make_shared<PdfNull>(pObject);
		break;
	case obj_type::obj_bool:
		return make_shared<PdfBool>(pObject);
		break;
	case obj_type::obj_int:
		return make_shared<PdfInt>(pObject);
		break;
	case obj_type::obj_real:
		break;
	case obj_type::obj_name:
		return make_shared<PdfName>(pObject);
		break;
	case obj_type::obj_string:
		return make_shared<PdfString>(pObject);
		break;
	case obj_type::obj_dict:
		return make_shared<PdfDict>(pObject);
		break;
	case obj_type::obj_indirect:
		break;
	case obj_type::obj_array:
		return make_shared<PdfArray>(pObject);
	case obj_type::obj_stream:
		break;
	case obj_type::obj_default:
		break;
	default:
		break;
	}
	return make_shared<PdfNull>(pObject);
}

PdfName::PdfName(pdf_obj * pObject) : PdfNode(pObject)
{
	_name = t_pdf_to_name(pObject);
}


/*
Construct PdfNode as an int
*/
PdfBool::PdfBool(pdf_obj * pObject) : PdfNode(pObject)
{
	_bool = t_pdf_to_bool(pObject);
}
PdfInt::PdfInt(pdf_obj * pObject) : PdfNode(pObject)
{
	_int = t_pdf_to_int(pObject);
}
PdfString::PdfString(pdf_obj * pObject) : PdfNode(pObject)
{
	_str = t_pdf_to_string(pObject);
}
PdfArray::PdfArray(pdf_obj * pObject) : PdfNode(pObject)
{
	_size = t_pdf_array_len(pObject);
	_array.resize(_size);
	//Lazy Ask to avoid stack overflow
}


//fz_device * PdfStream::_dev = fz_new_trace_device(ctx, fz_stdout(ctx));
//PdfStream::PdfStream(pdf_obj * pObject) : PdfDict(pObject), _stm(nullptr), _stm_obj(nullptr)
//{
//
//	pdf_document * pdf = nullptr;
//	fz_try(ctx)
//	{
//		pdf = pdf_get_indirect_document(ctx, _obj);
//	}
//	fz_catch(ctx)
//	{
//		fprintf(stderr, "[ERROR]Read indirect document Failed: %s", fz_caught_message(ctx));
//		_obj = nullptr;
//		return;
//	}
//	
//	fz_try(ctx)
//	{
//		_stm = pdf_open_contents_stream(ctx, pdf, _obj);
//	}
//	fz_catch(ctx)
//	{
//		fprintf(stderr, "[ERROR]Parse Stream Failed: %s", fz_caught_message(ctx));
//		return;
//	}
//
//	auto proc = pdf_new_run_processor(ctx, _dev, fz_identity, "View", NULL, 0, nullptr);
//	
//	pdf_debug_obj(ctx, _obj);
//	pdf_lexbuf buf;
//	fz_try(ctx)
//	{
//		pdf_lexbuf_init(ctx, &buf, PDF_LEXBUF_SMALL);
//		_obj = pdf_parse_stm_obj(ctx, pdf, _stm, &buf);
//	}
//	fz_catch(ctx)
//	{
//		fprintf(stderr, "[ERROR]Parse Stream Failed: %s", fz_caught_message(ctx));
//		return;
//	}
//	_stm_obj = CreateNode(_obj);
//	free();
//}

// Memory Accessers
shared_ptr<PdfNode> PdfArray::get(int idx)
{
	if (idx < 0 || idx >= _size)
	{
		return make_shared<PdfNull>(nullptr);
	}

	if (_array[idx] == nullptr)
	{
		//read and create target object
		return _array[idx] = CreateNode(t_pdf_array_get(_obj, idx));
	}
	else
	{
		return _array[idx];
	}
	return make_shared<PdfNull>(nullptr);
}
