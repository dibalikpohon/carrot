#include <stdio.h>
#include "../include/logutils.h"
#include "../include/interpreter.h"
#include "../lib/include/stb_ds.h"

SymTable *CARROT_TRACKING_ARR;

Interpreter create_interpreter() {
	Interpreter interpreter;
	interpreter.parent = NULL;
	interpreter.sym_table = NULL;
	//sh_new_strdup(interpreter.sym_table);
	return interpreter;
}

CarrotObj *interpreter_interpret(Interpreter *interpreter, Node *node) {
	return interpreter_visit(interpreter, node);
}

CarrotObj *interpreter_visit(Interpreter *context, Node *node) {
	switch (node->type) {
		case N_BINOP:
			return interpreter_visit_binop(context, node);
		case N_BLOCK:
			return interpreter_visit_block(context, node);
		case N_FUNC_CALL:
			return interpreter_visit_func_call(context, node);
		case N_IF:
			return interpreter_visit_if(context, node);
		case N_STATEMENTS:
			return interpreter_visit_statements(context, node);
		case N_VAR_ACCESS:
			return interpreter_visit_var_access(context, node);
		case N_VAR_ASSIGN:
			return interpreter_visit_var_assign(context, node);
		case N_VAR_DEF:
			return interpreter_visit_var_def(context, node);
		case N_LITERAL:
			return interpreter_visit_value(context, node);
		case N_FUNC_DEF: 
			return interpreter_visit_func_def(context, node);
		case N_ITER:
			return interpreter_visit_iter(context, node);
		case N_RETURN: 
			return interpreter_visit_return(context, node);
		// TODO: complete missing cases
		case N_STATEMENT:
		case N_NULL: 
		case N_UNKNOWN: 
			break;
	}
	printf("%s\n", "ERROR: Unknown node");
	exit(1);
}

CarrotObj *interpreter_visit_binop(Interpreter *context, Node *node) {
	CarrotObj *left = interpreter_visit(context, node->left);
	CarrotObj *right = interpreter_visit(context, node->right);
	if (strcmp(node->op_str, "+") == 0) {
		if (left->__add != NULL) return left->__add(left, right);
	} else if (strcmp(node->op_str, "-") == 0) {
		if (left->__subtract != NULL) return left->__subtract(left, right);
	} else if (strcmp(node->op_str, "*") == 0) {
		if (left->__mult != NULL) return left->__mult(left, right);
	} else if (strcmp(node->op_str, "/") == 0) {
		if (left->__div != NULL) return left->__div(left, right);
	} else if (strcmp(node->op_str, "==") == 0) {
		if (left->__ee != NULL) return left->__ee(left, right);
	} else if (strcmp(node->op_str, "!=") == 0) {
		if (left->__ne != NULL) return left->__ne(left, right);
	} else if (strcmp(node->op_str, ">") == 0) {
		if (left->__gt != NULL) return left->__gt(left, right);
	} else if (strcmp(node->op_str, "<") == 0) {
		if (left->__lt != NULL) return left->__lt(left, right);
	} else if (strcmp(node->op_str, ">=") == 0) {
		if (left->__ge != NULL) return left->__ge(left, right);
	} else if (strcmp(node->op_str, "<=") == 0) {
		if (left->__le != NULL) return left->__le(left, right);
	} else if (strcmp(node->op_str, "&&") == 0) {
		if (left->__and != NULL) return left->__and(left, right);
	} else if (strcmp(node->op_str, "||") == 0) {
		if (left->__or != NULL) return left->__or(left, right);
	}  

	printf("ERROR: operator %s is not defined for type %s and %s\n",
	       node->op_str, left->type_str, right->type_str);
	exit(1);
}

CarrotObj *interpreter_visit_block(Interpreter *context, Node *node) {
	for (int i = 0; i < arrlen(node->block_statements); i++) {
		interpreter_visit(context, node->block_statements[i]);
	}
	return carrot_null();
}

