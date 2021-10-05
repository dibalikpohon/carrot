#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/parser.h"
#include "include/interpreter.h"
#include "include/builtin_func.h"
#include "lib/include/stb_ds.h"

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

	source = (char*) calloc(1, sizeof(char) * (size + 1));
	fread(source, 1, size, file);
	source[size] = '\0';

	fclose(file);

	return source;
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
		carrot_init();

		Parser parser;
		parser_init(&parser, source);
		Node *n = parser_parse(&parser);
		
		// ..........................
		// TODO: perform typechecking
		//...........................
		//
		//  carrot_typecheck(&n);
		//...........................

		Interpreter interpreter = create_interpreter();

		/* register builtin function */
		carrot_register_all_builtin_func(&interpreter);

		interpreter_interpret(&interpreter, n);

		interpreter_free(&interpreter);
		free_node(n);
		free(source);

		carrot_finalize();

		return 0;
	} else {
		return 1;
	}
}
