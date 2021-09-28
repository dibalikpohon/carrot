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
		case N_STATEMENTS:
			return interpreter_visit_statements(context, node);
		case N_VAR_DEF:
			return interpreter_visit_var_def(context, node);
		case N_VAR_ACCESS:
			return interpreter_visit_var_access(context, node);
		case N_LITERAL:
			return interpreter_visit_value(context, node);
		// TODO: complete missing cases
		case N_STATEMENT:
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
		CarrotObj res = func_to_call.builtin_func(func_args);

		/* Clean up the evaluated arguments and its representation after
		 * built-in function call */
		for (int i = 0; i < arrlen(func_args); i++) sdsfree(func_args[i].repr);
		if (func_args != NULL) arrfree(func_args);
		return res;
	} 

	return carrot_null();
}

CarrotObj interpreter_visit_statements(Interpreter *context, Node *node) {
	int list_item_count = arrlen(node->list_items);

	CarrotObj *list_items = NULL;
	for (int i = 0; i < list_item_count; i++) {
		CarrotObj item = interpreter_visit(context, &node->list_items[i]);
		arrput(list_items, item);
	}

	return carrot_list(list_items);
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
	} else if (node->var_type == DT_LIST) {
		CarrotObj *list_items = NULL;
		value_obj = carrot_list(list_items);
	} else {
		printf("The data type for \"%s\" is not supported yet", node->value_token.text);
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
	} else if (node->var_type == DT_LIST) {
		CarrotObj *list_items = NULL;
		for (int i = 0; i < arrlen(node->list_items); i++) {
			arrput(list_items,
			       interpreter_visit(context, &node->list_items[i]));
		}
		value_obj = carrot_list(list_items);
	} else {
		printf("The data type for %s is not supported yet", node->var_name);
		exit(1);
	}
	shput(context->sym_table, node->var_name, value_obj);
	return carrot_null();
}

CarrotObj carrot_noop() {
	CarrotObj obj;
	obj.type = CARROT_NULL;
	return obj;
}

CarrotObj carrot_null() {
	CarrotObj obj;
	obj.type = CARROT_NULL;
	sprintf(obj.type_str, "%s", "null");
	obj.repr = sdsnew("null");
	return obj;
}

CarrotObj carrot_int(int int_val) {
	CarrotObj obj;
	obj.type = CARROT_INT;
	sprintf(obj.type_str, "%s", "int");
	char repr[255];
	sprintf(repr, "%d", int_val);
	obj.repr = sdsnew(repr);
	return obj;
}

CarrotObj carrot_list(CarrotObj *list_items) {
	CarrotObj obj;
	obj.type = CARROT_LIST;
	obj.list_items = list_items;
	sprintf(obj.type_str, "%s", "list");
	sds repr = sdsnew("[");
	for (int i = 0; i < arrlen(list_items); i++) {
		repr = sdscatsds(repr, list_items[i].repr);
		repr = sdscat(repr, ", ");
	}
	repr = sdscat(repr, "]");
	obj.repr = repr;
	return obj;
}

CarrotObj carrot_float(float float_val) {
	CarrotObj obj;
	obj.type = CARROT_FLOAT;
	sprintf(obj.type_str, "%s", "float");

	char repr[255];
	sprintf(repr, "%f", float_val);
	obj.repr = sdsnew(repr);
	return obj;
}

CarrotObj carrot_str(char *str_val) {
	CarrotObj obj;
	obj.type = CARROT_STR;
	sprintf(obj.type_str, "%s", "str");
	obj.repr = sdsnew(str_val);
	return obj;
}

void carrot_free(CarrotObj *root) {
	if (root->type == CARROT_LIST) {
		for (int i = 0; i < arrlen(root->list_items); i++) {
			carrot_free(&root->list_items[i]);
		}
		if (root->list_items != NULL) arrfree(root->list_items);
		sdsfree(root->repr);
	} else { 
		sdsfree(root->repr);
	} 
}

void interpreter_free(Interpreter *interpreter) {
	for (int i = 0; i < shlen(interpreter->sym_table); i++) {
		//shfree(interpreter->sym_table[i].value.repr);
		//carrot_free(&interpreter->sym_table[i].value);
	}
	shfree(interpreter->sym_table);
}