CarrotObj *interpreter_visit_func_call(Interpreter *context, Node *node) {
	char *func_name = node->func_name;
	CarrotObj *func_to_call = carrot_get_var(func_name, context);
	if (func_to_call == NULL) {
		char msg[255];
		sprintf(msg,
		        "Function \"%s\" is undefined. "
			"Make sure you define the function before calling it.",
			func_name);
		carrot_log_error(msg, "idklol", -1);
		exit(1);
	}

	if (func_to_call->is_builtin) {
		/* Case 1: the function being called is a builtin function */
		CarrotObj **func_args = NULL;
		for (int i = 0; i < arrlen(node->func_args); i++) {
			CarrotObj *itprtd = interpreter_visit(context, node->func_args[i]);
			arrput(func_args, itprtd);
		}
		CarrotObj *res = func_to_call->builtin_func(func_args);

		/* Clean up the evaluated arguments and its representation after
		 * built-in function call */
		//for (int i = 0; i < arrlen(func_args); i++) sdsfree(func_args[i].repr);
		if (func_args != NULL) arrfree(func_args);
		return res;
	} else {
		/* Case 2: the function being called is made inside carrot script */
		// -------
		//	Populate local variables within the function based on 
		//	argument names
		CarrotObj *return_value = NULL;
		Interpreter local_interpreter = create_interpreter();
		local_interpreter.parent = context;
		for (int i = 0; i < arrlen(func_to_call->func_arg_names); i++) {
			char *argname = func_to_call->func_arg_names[i];
			CarrotObj *argval = interpreter_visit(
				&local_interpreter,
				node->func_args[i]
			);
			shput(local_interpreter.sym_table, argname, argval);
		}
		//      Evaluate the function body (a list of statements)
		//
		for (int i = 0; i < arrlen(func_to_call->func_statements); i++) {
			interpreter_visit(&local_interpreter,
			                  func_to_call->func_statements[i]);
			
			// TODO: check if func_to_call->func_statements[i] node is
			// a N_RETURN node, and return accordingly
			Node *stmt = func_to_call->func_statements[i];
			if (stmt->type == N_RETURN) {
				return_value = interpreter_visit(&local_interpreter,
						                 stmt);
				break;
			}
		}

		//      End the local variable lifetime, except if it refers to
		//      the return value object
		int len = shlen(local_interpreter.sym_table);
		for (int i = 0; i < len; i++) {
			/* if return value obj also belongs to local sym_table,
			 * detach it first so it is not wiped and persists after
			 * leaving this function. It will still be tracked by
			 * the global tracker */
			if (local_interpreter.sym_table[i].value == return_value) {
				shdel(local_interpreter.sym_table,
				      local_interpreter.sym_table[i].key);
			}
		}
		interpreter_free(&local_interpreter);
		if (return_value==NULL) return carrot_null();
		return return_value;
	}
}

CarrotObj *interpreter_visit_func_def(Interpreter *context, Node *node) {
	CarrotObj *function = carrot_obj_allocate();
	function->func_statements = node->func_statements;
	strcpy(function->func_name, node->func_name);
	for (int i = 0; i < arrlen(node->func_params); i++) {
		arrput(function->func_arg_names, node->func_params[i]->param_name);
	}
	shput(context->sym_table, node->func_name, function);
	return carrot_null();
}

CarrotObj *interpreter_visit_if(Interpreter *context, Node *node) {
	int found_true = 0;
	for (int i = 0; i < arrlen(node->conditions); i++) {
		if (interpreter_visit(context, node->conditions[i])->bool_val) {
			interpreter_visit(context, node->if_blocks[i]);
			found_true = 1;
			break;
		}
	}

	/* if no condition is true, and we have else block, then execute 
	 * the else block */
	if (!found_true) {
		if (node->else_block != NULL) 
			interpreter_visit(context, node->else_block);
	}
	return carrot_null();
}

CarrotObj *interpreter_visit_iter(Interpreter *context, Node *node) {
	CarrotObj *iterable = interpreter_visit(context, node->iterable);
	int iterable_len = arrlen(iterable->list_items);
	char *loop_iterator_var_name = node->loop_iterator_var_name;
	char *loop_index_var_name = node->loop_index_var_name;
	Interpreter local_interpreter = create_interpreter();
	local_interpreter.parent = context;

	for (int i = 0; i < iterable_len; i++) {
		shput(local_interpreter.sym_table,
		      loop_iterator_var_name,
		      iterable->list_items[i]);
		if (node->loop_with_index)
			shput(local_interpreter.sym_table,
			      loop_index_var_name,
			      carrot_int(i));
		for (int j = 0; j < arrlen(node->loop_statements); j++)
			interpreter_visit(&local_interpreter,
				          node->loop_statements[j]);
	}
	/* TODO: free local variables */
	interpreter_free(&local_interpreter);
	return carrot_null();
}

