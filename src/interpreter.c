#include <stdio.h>
#include "../include/logutils.h"
#include "../include/interpreter.h"
#include "../lib/include/stb_ds.h"

Interpreter create_interpreter() {
	Interpreter interpreter;
	interpreter.parent = NULL;
	interpreter.sym_table = NULL;
	return interpreter;
}

CarrotObj interpreter_interpret(Interpreter *interpreter, Node *node) {
	return interpreter_visit(interpreter, node);
}

CarrotObj interpreter_visit(Interpreter *context, Node *node) {
	switch (node->type) {
		case N_FUNC_CALL:
			return interpreter_visit_func_call(context, node);
		case N_LIST:
			return interpreter_visit_list(context, node);
		case N_VAR_DEF:
			return interpreter_visit_var_def(context, node);
		case N_VAR_ACCESS:
			return interpreter_visit_var_access(context, node);
		case N_LITERAL:
			return interpreter_visit_value(context, node);
		// TODO: complete missing cases
		case N_STATEMENT:
		case N_STATEMENTS: 
		case N_NULL: 
		case N_FUNC_DEF: 
			break;
	}
	printf("%s\n", "ERROR: Unknown node");
	exit(1);
}

CarrotObj interpreter_visit_func_call(Interpreter *context, Node *node) {
	char *func_name = node->func_name;
	int idx = shgeti(context->sym_table, func_name);
	if (idx == -1) {
		char msg[255];
		sprintf(msg,
		        "Function \"%s\" is undefined. "
			"Make sure you define the function before calling it.",
			func_name);
		carrot_log_error(msg);
		exit(1);
	}

	CarrotObj func_to_call = shget(context->sym_table, func_name);
	if (func_to_call.is_builtin) {
		CarrotObj *func_args = NULL;
		for (int i = 0; i < arrlen(node->func_args); i++) {
			CarrotObj itprtd = interpreter_visit(context, &node->func_args[i]);
			arrput(func_args, itprtd);
		}
		return func_to_call.builtin_func(func_args);
	} 

	// printf("ERROR: func_call: unknown error sorry lol");
	return carrot_null();
}

CarrotObj interpreter_visit_list(Interpreter *context, Node *node) {
	int list_item_count = arrlen(node->list_items);

	CarrotObj *list_items = NULL;
	for (int i = 0; i < list_item_count; i++) {
		CarrotObj item = interpreter_visit(context, &node->list_items[i]);
		arrput(list_items, item);
	}
	CarrotObj list;
	list.list_items = list_items;


	//// TODO: handle list type
	return carrot_null();
}

CarrotObj interpreter_visit_value(Interpreter *context, Node *node) {
	CarrotObj value_obj;
	if (node->var_type == DT_STR) {
		value_obj = carrot_str(node->value_token.text);
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

	return value_obj;
}

CarrotObj interpreter_visit_var_access(Interpreter *context, Node *node) {
	char *var_name = node->var_name;
	int idx = shgeti(context->sym_table, var_name);
	if (idx == -1) {
		char msg[255];
		sprintf(msg,
		        "You are trying to access variable \"%s\", while it is undefined. "
			"Have you defined it before?",
			var_name);
		carrot_log_error(msg);
		exit(1);
	}

	return shget(context->sym_table, var_name);
}

CarrotObj interpreter_visit_var_def(Interpreter *context, Node *node) {
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

CarrotObj carrot_null() {
	CarrotObj obj;
	obj.type = CARROT_NULL;
	sprintf(obj.type_str, "%s", "null");
	sprintf(obj.repr, "%s", "null");
	return obj;
}

CarrotObj carrot_int(int int_val) {
	CarrotObj obj;
	obj.type = CARROT_INT;
	sprintf(obj.type_str, "%s", "int");
	sprintf(obj.repr, "%d", int_val);
	return obj;
}

CarrotObj carrot_float(float float_val) {
	CarrotObj obj;
	obj.type = CARROT_FLOAT;
	sprintf(obj.type_str, "%s", "float");
	sprintf(obj.repr, "%f", float_val);
	return obj;
}

CarrotObj carrot_str(char *str_val) {
	CarrotObj obj;
	obj.type = CARROT_STR;
	sprintf(obj.type_str, "%s", "str");
	sprintf(obj.repr, "%s", str_val);
	return obj;
}
