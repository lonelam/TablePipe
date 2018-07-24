#pragma once
#include "stdafx.h"
#include "pdf_xref.h"
#include "pdf_dict.h"
#include "pdf_node.h"
#include "fz_wrapper.h"

enum {
	CS_UNCHANGED = 0,
	CS_GRAY_0 = 1,
	CS_GRAY_1 = 2,
	CS_RGB_0 = 3,
	CS_RGB_1 = 4,
	CS_CMYK_0 = 5,
	CS_CMYK_1 = 6,
	CS_OTHER_0 = 7,

	ALPHA_UNCHANGED = 0,
	ALPHA_1 = 1,
	ALPHA_0 = 2,
	ALPHA_PRESENT = 3,

	CTM_UNCHANGED = 0,
	CTM_CHANGE_AD = 1,
	CTM_CHANGE_BC = 2,
	CTM_CHANGE_EF = 4,

	MAX_NODE_SIZE = (1 << 9) - sizeof(fz_display_node)
};


enum { ISOLATED = 1, KNOCKOUT = 2 };
enum { OPM = 1, OP = 2, BP = 3, RI = 4 };



#define SIZE_IN_NODES(t) \
	((t + sizeof(fz_display_node) - 1) / sizeof(fz_display_node))

/* unpack ri, op, opm, bp from flags, even/odd in lower bit */
static void
fz_unpack_color_params(fz_color_params *color_params, int flags)
{
	color_params->ri = (flags >> RI) & 3;
	color_params->bp = (flags >> BP) & 1;
	color_params->op = (flags >> OP) & 1;
	color_params->opm = (flags >> OPM) & 1;
}



typedef struct fz_list_tile_data_s fz_list_tile_data;

struct fz_list_tile_data_s
{
	float xstep;
	float ystep;
	fz_rect view;
	int id;
};