CarrotObj *interpreter_visit_return(Interpreter *context, Node *node) {
	return interpreter_visit(context, node->return_value);
}

CarrotObj *interpreter_visit_statements(Interpreter *context, Node *node) {
	int list_item_count = arrlen(node->list_items);

	CarrotObj **list_items = NULL;
	for (int i = 0; i < list_item_count; i++) {
		CarrotObj *item = interpreter_visit(context, node->list_items[i]);
		arrput(list_items, item);
	}

	return carrot_list(list_items);
}

CarrotObj *interpreter_visit_value(Interpreter *context, Node *node) {
	if (node->var_type == DT_STR) {
		return carrot_str(node->value_token.text);
	} else if (node->var_type == DT_INT) {
		return carrot_int(node->int_val);
	} else if (node->var_type == DT_FLOAT) {
		return carrot_float(node->float_val);
	} else if (node->var_type == DT_BOOL) {
		return carrot_bool(node->bool_val);
	} else if (node->var_type == DT_NULL) {
		return carrot_null(node->bool_val);
	} else if (node->var_type == DT_LIST) {
		CarrotObj **list_items = NULL;
		for (int i = 0; i < arrlen(node->list_items); i++) {
			arrput(list_items,
			       interpreter_visit(context, node->list_items[i]));
		}
		return carrot_list(list_items);
	} else {
		printf("The data type for \"%s\" is not supported yet", node->value_token.text);
		exit(1);
	}

}

CarrotObj *interpreter_visit_var_access(Interpreter *context, Node *node) {
	char *var_name = node->var_name;
	CarrotObj *obj = carrot_get_var(var_name, context);

	if (obj == NULL) {
		char msg[255];
		sprintf(msg,
		        "You are trying to access variable \"%s\", while it is undefined. "
			"Have you defined it before?",
			var_name);
		carrot_log_error(msg, "idklol", -1);
		exit(1);
	}

	return obj;
}

CarrotObj *interpreter_visit_var_assign(Interpreter *context, Node *node) {
	CarrotObj *existing_var_content = shget(context->sym_table, node->var_name);
	if (existing_var_content != NULL) {
		/* remove existing_var_content from context's local symbol table and
		 * global tracker */
		shdel(context->sym_table, node->var_name);

		/* TODO Remove the existing variable content itself */
		// shdel(CARROT_TRACKING_ARR, existing_var_content->hash);
	}
	CarrotObj *var_content = interpreter_visit(context, node->var_node);
	shput(context->sym_table, node->var_name, var_content);
	return var_content;
}

CarrotObj *interpreter_visit_var_def(Interpreter *context, Node *node) {
	if (shget(context->sym_table, node->var_name) != NULL) {
		printf("ERROR: variable redefinition in the same scope: %s\n", 
		       node->var_name);
		exit(1);
	}
	CarrotObj *var_content = interpreter_visit(context, node->var_node);
	shput(context->sym_table, node->var_name, var_content);

	return var_content;
}

