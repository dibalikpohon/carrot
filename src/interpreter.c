#include <stdio.h>
#include "../include/interpreter.h"
#include "../lib/include/stb_ds.h"

Interpreter create_interpreter() {
	Interpreter interpreter;
	interpreter.parent = NULL;
	interpreter.sym_table = NULL;
	return interpreter;
}

void interpreter_interpret(Interpreter *interpreter, CarrotObj *node) {
	interpreter_visit(interpreter, node);
}

void interpreter_visit(Interpreter *context, CarrotObj *node) {
	switch (node->type) {
		case N_FUNC_CALL:
			interpreter_visit_func_call(context, node);
			break;
		case N_LIST:
			interpreter_visit_list(context, node);
			break;
		case N_VAR_DEF:
			interpreter_visit_var_def(context, node);
			break;
		default: exit(1);
	}
}

void interpreter_visit_func_call(Interpreter *context, CarrotObj *node) {
	char *func_name = node->func_name;
	CarrotObj func_to_call = shget(context->sym_table, func_name);
	if (func_to_call.is_builtin) {
		func_to_call.builtin_func(func_to_call.func_args);
	}
}

void interpreter_visit_list(Interpreter *context, CarrotObj *node) {
	int list_item_count = arrlen(node->list_items);
	for (int i = 0; i < list_item_count; i++) {
		interpreter_visit(context, &node->list_items[i]);
	}
}

void interpreter_visit_var_def(Interpreter *context, CarrotObj *node) {
	//printf("%s\n", node->var_name);
}
