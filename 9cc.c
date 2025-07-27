//SPDX-License-Identifier: GPL-2.0-only
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum tokenkind {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
};

struct token {
	enum tokenkind kind;
	struct token *next;
	int val;		//kindがTK_NUMの場合のその値
	char *loc;
};

//現在注目しているトークン
struct token *token;

//エラー報告兼終了関数
void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

bool consume (char op)
{
	if (token -> kind != TK_RESERVED || token -> loc[0] != op) {
		return false;
	}
	token = token -> next;
	return true;
}

void expect(char op)
{
	if (token -> kind != TK_RESERVED || token -> loc[0] != op) {
		error("'%c'ではありません", op);
	}
	token = token -> next;
}

int expect_number() 
{
	if (token -> kind != TK_NUM) {
		error("数ではありません");
	}
	int val = token -> val;
	token = token -> next;
	return val;
}

bool at_eof() {
	return token -> kind == TK_EOF;
}

struct token *new_token(enum tokenkind kind, struct token *cur, char *loc)
{
	struct token *tok = calloc(1, sizeof(struct token));
	tok -> kind = kind;
	tok -> loc = loc;
	cur -> next = tok;
	return tok;
}

struct token *tokenize(char *p)
{
	struct token head;
	head.next = NULL;
	struct token *cur = &head;

	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur -> val = strtol(p, &p, 10);
			continue;
		}
		error("トークナイズできません");
	}
	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		error("引数の個数が正しくありません");
		return 1;
	}

	token = tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("  mov rax, %d\n", expect_number()); 

	while(!at_eof()) {
		if (consume('+')) {
			printf("  add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	printf(".section .note.GNU-stack,\"\",@progbits\n");
	return 0;
}
