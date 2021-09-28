#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../include/parser.h"
#include "../lib/include/sds.h"

typedef enum {
	CARROT_STR, CARROT_INT, CARROT_FLOAT, CARROT_LIST, CARROT_NULL,
	CARROT_FUNCTION,
} carrot_dtype_t;

typedef struct CarrotObj_t {
	carrot_dtype_t      type;
	char                type_str[255];

	struct CarrotObj_t  *list_items;

	/* Function call */
	struct CarrotObj_t  (*builtin_func)(struct CarrotObj_t *args);
	int                 is_builtin;
	char                func_name[255];
	struct CarrotObj_t  *func_args;

	/* Common properties */
	sds                 repr;
} CarrotObj;


typedef struct SymTable_t {
	char *key;
	CarrotObj value;
} SymTable;

typedef struct INTERPRETER {
	SymTable *sym_table;
	struct INTERPRETER *parent;
} Interpreter;

Interpreter create_interpreter();

CarrotObj interpreter_init(Interpreter *interpreter, Node *node);
CarrotObj interpreter_interpret(Interpreter *interpreter, Node *node);
CarrotObj interpreter_visit(Interpreter *context, Node *node);
CarrotObj interpreter_visit_func_call(Interpreter *context, Node *node);
CarrotObj interpreter_visit_list(Interpreter *context, Node *node);
CarrotObj interpreter_visit_statements(Interpreter *context, Node *node);
CarrotObj interpreter_visit_value(Interpreter *context, Node *node);
CarrotObj interpreter_visit_var_access(Interpreter *context, Node *node);
CarrotObj interpreter_visit_var_def(Interpreter *context, Node *node);

CarrotObj carrot_noop();
CarrotObj carrot_null();
CarrotObj carrot_int(int int_val);
CarrotObj carrot_list(CarrotObj *list_items);
CarrotObj carrot_float(float float_val);
CarrotObj carrot_str(char *str_val);

void carrot_free(CarrotObj *root);
void interpreter_free(Interpreter *interpreter);

#endif
