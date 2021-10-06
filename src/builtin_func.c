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

CarrotObj *carrot_func_range(CarrotObj **args) {
	if (arrlen(args) < 1 || arrlen(args) > 3) {
		printf("ERROR: Function 'range' accepts 1, 2 or 3 arguments.\n");
		printf("       Usage: `range(upper_bound) or range(lower_bound, upper_bound)`\n");
		printf("       or range(lower_bound, upper_bound, step).\n");
		exit(1);
	}

	int all_int = 1;
	for (int i = 0; i < arrlen(args); i++) {
		all_int = all_int && (args[i]->type == CARROT_INT);
	}
	if (!all_int) {
		printf("ERROR: All arguments for `range` should be of `int` type\n");
		exit(1);
	}

	CarrotObj **list_items = NULL;

	if (arrlen(args) == 1) {
		for (int i = 0; i < args[0]->int_val; i++) {
			arrput(list_items, carrot_int(i));
		}
		return carrot_list(list_items);
	}

	int step = 1;
	if (arrlen(args) == 3) {
		step = args[2]->int_val;
	}

	int low = args[0]->int_val;
	int high = args[1]->int_val;
	for (int i = low; i < high; i += step) {
		arrput(list_items, carrot_int(i));
	}
	return carrot_list(list_items);
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
	carrot_register_builtin_func("range",
				     carrot_func_range,
				     interpreter);
	carrot_register_builtin_func("type",
				     carrot_func_type,
				     interpreter);
}
