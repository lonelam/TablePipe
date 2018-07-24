#include "stdafx.h"
#include "pdf_dict.h"
#include "pdf_xref.h"
#include "fz_wrapper.h"
#include <algorithm>
using std::unique;
using std::cout;
using std::endl;
using std::string;

shared_ptr<PdfName> CreateName(pdf_obj * pObject)
{
	if (t_pdf_is_name(pObject))
	{
		return make_shared<PdfName>(pObject);
	}
	throw std::runtime_error("PDF Object is Assert to be Name but it's not.");
}

PdfDict::PdfDict(pdf_obj * pObject) : PdfNode(pObject)
{
	_size = t_pdf_dict_len(pObject);
	//Lazy Ask to avoid stack overflow
	for (int i = 0; i < _size; i++)
	{
		_key_cache[i] = CreateName(t_pdf_dict_get_key(_obj, i));
	}
}
shared_ptr<PdfNode> PdfDict::get(int idx)
{
	if (idx < 0 || idx >= _size)
	{
		return make_shared<PdfNull>(nullptr);
	}
	return get(getName(idx));
}

shared_ptr<PdfNode> PdfDict::get(shared_ptr<PdfName> key)
{
	if (_dict_cache.find(key->getName()) == _dict_cache.end())
	{
		return _dict_cache[key->getName()] = CreateNode(t_pdf_dict_get(_obj, key->getObj()));
	}
	return _dict_cache[key->getName()];
}
shared_ptr<PdfName> PdfDict::getName(int idx)
{
	if (idx < 0 || idx >= _size)
	{
		throw std::out_of_range("[ERROR] Getting Dict Name in invalid range");
	}
	if (_key_cache.find(idx) == _key_cache.end())
	{
		return _key_cache[idx] = CreateName(t_pdf_dict_get_key(_obj, idx));
	}
	return _key_cache[idx];
}

shared_ptr<PdfNode> PdfDict::get(string key)
{
	if (_dict_cache.find(key) == _dict_cache.end())
	{
		return _dict_cache[key] = CreateNode(t_pdf_dict_gets(_obj, key.c_str()));
	}
	return _dict_cache[key];
}


shared_ptr<PdfDict> CreateDict(pdf_obj * pObject)
{
	// check type of pObject
	if (!pdf_is_dict(ctx, pObject))
	{
		throw std::runtime_error("[ERROR]CreateDict param is not a dict");
	}
	return make_shared<PdfDict>(pObject);
}


StructTreeRoot::StructTreeRoot(pdf_obj * pObject)
{
	if (!t_pdf_is_dict(pObject))
	{
		throw std::runtime_error("[ERROR] Creating StructTreeRoot with a non-dict object");
	}
	_Type = make_shared<PdfName>(t_pdf_dict_gets(pObject, "Type"));
	_RoleMap = make_shared<PdfDict>(t_pdf_dict_gets(pObject, "RoleMap"));
	_ParentTree = make_shared<PdfDict>(t_pdf_dict_gets(pObject, "ParentTree"));
	_Kids = make_shared<PdfArray>(t_pdf_dict_gets(pObject, "K"));
	_ParentTreeNextKey = make_shared<PdfInt>(t_pdf_dict_gets(pObject, "ParentTreeNextKey"));
	for (int i = 0; i < _Kids->size(); i++)
	{
		_docs.push_back(make_shared<StructElem>(_Kids->get(i)->getObj()));
	}
}

StructElem::StructElem(pdf_obj * pObject): _Pg_i_cache(-2)
{
	if (!t_pdf_is_dict(pObject))
	{
		throw std::runtime_error("[ERROR] Creating StructElem with a non-dict object");
	}
	pdf_obj * struct_elem_type = t_pdf_dict_gets(pObject, "Type");
	assert(t_pdf_to_string(struct_elem_type) == "StructElem");
	_P = make_shared<PdfDict>(t_pdf_dict_gets(pObject, "P"));
	_S = make_shared<PdfName>(t_pdf_dict_gets(pObject, "S"));
	_Pg = make_shared<PdfDict>(t_pdf_dict_gets(pObject, "Pg"));
	pdf_obj * k_arr = t_pdf_dict_gets(pObject, "K");
	int k_size = t_pdf_array_len(k_arr);

	for (int i = 0; i < k_size; i++)
	{
		pdf_obj * kid = t_pdf_array_get(k_arr, i);
		if (t_pdf_is_int(kid))
		{
			_contents.push_back(t_pdf_to_int(kid));
		}
		else if (t_pdf_is_dict(kid))
		{
			_kids.push_back(make_shared<StructElem>(kid));
		}
	}
	
}

int StructElem::Pg_i()
{
	if (_Pg_i_cache != -2)
	{
		return _Pg_i_cache;
	}
	if (_Pg)
	{
		return _Pg_i_cache = _Pg->get("StructParents")->toInt();
	}
	return _Pg_i_cache  = -1;
}

vector<pair<int, int>> StructElem::walk_contents()
{
	vector<pair<int, int>> ret;
	if (Pg_i() != -1)
	{
		for (int mcid : _contents)
		{
			ret.push_back({ Pg_i(), mcid });
		}
	}
	for (shared_ptr<StructElem> kid : _kids)
	{
		vector<pair<int, int>> tail = kid->walk_contents();
		ret.insert(ret.end(), tail.begin(), tail.end());
	}
	return ret;
}

vector<pair<int, int>> StructElem::sorted_contents()
{
	vector<pair<int, int>> ret = walk_contents();
	ret.erase(unique(ret.begin(), ret.end()), ret.end());
	return ret;
}