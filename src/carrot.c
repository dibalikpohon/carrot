#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../lib/include/stb_ds.h"

#define MAX_BUFFER_SIZE 1024

char *read_source_file(char *filename) {
	char *source;
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Could not open '%s'\n", filename);
		return NULL;
	}

	// calculate source file size
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	source = (char*) malloc(sizeof(char) * (size + 1));
	fread(source, 1, size, file);
	source[size] = '\0';

	fclose(file);

	return source;
}

CarrotObj print(CarrotObj *args) {
	printf("HELLO FROM BUILTIN FUNCTION\n");

	CarrotObj ob;
	ob.type = N_NULL;
	return ob;
}

int main(int argc, char **argv) {
	char *source;
	char *filename = argv[1];

	if (argc != 2) {
		printf("Specify source file");
		exit(1);
	}

	source = read_source_file(filename);
	if (source) {
		Parser parser;
		parser_init(&parser, source);
		CarrotObj n = parser_parse(&parser);
		
		Interpreter interpreter = create_interpreter();

		/* register builtin function */
		CarrotObj print_func;
		print_func.type = N_FUNC_DEF;
		print_func.is_builtin = 1;
		print_func.builtin_func = print;
		strcpy(print_func.func_name, "print");
		shput(interpreter.sym_table, "print", print_func);

		interpreter_interpret(&interpreter, &n);
		// TODO: free sym_table

		free_node(&n);

		//printf("%d\n", n.statements[0].int_val);
		printf("===================\n");
		for (int i = 0; i < parser.lexer.token_cnt; i++) {
			printf("%s\n", parser.lexer.tokens[i].text);
		}
		printf("===================\n");

		parser_free(&parser);
		free(source);

		return 0;
	} else {
		return 1;
	}
}
