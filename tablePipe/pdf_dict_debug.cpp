#include "stdafx.h"
#include "pdf_dict.h"

string PdfDict::meta(string indent)
{
	if (indent.size() > MAXINDENT) return "<DICT OUT OF RANGE>";
	string ret = "< Dictionary meta :" + to_string(_size) + ": \n";
	ret.append(indent);
	for (int i = 0; i < _size; i++)
	{
		string name = getName(i)->getName();
		ret.append(name);
		ret.append(", ");
		if (name == "Parent")
		{
			ret.append("<Dict Parent>");
		}
		else if (name == "Pg")
		{
			ret.append("<Dict Pg>");
		}
		else if (name == "P")
		{
			ret.append("<Dict P>");
		}
		else
		{
			ret.append(get(i)->meta(indent + " |"));
		}
		ret.append("\n");
		ret.append(indent);
	}
	ret.append(">");
	return ret;
}
//string PdfStream::meta(string indent)
//{
//	return "{Stream " + PdfDict::meta(indent) + "}";
//}
void PdfDict::debug()
{
	cout << "<Dictionary {";
	for (int i = 0; i < _size; i++)
	{
		cout << _key_cache[i]->getName() << ", " << _dict_cache[_key_cache[i]->getName()]->meta() << endl;
	}
	cout << "}>" << endl;
}