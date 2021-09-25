#include <stdio.h>
#include "../include/interpreter.h"
#include "../lib/include/stb_ds.h"

Interpreter create_interpreter() {
	Interpreter interpreter;
	interpreter.parent = NULL;
	interpreter.sym_table = NULL;
	return interpreter;
}

CarrotObj interpreter_interpret(Interpreter *interpreter, CarrotObj *node) {
	interpreter_visit(interpreter, node);
}

CarrotObj interpreter_visit(Interpreter *context, CarrotObj *node) {
	switch (node->type) {
		case N_FUNC_CALL:
			return interpreter_visit_func_call(context, node);
		case N_LIST:
			return interpreter_visit_list(context, node);
		case N_VAR_DEF:
			return interpreter_visit_var_def(context, node);
		case N_VAR_ACCESS:
			return interpreter_visit_var_access(context, node);
		default: exit(1);
	}
}

CarrotObj interpreter_visit_func_call(Interpreter *context, CarrotObj *node) {
	char *func_name = node->func_name;
	CarrotObj func_to_call = shget(context->sym_table, func_name);
	if (func_to_call.is_builtin) {
		func_to_call.builtin_func(node->func_args);
	}
}

CarrotObj interpreter_visit_list(Interpreter *context, CarrotObj *node) {
	int list_item_count = arrlen(node->list_items);
	for (int i = 0; i < list_item_count; i++) {
		interpreter_visit(context, &node->list_items[i]);
	}
}

CarrotObj interpreter_visit_var_access(Interpreter *context, CarrotObj *node) {
	char *var_name = node->var_name;
	int idx = shgeti(context->sym_table, var_name);
	if (idx == -1) {
		printf("Index: \"%d\" \n", idx);
		printf("Error: \"%s\" is undefined.\n", var_name);
	}
}

CarrotObj interpreter_visit_var_def(Interpreter *context, CarrotObj *node) {
	//printf("%s\n", node->var_name);
	shput(context->sym_table, node->var_name, *node);
	return carrot_null();
}
