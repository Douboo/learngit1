/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#include "parse.h"

int git_parse_ctx_init(git_parse_ctx *ctx, const char *content, size_t content_len)
{
	if (content_len)
		ctx->content = content;
	else
		ctx->content = NULL;

	ctx->content_len = content_len;
	ctx->remain = ctx->content;
	ctx->remain_len = ctx->content_len;

	return 0;
}

void git_parse_ctx_clear(git_parse_ctx *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
}

void git_parse_advance_line(git_parse_ctx *ctx)
{
	ctx->line += ctx->line_len;
	ctx->remain_len -= ctx->line_len;
	ctx->line_len = git__linenlen(ctx->line, ctx->remain_len);
	ctx->line_num++;
}

void git_parse_advance_chars(git_parse_ctx *ctx, size_t char_cnt)
{
	ctx->line += char_cnt;
	ctx->remain_len -= char_cnt;
	ctx->line_len -= char_cnt;
}

int git_parse_advance_expected(
	git_parse_ctx *ctx,
	const char *expected,
	size_t expected_len)
{
	if (ctx->line_len < expected_len)
		return -1;

	if (memcmp(ctx->line, expected, expected_len) != 0)
		return -1;

	git_parse_advance_chars(ctx, expected_len);
	return 0;
}

int git_parse_advance_ws(git_parse_ctx *ctx)
{
	int ret = -1;

	while (ctx->line_len > 0 &&
		ctx->line[0] != '\n' &&
		git__isspace(ctx->line[0])) {
		ctx->line++;
		ctx->line_len--;
		ctx->remain_len--;
		ret = 0;
	}

	return ret;
}

int git_parse_advance_nl(git_parse_ctx *ctx)
{
	if (ctx->line_len != 1 || ctx->line[0] != '\n')
		return -1;

	git_parse_advance_line(ctx);
	return 0;
}