CarrotObj *__int_add(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->int_val + other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->int_val + other->float_val);
	}
	printf("ERROR: Cannot perform addition on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_subtract(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->int_val - other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->int_val - other->float_val);
	}
	printf("ERROR: Cannot perform subtraction on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_mult(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->int_val * other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->int_val * other->float_val);
	}
	printf("ERROR: Cannot perform multiplication on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_div(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->int_val / other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float((float)self->int_val / other->float_val);
	}
	printf("ERROR: Cannot perform division on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_ee(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val == other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val == other->float_val);
	}
	printf("ERROR: Cannot perform \"equal to\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_ne(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val != other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val != other->int_val);
	}
	printf("ERROR: Cannot perform \"not equal to\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_gt(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val > other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val > other->float_val);
	}
	printf("ERROR: Cannot perform \">\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_lt(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val < other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val < other->float_val);
	}
	printf("ERROR: Cannot perform \"<\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_ge(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val >= other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val >= other->float_val);
	}
	printf("ERROR: Cannot perform \">=\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__int_le(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->int_val <= other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->int_val <= other->float_val);
	}
	printf("ERROR: Cannot perform \"<=\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_add(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->float_val + (int) other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->float_val + other->float_val);
	}

	printf("ERROR: Cannot perform addition on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_subtract(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->float_val - (float) other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->float_val - other->float_val);
	}

	printf("ERROR: Cannot perform subtraction on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_mult(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_int(self->float_val * (float) other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->float_val * other->float_val);
	}

	printf("ERROR: Cannot perform multiplication on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_div(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_float(self->float_val / (float)other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_float(self->float_val / other->float_val);
	}

	printf("ERROR: Cannot perform division on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_ee(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val == other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val == other->float_val);
	}
	printf("ERROR: Cannot perform \"==\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_ne(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val != other->float_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val != other->float_val);
	}
	printf("ERROR: Cannot perform \"!=\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_gt(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val > other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val > other->float_val);
	}
	printf("ERROR: Cannot perform \">\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_lt(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val < other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val < other->float_val);
	}
	printf("ERROR: Cannot perform \"<\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_ge(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val >= other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val >= other->float_val);
	}
	printf("ERROR: Cannot perform \">=\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__float_le(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "int") == 0) {
		return carrot_bool(self->float_val <= other->int_val);
	} else if (strcmp(other->type_str, "float") == 0) {
		return carrot_bool(self->float_val <= other->float_val);
	}
	printf("ERROR: Cannot perform \"<=\" comparison on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__bool_and(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "bool") == 0) {
		return carrot_bool(self->bool_val && other->bool_val);
	} 
	printf("ERROR: Cannot use \"&&\" on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__bool_or(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "bool") == 0) {
		return carrot_bool(self->bool_val || other->bool_val);
	} 
	printf("ERROR: Cannot use \"||\" on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__str_ee(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "str") == 0) {
		int ee = sdscmp(self->repr, other->repr) == 0;
		return carrot_bool(ee);
	}
	printf("ERROR: Cannot use \"equal to\" on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *__str_ne(CarrotObj *self, CarrotObj *other) {
	if (strcmp(other->type_str, "str") == 0) {
		int ne = sdscmp(self->repr, other->repr) != 0;
		return carrot_bool(ne);
	}
	printf("ERROR: Cannot use \"not equal to\" on %s and %s\n", self->type_str, other->type_str);
	exit(1);
}

CarrotObj *carrot_obj_allocate() {
	CarrotObj *obj = calloc(1, sizeof(CarrotObj));

	char *hash = calloc(1, 64);
	sprintf(hash, "%p", (void *) obj);
	obj->hash = hash;
	shput(CARROT_TRACKING_ARR, hash, obj);
	return obj;
}

CarrotObj *carrot_noop() {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_NULL;
	return obj;
}

CarrotObj *carrot_null() {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_NULL;
	obj->type_str = sdsnew("null");
	obj->repr = sdsnew("null");
	return obj;
}

CarrotObj *carrot_get_var(char *var_name, Interpreter *context) {
	/* look up the variable based on name. If it is not found 
	 * in the context's sym_table, then recursicely look up
	 * on context's parent interpreter */
	CarrotObj *obj = shget(context->sym_table, var_name);
	if (obj != NULL)
		return obj;

	if (obj == NULL && context->parent != NULL) {
		return carrot_get_var(var_name, context->parent);
	}

	return NULL;
}

CarrotObj *carrot_bool(int bool_val) {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_BOOL;
	obj->bool_val = bool_val;
	obj->type_str = sdsnew("bool");
	obj->repr = sdscatprintf(sdsempty(),
			         "%s",
				 bool_val == 1 ? "true" : "false");
	obj->__and = __bool_and;
	obj->__or = __bool_or;
	return obj;
}

