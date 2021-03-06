// tablePipe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "magic.h"
#include <fstream>
#include "pdf_xref.h"
#include "pdf_dict.h"
#include "pdf_node.h"
#include "text_item.h"
#include "fz_wrapper.h"


using std::shared_ptr;
using std::make_shared;

using std::cout;
using std::endl;
using std::experimental::filesystem::path;
using std::string;
const char * OUTPUT_FILE = ".\\output.txt";
void test_trailer(const char * documentPath);
void test_pages(const char * documentPath);
void test_contents(const char * documentPath);
void test_trace(const char * documentPath);
void test_json(const char * documentPath);



int main(int argc, char ** arg)
{
	magic();
	if (argc != 2)
	{
		return 0;
	}
	string fullPath(arg[1]);
	fwrite(fullPath.c_str(), 1, fullPath.size(), stdout);
	cout << endl;
	fz_document *doc = nullptr;

	wstring extext;
	if (!ctx)
	{
		fprintf(stderr, "cannot create mupdf context\n");
		return EXIT_FAILURE;
	}
	/* Register the default file types to handle. */
	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		fprintf(stderr, "cannot register document handlers: %s\n", fz_caught_message(ctx));
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
	//test_pages(fullPath.c_str());
	//test_contents(fullPath.c_str());
	test_trailer(fullPath.c_str());
	//pdf_obj * trailer = pdf_trailer(ctx, pdf);
	//test_json(fullPath.c_str());
    return 0;
}


void test_json(const char * documentPath)
{
	pdf_document * pdf = pdf_open_document(ctx, documentPath);
	ofstream fout(OUTPUT_FILE);

	int page_n = pdf_count_pages(ctx, pdf);
	vector<vector<string> > text_table(page_n);
	for (int i = 0; i < page_n; i++)
	{
		text_table[i] = generate_mcitem_text_from_page(pdf, i);
	}
	//shared_ptr<PdfNode> trailer = CreateNode(pdf_trailer(ctx, pdf));
	shared_ptr<StructTreeRoot> doc_root = make_shared<StructTreeRoot>(t_pdf_dict_getp(pdf_trailer(ctx, pdf), "Root/StructTreeRoot"));
	fout << doc_root->json_naive(text_table) << endl;
}

void test_trailer(const char * documentPath)
{
	fz_device * tr_dev = fz_new_trace_device(ctx, fz_stdout(ctx));


	pdf_document * pdf = pdf_open_document(ctx, documentPath);
	ofstream fout(OUTPUT_FILE);
	//cout << "This PDF Document Has " << pdf_count_objects(ctx, pdf) << " objects" << endl;
	int page_n = pdf_count_pages(ctx, pdf);
	shared_ptr<PdfNode> trailer = CreateNode(pdf_trailer(ctx, pdf));
	shared_ptr<StructTreeRoot> doc_root = make_shared<StructTreeRoot>(t_pdf_dict_getp(pdf_trailer(ctx, pdf), "Root/StructTreeRoot"));
	vector<vector<string> > text_table(page_n);
	vector<vector<fz_display_list * >>  item_table(page_n);
	for (int i = 0; i < page_n; i++)
	{
		text_table[i] = generate_mcitem_text_from_page(pdf, i);
		item_table[i] = generate_mcitem_list_from_page(pdf, i);
	}
	for (shared_ptr<StructElem> doc : doc_root->docs())
	{
		cout << "[DEBUG] document" << endl;
		for (shared_ptr<StructElem> section : doc->kids())
		{
			cout << "[DEBUG] section   " << section->S() << endl;
			if (section->S() == "P")
			{
				fout << "Para: " << endl;
				string extracted_text;
				for (pair<int,int> mcitem : section->sorted_contents())
				{
					extracted_text += text_table[mcitem.first][mcitem.second];
					//fz_run_display_list(ctx, item_table[mcitem.first][mcitem.second], tr_dev, fz_identity, fz_infinite_rect, NULL);
				}
				cout << extracted_text << endl;
				fout << extracted_text << endl;
			}
			else if (section->S() == "Table")
			{
				fout << "Table:" << endl;
				vector<int> fuck = section->contents();
				
				for (int mcid : fuck)
				{
					if (section->Pg_i() == -1)
					{
						cout << "debug" << endl;
					}
					fz_run_display_list(ctx, item_table[section->Pg_i()][mcid], tr_dev, fz_identity, fz_infinite_rect, NULL);
				}
				for (shared_ptr<StructElem> tpart : section->kids())
				{
					fout << "*****PArt  "<< tpart->S() <<"start" << endl;
					for (shared_ptr<StructElem> trow : tpart->kids())
					{
						fout << "********row  " << trow->S() << "start" << endl;
						for (shared_ptr<StructElem> tcell : trow->kids())
						{
							fout << "**************"<< tcell->S() << "cell start" << endl;
							for (pair<int, int> mcitem : tcell->sorted_contents())
							{
								fout << text_table[mcitem.first][mcitem.second];
								cout << mcitem.first << ", " << mcitem.second << endl;
								//fz_run_display_list(ctx, item_table[mcitem.first][mcitem.second], tr_dev, fz_identity, fz_infinite_rect, NULL);
							}

						}
					}
				}
				//fz_run_display_list(ctx, item_table[target_page][mcid], tr_dev, fz_identity, fz_infinite_rect, NULL);
			}
			
		}
	}
	trailer->raw_debug();
	//fout << trailer->meta() << endl;
	cout << "Parse Finished. Result: " << documentPath << endl;
}