class TableSkeleton
{
public:
	vector<int> discreter_x, discreter_y;
	//
	TableSkeleton(fz_display_list * d_list)
	{
		fz_display_node *node;
		fz_display_node *node_end;
		fz_display_node *next_node;
		int clipped = 0;
		int tiled = 0;
		int progress = 0;

		/* Current graphics state as unpacked from list */
		fz_path *path = NULL;
		float alpha = 1.0f;
		fz_matrix ctm = fz_identity;
		fz_stroke_state *stroke = NULL;
		float color[FZ_MAX_COLORS] = { 0 };
		fz_colorspace *colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
		fz_color_params color_params;
		fz_rect rect = { 0 };

		/* Transformed versions of graphic state entries */
		fz_rect trans_rect;
		fz_matrix trans_ctm;
		int tile_skip_depth = 0;

		fz_var(colorspace);

		color_params = *fz_default_color_params(ctx);

		node = d_list->list;
		node_end = &d_list->list[d_list->len];
		for (; node != node_end; node = next_node)
		{
			int empty;
			fz_display_node n = *node;

			next_node = node + n.size;

			node++;

			if (n.rect)
			{
				rect = *(fz_rect *)node;
				node += SIZE_IN_NODES(sizeof(fz_rect));
			}

			if (n.cs)
			{
				int i, en;

				fz_drop_colorspace(ctx, colorspace);
				switch (n.cs)
				{
				default:
				case CS_GRAY_0:
					colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
					color[0] = 0.0f;
					break;
				case CS_GRAY_1:
					colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
					color[0] = 1.0f;
					break;
				case CS_RGB_0:
					colorspace = fz_keep_colorspace(ctx, fz_device_rgb(ctx));
					color[0] = 0.0f;
					color[1] = 0.0f;
					color[2] = 0.0f;
					break;
				case CS_RGB_1:
					colorspace = fz_keep_colorspace(ctx, fz_device_rgb(ctx));
					color[0] = 1.0f;
					color[1] = 1.0f;
					color[2] = 1.0f;
					break;
				case CS_CMYK_0:
					colorspace = fz_keep_colorspace(ctx, fz_device_cmyk(ctx));
					color[0] = 0.0f;
					color[1] = 0.0f;
					color[2] = 0.0f;
					color[3] = 0.0f;
					break;
				case CS_CMYK_1:
					colorspace = fz_keep_colorspace(ctx, fz_device_cmyk(ctx));
					color[0] = 0.0f;
					color[1] = 0.0f;
					color[2] = 0.0f;
					color[3] = 1.0f;
					break;
				case CS_OTHER_0:
					colorspace = fz_keep_colorspace(ctx, *(fz_colorspace **)(node));
					node += SIZE_IN_NODES(sizeof(fz_colorspace *));
					en = fz_colorspace_n(ctx, colorspace);
					for (i = 0; i < en; i++)
						color[i] = 0.0f;
					break;
				}
			}

			if (n.color)
			{
				int nc = fz_colorspace_n(ctx, colorspace);
				memcpy(color, (float *)node, nc * sizeof(float));
				node += SIZE_IN_NODES(nc * sizeof(float));
			}

			if (n.alpha)
			{
				switch (n.alpha)
				{
				default:
				case ALPHA_0:
					alpha = 0.0f;
					break;
				case ALPHA_1:
					alpha = 1.0f;
					break;
				case ALPHA_PRESENT:
					alpha = *(float *)node;
					node += SIZE_IN_NODES(sizeof(float));
					break;
				}
			}
			if (n.ctm != 0)
			{
				float *packed_ctm = (float *)node;
				if (n.ctm & CTM_CHANGE_AD)
				{
					ctm.a = *packed_ctm++;
					ctm.d = *packed_ctm++;
					node += SIZE_IN_NODES(2 * sizeof(float));
				}
				if (n.ctm & CTM_CHANGE_BC)
				{
					ctm.b = *packed_ctm++;
					ctm.c = *packed_ctm++;
					node += SIZE_IN_NODES(2 * sizeof(float));
				}
				if (n.ctm & CTM_CHANGE_EF)
				{
					ctm.e = *packed_ctm++;
					ctm.f = *packed_ctm;
					node += SIZE_IN_NODES(2 * sizeof(float));
				}
			}

			if (n.stroke)
			{
				fz_drop_stroke_state(ctx, stroke);
				stroke = fz_keep_stroke_state(ctx, *(fz_stroke_state **)node);
				node += SIZE_IN_NODES(sizeof(fz_stroke_state *));
			}

			if (n.path)
			{
				fz_drop_path(ctx, path);
				path = fz_keep_path(ctx, (fz_path *)node);
				node += SIZE_IN_NODES(fz_packed_path_size(path));
			}

			if (tile_skip_depth > 0)
			{
				if (n.cmd == FZ_CMD_BEGIN_TILE)
					tile_skip_depth++;
				else if (n.cmd == FZ_CMD_END_TILE)
					tile_skip_depth--;
				if (tile_skip_depth > 0)
					continue;
			}

			trans_rect = fz_transform_rect(rect, fz_identity);


			/* cull objects to draw using a quick visibility test */

			if (tiled ||
				n.cmd == FZ_CMD_BEGIN_TILE || n.cmd == FZ_CMD_END_TILE ||
				n.cmd == FZ_CMD_RENDER_FLAGS || n.cmd == FZ_CMD_DEFAULT_COLORSPACES ||
				n.cmd == FZ_CMD_BEGIN_LAYER || n.cmd == FZ_CMD_END_LAYER || n.cmd == FZ_CMD_BEGIN_MCITEM)
			{
				empty = 0;
			}
			else
			{
				empty = fz_is_empty_rect(fz_intersect_rect(trans_rect, fz_infinite_rect));
			}

			if (clipped || empty)
			{
				switch (n.cmd)
				{
				case FZ_CMD_CLIP_PATH:
				case FZ_CMD_CLIP_STROKE_PATH:
				case FZ_CMD_CLIP_TEXT:
				case FZ_CMD_CLIP_STROKE_TEXT:
				case FZ_CMD_CLIP_IMAGE_MASK:
				case FZ_CMD_BEGIN_MASK:
				case FZ_CMD_BEGIN_GROUP:
					clipped++;
					continue;
				case FZ_CMD_POP_CLIP:
				case FZ_CMD_END_GROUP:
					if (!clipped)
						goto visible;
					clipped--;
					continue;
				case FZ_CMD_END_MASK:
					if (!clipped)
						goto visible;
					continue;
				default:
					continue;
				}
			}

		visible:
			trans_ctm = fz_concat(ctm, fz_identity);

			fz_try(ctx)
			{
				switch (n.cmd)
				{
				case FZ_CMD_FILL_PATH:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_STROKE_PATH:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_CLIP_PATH:
					break;
				case FZ_CMD_CLIP_STROKE_PATH:
					break;
				case FZ_CMD_FILL_TEXT:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_STROKE_TEXT:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_CLIP_TEXT:
					break;
				case FZ_CMD_CLIP_STROKE_TEXT:
					break;
				case FZ_CMD_IGNORE_TEXT:
					break;
				case FZ_CMD_FILL_SHADE:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_FILL_IMAGE:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_FILL_IMAGE_MASK:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_CLIP_IMAGE_MASK:
					break;
				case FZ_CMD_POP_CLIP:
					break;
				case FZ_CMD_BEGIN_MASK:
					fz_unpack_color_params(&color_params, n.flags);
					break;
				case FZ_CMD_END_MASK:
					break;
				case FZ_CMD_BEGIN_GROUP:
					break;
				case FZ_CMD_END_GROUP:
					break;
				case FZ_CMD_BEGIN_TILE:
				{
					int cached;
					fz_list_tile_data *data = (fz_list_tile_data *)node;
					fz_rect tile_rect;
					tiled++;
					tile_rect = data->view;
					break;
				}
				case FZ_CMD_END_TILE:
					tiled--;
					break;
				case FZ_CMD_RENDER_FLAGS:
					break;
				case FZ_CMD_DEFAULT_COLORSPACES:
					break;
				case FZ_CMD_BEGIN_LAYER:
					break;
				case FZ_CMD_BEGIN_MCITEM:
					break;
				case FZ_CMD_END_LAYER:
					break;
				}
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_ABORT)
					break;
				fz_warn(ctx, "Ignoring error during interpretation");
			}

		}
	}
};
