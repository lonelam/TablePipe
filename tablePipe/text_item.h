#pragma once
#include "stdafx.h"

using std::string;
class TextItem
{
private:
	string uspan;
public:
	
};

vector<string> generate_mcitem_text_from_page(pdf_document * pdf, int page_i)
{
	vector<string> ret;
	pdf_page * tmp_page = nullptr;
	pdf_obj * content_page = nullptr;
	fz_stream * stm = nullptr;
	fz_try(ctx)
	{
		tmp_page = pdf_load_page(ctx, pdf, page_i);
		content_page = pdf_page_contents(ctx, tmp_page);
		fz_page * pg = &tmp_page->super;
		fz_display_list * d_list = fz_new_display_list_from_page(ctx, pg);
		fz_device * dev = fz_new_list_device(ctx, d_list);
		pdf_run_page(ctx, tmp_page, dev, fz_identity, nullptr);
		fz_list_device * list_dev = (fz_list_device *)dev;
		for (fz_list_item * cur = list_dev->item_head; cur; cur = cur->next)
		{
			while (cur->id >= ret.size())
			{
				ret.push_back("");
			}
			//Create new display list
			fz_display_list * cur_list = fz_new_display_list(ctx, fz_bound_page(ctx, pg));
			int newsize = cur->end - cur->start;
			fz_display_node * old = cur_list->list;
			ptrdiff_t diff;

			cur_list->list = reinterpret_cast<fz_display_node *>(fz_resize_array(ctx, cur_list->list, newsize, sizeof(fz_display_node)));
			cur_list->max = newsize;
			cur_list->len = newsize;
			for (int j = 0; j < cur_list->len; j++)
			{
				cur_list->list[j] = d_list->list[cur->start + j];
			}

			//Extract text out
			fz_stext_page *  text_page = fz_new_stext_page(ctx, fz_bound_page(ctx, pg));
			fz_device * text_dev = fz_new_stext_device(ctx, text_page, NULL);
			fz_run_display_list(ctx, cur_list, text_dev, fz_identity, fz_infinite_rect, NULL);

			//read text and restore
			fz_stext_block *c_block;
			fz_stext_line *line;
			fz_stext_char *ch;
			char utf[10];

			for (c_block = text_page->first_block; c_block; c_block = c_block->next)
			{
				if (c_block->type == FZ_STEXT_BLOCK_TEXT)
				{
					for (line = c_block->u.t.first_line; line; line = line->next)
					{
						for (ch = line->first_char; ch; ch = ch->next)
						{
							int clen = fz_runetochar(utf, ch->c);
							for (int i = 0; i < clen; i++)
								ret[cur->id].push_back(utf[i]);
						}
					}
				}
			}
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "Generate MCitem from PDF Page %d Failed: %s\n", page_i, fz_caught_message(ctx));
	}
	return ret;
}


vector<fz_display_list *> generate_mcitem_list_from_page(pdf_document * pdf, int page_i)
{
	vector<fz_display_list *> ret;
	pdf_page * tmp_page = nullptr;
	pdf_obj * content_page = nullptr;
	fz_stream * stm = nullptr;
	fz_try(ctx)
	{
		tmp_page = pdf_load_page(ctx, pdf, page_i);
		content_page = pdf_page_contents(ctx, tmp_page);
		fz_page * pg = &tmp_page->super;
		fz_display_list * d_list = fz_new_display_list_from_page(ctx, pg);
		fz_device * dev = fz_new_list_device(ctx, d_list);
		pdf_run_page(ctx, tmp_page, dev, fz_identity, nullptr);
		fz_list_device * list_dev = (fz_list_device *)dev;
		for (fz_list_item * cur = list_dev->item_head; cur; cur = cur->next)
		{
			while (cur->id >= ret.size())
			{
				ret.push_back(NULL);
			}
			//Create new display list
			fz_display_list * cur_list = fz_new_display_list(ctx, fz_bound_page(ctx, pg));
			int newsize = cur->end - cur->start;
			fz_display_node * old = cur_list->list;
			ptrdiff_t diff;

			cur_list->list = reinterpret_cast<fz_display_node *>(fz_resize_array(ctx, cur_list->list, newsize, sizeof(fz_display_node)));
			cur_list->max = newsize;
			cur_list->len = newsize;
			//copy list nodes
			for (int j = 0; j < cur_list->len; j++)
			{
				cur_list->list[j] = d_list->list[cur->start + j];
			}

			ret[cur->id] = cur_list;
		}
	}
	fz_catch(ctx)
	{
		fprintf(stderr, "Generate MCitem from PDF Page %d Failed: %s\n", page_i, fz_caught_message(ctx));
	}
	return ret;
}