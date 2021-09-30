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

	source = (char*) calloc(1, sizeof(char) * (size + 1));
	fread(source, 1, size, file);
	source[size] = '\0';

	fclose(file);

	return source;
}

CarrotObj *carrot_func_print(CarrotObj **args) {
	int argc = arrlen(args);
	for (int i = 0; i < argc; i++) {
		printf("%s", args[i]->repr);
	}
	return carrot_null();
}

CarrotObj *carrot_func_println(CarrotObj **args) {
	int argc = arrlen(args);
	for (int i = 0; i < argc; i++) {
		printf("%s", args[i]->repr);
	}
	printf("\n");
	return carrot_null();
}

CarrotObj *carrot_func_type(CarrotObj **args) {
	int argc = arrlen(args);
	if (argc != 1) {
		printf("ERROR: Function 'type' accepts exactly 1 arguments, but %d are passed.\n", argc);
		exit(1);
	}
	return carrot_str(args[0]->type_str);
}

void carrot_register_builtin_func(char *name,
		                  CarrotObj *(*func)(CarrotObj **args),
		                  Interpreter *interpreter) {
	CarrotObj *builtin_func = carrot_obj_allocate();
	builtin_func->type = CARROT_FUNCTION;
	builtin_func->builtin_func = func;
	builtin_func->is_builtin = 1;
	strcpy(builtin_func->func_name, name);
	shput(interpreter->sym_table, name, builtin_func);
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
		Node n = parser_parse(&parser);
		
		// ..........................
		// TODO: perform typechecking
		//...........................
		//
		//  carrot_typecheck(&n);
		//...........................


		Interpreter interpreter = create_interpreter();

		/* register builtin function */
		//carrot_register_builtin_func("print",
		//		             &carrot_func_print,
		//			     &interpreter);
		carrot_register_builtin_func("println",
				             &carrot_func_println,
					     &interpreter);
		//carrot_register_builtin_func("type",
		//		             &carrot_func_type,
		//			     &interpreter);

		CarrotObj *result = interpreter_interpret(&interpreter, &n);

		//carrot_free(result);

		free_node(&n);
		interpreter_free(&interpreter);
		free(source);

		carrot_finalize();

		return 0;
	} else {
		return 1;
	}
}