void test_pages(const char * documentPath)
{
	pdf_document * pdf = pdf_open_document(ctx, documentPath);
	cout << "This PDF Document Has " << pdf_count_objects(ctx, pdf) << " objects" << endl;
	int n = pdf_count_pages(ctx, pdf);
	ofstream fout(OUTPUT_FILE);
	for (int i = 0; i < n; i++)
	{
		cout << "Running Page " <<  i << "." << endl;
		pdf_page * tmp_page = nullptr;
		pdf_obj * content_page = nullptr;
		fz_try(ctx)
		{
			tmp_page = pdf_load_page(ctx, pdf, i);
			content_page = pdf_page_contents(ctx, tmp_page);
		}
		fz_catch(ctx)
		{
			fprintf(stderr, "Load Page from PDF Document Failed: %s\n", fz_caught_message(ctx));
		}
		shared_ptr<PdfNode> page = CreateNode(content_page);
		fout << page->meta() << endl;
	}
}

void test_contents(const char * documentPath)
{
	pdf_document * pdf = pdf_open_document(ctx, documentPath);
	cout << "This PDF Document Has " << pdf_count_objects(ctx, pdf) << " objects" << endl;
	int n = pdf_count_pages(ctx, pdf);
	ofstream fout(OUTPUT_FILE);
	for (int i = 0; i < n; i++)
	{
		cout << "Running Page " << i << "." << endl;
		pdf_page * tmp_page = nullptr;
		pdf_obj * content_page = nullptr;
		fz_stream * stm = nullptr;
		fz_try(ctx)
		{
			vector<string> item_strs = generate_mcitem_text_from_page(pdf, i);
			for (string s : item_strs)
			{
				fout << s << endl;
			}
			fout << endl;
		}
		fz_catch(ctx)
		{
			fprintf(stderr, "Load Page from PDF Document Failed: %s\n", fz_caught_message(ctx));
		}
		cout << "Finish" << endl;
	}
}


void test_trace(const char * documentPath)
{
	pdf_document * pdf = pdf_open_document(ctx, documentPath);
	cout << "This PDF Document Has " << pdf_count_objects(ctx, pdf) << " objects" << endl;
	int n = pdf_count_pages(ctx, pdf);
	ofstream fout(OUTPUT_FILE);
	for (int i = 0; i < n; i++)
	{
		cout << "Running Page " << i << "." << endl;
		pdf_page * tmp_page = nullptr;
		pdf_obj * content_page = nullptr;
		fz_stream * stm = nullptr;
		fz_try(ctx)
		{
			tmp_page = pdf_load_page(ctx, pdf, i);
			content_page = pdf_page_contents(ctx, tmp_page);
			stm = pdf_open_contents_stream(ctx, pdf, content_page);
			fz_buffer * buf = fz_read_all(ctx, stm, 1024);
			cout << fz_string_from_buffer(ctx, buf) << endl;
			fz_page * pg = &tmp_page->super;
			fz_display_list * d_list = fz_new_display_list_from_page(ctx, pg);
			fz_device * dev = fz_new_trace_device(ctx, fz_stdout(ctx));
			pdf_run_page(ctx, tmp_page, dev, fz_identity, nullptr);
		}
		fz_catch(ctx)
		{
			fprintf(stderr, "Load Page from PDF Document Failed: %s\n", fz_caught_message(ctx));
		}
	}
}
