#include <stdio.h>
#include "../include/interpreter.h"
#include "../include/builtin_func.h"
#include "../lib/include/stb_ds.h"

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
	builtin_func->repr = sdsnew("function");
	strcpy(builtin_func->func_name, name);
	shput(interpreter->sym_table, name, builtin_func);
}

void carrot_register_all_builtin_func(Interpreter *interpreter) {
	carrot_register_builtin_func("print",
				     &carrot_func_print,
				     interpreter);
	carrot_register_builtin_func("println",
				     &carrot_func_println,
				     interpreter);
	carrot_register_builtin_func("type",
				     carrot_func_type,
				     interpreter);
}
