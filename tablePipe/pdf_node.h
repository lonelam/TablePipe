#pragma once
#include "stdafx.h"
#include "pdf_xref.h"
using std::wstring;
using std::endl;
using std::map;
using std::to_string;
using std::make_shared;
using std::shared_ptr;
class PdfNode
{
protected:
	pdf_obj * _obj;
public:
	PdfNode(pdf_obj * pObject) : _obj(pObject)
	{
		//pdf_type(ctx, pObject);
	}

	~PdfNode() { }
	void raw_debug();
	pdf_obj * getObj() { return _obj; }

	//debug under console
	virtual string meta(string indent = "") { return "no implemented meta"; }
	virtual int toInt() { throw std::runtime_error("PdfNode.toInt un implemented."); }
	// make it virtual after all
	virtual void debug() { cout << "unimplemented" << endl; }
};


shared_ptr<PdfNode> CreateNode(pdf_obj * obj);

class PdfNull : public PdfNode
{
public:
	PdfNull() : PdfNode(nullptr) {}
	PdfNull(pdf_obj * pObject) : PdfNode(pObject) {}
	void debug() { cout << "This is Null Node" << endl; }
	string meta(string indent = "") { return "<NULL>"; }
};
class PdfBool : public PdfNode
{
private:
	bool _bool;
public:
	PdfBool(pdf_obj * pObject);
	string meta(string indent = "") { return _bool?"<TRUE>":"<FALSE>"; }
};
class PdfInt : public PdfNode
{
private:
	int _int;
public:
	PdfInt(pdf_obj * pObject);
	void debug() { cout << "<Int " << _int << ">" << endl; }
	string meta(string indent = "") { return to_string(_int); }
	int toInt() { return _int; }
};
class PdfReal : public PdfNode
{

};

class PdfString : public PdfNode
{
private:
	string _str;
public:
	PdfString(pdf_obj * pObject);
	void debug() { cout << "<String " << _str << ">" << endl; }
	string meta(string indent = "");
};

class PdfName : public PdfNode
{
private:
	string _name;
public:
	PdfName(pdf_obj * pObject);
	string getName() const { return _name; }
	void debug() { cout << "<NameNode \"" << _name << "\">" << endl; }
	string meta(string indent = "") { return  "/" + _name; }
};
class PdfArray : public PdfNode
{
private:
	std::vector<shared_ptr<PdfNode>> _array;
	size_t _size;
public:
	PdfArray(pdf_obj * pObject);
	void debug();
	string meta(string indent = "");
	shared_ptr<PdfNode> get(int idx);
	size_t size() { return _size; }
};

