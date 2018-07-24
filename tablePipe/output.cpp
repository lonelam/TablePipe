#include "stdafx.h"
#include "pdf_dict.h"
using std::string;
string StructTreeRoot::json_naive(vector<vector<string> > & text_table)
{
	int page_n = text_table.size();
	string ret;
	ret += "[";//start array
	for (shared_ptr<StructElem> doc : _docs)
	{
		ret += "{ \"type\": \"document\", ";
		ret += "\"sections\": ["; //start of array
		for (shared_ptr<StructElem> section : doc->kids())
		{
			if (section->S() == "P")
			{
				ret += "{ \"type\": \"paragraph\", \"text\": \"";
				for (pair<int, int> mcitem : section->sorted_contents())
				{
					ret += text_table[mcitem.first][mcitem.second];
				}
				ret += "\"},";
			}
			else if (section->S() == "Table")
			{
				ret += "{ \"type\": \"table\", \"table\": [";
				vector<int> fuck = section->contents();
				for (int mcid : fuck)
				{
					if (section->Pg_i() == -1)
					{
						cout << "debug" << endl;
					}
				}

				for (shared_ptr<StructElem> tpart : section->kids())
				{
					ret += "[";
					for (shared_ptr<StructElem> trow : tpart->kids())
					{
						ret += "[";
						for (shared_ptr<StructElem> tcell : trow->kids())
						{
							ret += "\"";
							for (pair<int, int> mcitem : tcell->sorted_contents())
							{
								ret += text_table[mcitem.first][mcitem.second];
							}
							ret += "\",";
						}
						ret += "],";
					}
					ret += "], ";
				}
				ret += "],},";
			}
		}
		ret += "]";//end of array
		ret += "},";
	}
	ret += "]";// end array
	return ret;
}
