#include "stdafx.h"
#include "pdf_node.h"
#include "pdf_dict.h"

//debug print all infomations on screen
void PdfNode::raw_debug()
{
	fz_try(ctx)
	{
		pdf_debug_obj(ctx, _obj);
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "[ERROR]call pdf_debug_obj Failed: %s", fz_caught_message(ctx));
	}
}


void PdfArray::debug()
{
	cout << "Array [";
	for (int i = 0; i < _size; i++)
	{
		cout << _array[i]->meta() << ", ";
	}
	cout << "]" << endl;
}


//meta is for further infomation recursively
string PdfString::meta(string indent)
{
	return "<String \"" + _str + "\">";
}

string PdfArray::meta(string indent)
{
	if (indent.size() > MAXINDENT) return "<ARRAY OUT OF RANGE>";
	string ret = "<Array ";
	ret.append(to_string(_size));
	ret.append(": \n");
	ret.append(indent);
	for (int i = 0; i < _size; i++)
	{
		ret.append(get(i)->meta(indent + " |"));
		ret.append(",\n");
		ret.append(indent);
	}
	ret.append(">");
	return ret;
}