CarrotObj *carrot_int(int int_val) {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_INT;
	obj->int_val = int_val;
	obj->type_str = sdsnew("int");
	obj->repr = sdscatprintf(sdsempty(), "%d", int_val);
	obj->__add = __int_add;
	obj->__subtract = __int_subtract;
	obj->__mult = __int_mult;
	obj->__div = __int_div;
	obj->__ee = __int_ee;
	obj->__ne = __int_ne;
	obj->__ge = __int_ge;
	obj->__le = __int_le;
	obj->__gt = __int_gt;
	obj->__lt = __int_lt;
	return obj;
}

CarrotObj *carrot_list(CarrotObj **list_items) {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_LIST;
	obj->list_items = list_items;
	obj->type_str = sdsnew("list");
	sds repr = sdsnew("[");
	for (int i = 0; i < arrlen(list_items); i++) {
		if (list_items[i]->type == CARROT_STR) {
			repr = sdscat(repr, "\"");
			repr = sdscatsds(repr, list_items[i]->repr);
			repr = sdscat(repr, "\"");
		} else {
			repr = sdscatsds(repr, list_items[i]->repr);
		}

		if (i < arrlen(list_items) - 1)
			repr = sdscat(repr, ", ");
	}
	repr = sdscat(repr, "]");
	obj->repr = repr;
	return obj;
}

CarrotObj *carrot_float(float float_val) {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_FLOAT;
	obj->float_val = float_val;
	obj->type_str = sdsnew("float");
	obj->repr = sdscatprintf(sdsempty(), "%f", float_val);
	obj->__add = __float_add;
	obj->__subtract = __float_subtract;
	obj->__mult = __float_mult;
	obj->__div = __float_div;
	obj->__ee = __float_ee;
	obj->__ne = __float_ne;
	obj->__ge = __float_ge;
	obj->__le = __float_le;
	obj->__gt = __float_gt;
	obj->__lt = __float_lt;
	return obj;
}

CarrotObj *carrot_str(char *str_val) {
	CarrotObj *obj = carrot_obj_allocate();
	obj->type = CARROT_STR;
	obj->type_str = sdsnew("str");
	obj->repr = sdsnew(str_val);
	obj->__ee = __str_ee;
	obj->__ne = __str_ne;
	return obj;
}

CarrotObj *carrot_eval(Interpreter *interpreter, char *source) {
	Parser parser;
	parser_init(&parser, source);
	Node *n = parser_parse(&parser);

	CarrotObj *res = interpreter_interpret(interpreter, n);

	free_node(n);
	return res;
}

void carrot_finalize() {
	/* Frees remaining CarrotObj's in heap.
	 * Call this in the very end of main function */
	int len = shlen(CARROT_TRACKING_ARR);
	for (int i = 0; i < len; i++) {
		CarrotObj *obj = CARROT_TRACKING_ARR[i].value;
		shdel(CARROT_TRACKING_ARR, obj->hash);
		carrot_free(obj);
	}

	shfree(CARROT_TRACKING_ARR);
}

void carrot_free(CarrotObj *root) {
	/* It only frees the members of root. If root member is a pointer
	 * to array of allocated objects, it should be freed manually somewhere
	 * else */
	if (arrlen(root->list_items) >= 0) arrfree(root->list_items);
	if (arrlen(root->func_arg_names) >= 0) arrfree(root->func_arg_names);
	free(root->hash);
	sdsfree(root->repr);
	sdsfree(root->type_str);
	free(root);
}

void carrot_init() {
	/* Initialize hashtable that tracks CarrotObj's allocated in heap */
	CARROT_TRACKING_ARR = NULL;
	sh_new_strdup(CARROT_TRACKING_ARR);
}

void interpreter_free(Interpreter *interpreter) {
	/* Frees the members of interpreter struct as well as
	 * the content of symbol table. */
	int len = shlen(interpreter->sym_table);
	for (int i = 0; i < len; i++) {
		CarrotObj* obj = interpreter->sym_table[i].value;
		char *hash = obj->hash;
		char *key = interpreter->sym_table[i].key;
		shdel(CARROT_TRACKING_ARR, hash);
		shdel(interpreter->sym_table, key);
		carrot_free(obj);
	}
	shfree(interpreter->sym_table);
}
