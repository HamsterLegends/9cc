//SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	char *p = argv[1];

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("  mov rax, %ld\n", strtol(p, &p, 10));

	while(*p) {
		if (*p == '+') {
			p++;
			printf("  add rax, %ld\n", strtol(p, &p, 10));
			continue;
		}
		
		if (*p == '-') {
			p++;
			printf("  sub rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		fprintf(stderr,"予期しない文字です: '%c'\n", *p);
		return 1;
	}
	printf("  ret\n");
	printf(".section .note.GNU-stack,\"\",@progbits\n");
	return 0;
}
