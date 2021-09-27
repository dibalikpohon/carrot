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
	return interpreter_visit(interpreter, node);
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
		case N_VALUE:
			return *node;
		default: exit(1);
	}
}

CarrotObj interpreter_visit_func_call(Interpreter *context, CarrotObj *node) {
	char *func_name = node->func_name;
	int idx = shgeti(context->sym_table, func_name);
	if (idx == -1) {
		printf("ERROR: function \"%s\" is undefined.\n", func_name);
		exit(1);
	}
	CarrotObj func_to_call = shget(context->sym_table, func_name);
	if (func_to_call.is_builtin) {
		node->func_interpreted_args = NULL;
		for (int i = 0; i < arrlen(node->func_args); i++) {
			CarrotObj itprtd = interpreter_visit(context, &node->func_args[i]);
			arrput(node->func_interpreted_args, itprtd);
		}
		return func_to_call.builtin_func(node->func_interpreted_args);
	} 

	// printf("ERROR: func_call: unknown error sorry lol");
	return carrot_null();
}

CarrotObj interpreter_visit_list(Interpreter *context, CarrotObj *node) {
	int list_item_count = arrlen(node->list_items);
	node->interpreted_list_items = NULL;
	for (int i = 0; i < list_item_count; i++) {
		CarrotObj obj = interpreter_visit(context, &node->list_items[i]);
		arrput(node->interpreted_list_items, obj);
	}

	// TODO: handle list type
	return carrot_null();
}

CarrotObj interpreter_visit_var_access(Interpreter *context, CarrotObj *node) {
	char *var_name = node->var_name;
	int idx = shgeti(context->sym_table, var_name);
	if (idx == -1) {
		printf("ERROR: \"%s\" is undefined.\n", var_name);
		exit(1);
	}

	return shget(context->sym_table, var_name);
}

CarrotObj interpreter_visit_var_def(Interpreter *context, CarrotObj *node) {
	CarrotObj value_obj;
	if (node->var_type == DT_STR) {
		value_obj = carrot_str(node->str_val);
	} else if (node->var_type == DT_INT) {
		value_obj = carrot_int(node->int_val);
	} else if (node->var_type == DT_FLOAT) {
		value_obj = carrot_float(node->float_val);
	} else if (node->var_type == DT_NULL) {
		value_obj = carrot_null();
	} else {
		printf("The data type for %s is not supported yet", node->var_name);
		exit(1);
	}
	shput(context->sym_table, node->var_name, value_obj);
	return carrot_null();
}
