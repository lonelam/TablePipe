#pragma once
#include "stdafx.h"
#include "pdf_node.h"
using std::vector;
using std::shared_ptr;
using std::make_shared;
class PdfDict : public PdfNode
{
protected:
	map<string, shared_ptr<PdfNode>> _dict_cache;
	map<int, shared_ptr<PdfName>> _key_cache;
	size_t _size;
public:
	shared_ptr<PdfNode> get(int idx);
	shared_ptr<PdfNode> operator[](int idx) { return get(idx); }
	shared_ptr<PdfName> getName(int idx);
	shared_ptr<PdfNode> get(string key);
	shared_ptr<PdfNode> operator[](string key) { return get(key); }
	shared_ptr<PdfNode> get(shared_ptr<PdfName> key);
	shared_ptr<PdfNode> operator[](shared_ptr<PdfName> key) { return get(key); }

	
	PdfDict(pdf_obj * pObject);
	void debug();
	string meta(string indent = "");
};

using std::pair;
class StructElem
{
protected:
	shared_ptr<PdfName> _S;
	shared_ptr<PdfDict> _P;
	shared_ptr<PdfDict> _Pg;
	vector<shared_ptr<StructElem>> _kids;
	vector<int> _contents;
	int _Pg_i_cache;
public:
	StructElem(pdf_obj * pObject);
	vector<shared_ptr<StructElem>> kids() { return _kids; }
	shared_ptr<PdfDict> P() { return _P; }
	string S() { return _S->getName(); }
	shared_ptr<PdfDict> Pg() { return _Pg; }
	int Pg_i();
	vector<int> contents() { return _contents; }
	vector<pair<int, int>> walk_contents();
	vector<pair<int, int>> sorted_contents();
	string meta();
};

class StructParagraph
{

};

class StructTreeRoot
{
protected:
	shared_ptr<PdfName> _Type;
	shared_ptr<PdfDict> _RoleMap;
	shared_ptr<PdfDict> _ParentTree;
	shared_ptr<PdfArray> _Kids;
	shared_ptr<PdfInt> _ParentTreeNextKey;
	vector<shared_ptr<StructElem>> _docs;
public:
	StructTreeRoot(pdf_obj * pObject);
	vector<shared_ptr<StructElem>> docs() { return _docs; }
	string json_naive(vector<vector<string> > & text_table);
};

//class PdfStream : public PdfDict
//{
//private:
//	shared_ptr<PdfNode> _stm_obj;
//	fz_stream * _stm;
//	static fz_device * _dev;
//public:
//	PdfStream(pdf_obj * pObject);
//	string meta(string indent = "");
//	//void resolve();
//};

shared_ptr<PdfDict> CreateDict(pdf_obj * pObject);
