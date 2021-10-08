#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../include/parser.h"
#include "../lib/include/sds.h"

typedef enum {
	CARROT_STR, CARROT_INT, CARROT_FLOAT, CARROT_BOOL, CARROT_LIST,
	CARROT_NULL, CARROT_FUNCTION,
} carrot_dtype_t;

typedef struct CarrotObj_t {
	carrot_dtype_t      type;
	sds                 type_str;

	struct CarrotObj_t  **list_items;

	/* Value properties */
	struct CarrotObj_t  *self;
	int                 bool_val;
	int                 int_val;
	float               float_val;
	sds                 str_val;

	/* Function call object properties */
	struct CarrotObj_t  *(*builtin_func)(struct CarrotObj_t **args);
	int                 is_builtin;
	char                func_name[255];   // shared with function def object
	char                **func_arg_names; // shared with function def object

	/* Function definition object properties 
	 * No need to free this inside interpreter_free() */
	Node                **func_statements;

	/* Common properties */
	sds                 repr;
	char	            *hash;

	/* Object builtin methods */
	struct CarrotObj_t  **members;
	struct CarrotObj_t  *(*__plus)(struct CarrotObj_t *self);
	struct CarrotObj_t  *(*__negate)(struct CarrotObj_t *self);
	struct CarrotObj_t  *(*__not)(struct CarrotObj_t *self);
	struct CarrotObj_t  *(*__add)(struct CarrotObj_t *self, 
			              struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__subtract)(struct CarrotObj_t *self,
	                                   struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__mult)(struct CarrotObj_t *self,
	                               struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__div)(struct CarrotObj_t *self,
	                              struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__ee)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__ne)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__ge)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__le)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__gt)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__lt)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__and)(struct CarrotObj_t *self,
	                              struct CarrotObj_t *other);
	struct CarrotObj_t  *(*__or)(struct CarrotObj_t *self,
	                             struct CarrotObj_t *other);
} CarrotObj;


typedef struct SymTable_t {
	char *key;
	CarrotObj *value;
} SymTable;

typedef struct INTERPRETER {
	SymTable *sym_table;
	struct INTERPRETER *parent;
} Interpreter;


Interpreter create_interpreter();

CarrotObj *interpreter_init(Interpreter *interpreter, Node *node);
CarrotObj *interpreter_interpret(Interpreter *interpreter, Node *node);
CarrotObj *interpreter_visit(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_binop(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_block(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_func_call(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_func_def(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_if(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_iter(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_list(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_return(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_statements(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_unop(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_value(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_var_access(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_var_assign(Interpreter *context, Node *node);
CarrotObj *interpreter_visit_var_def(Interpreter *context, Node *node);

CarrotObj *carrot_obj_allocate();
CarrotObj *carrot_noop();
CarrotObj *carrot_null();
CarrotObj *carrot_get_var(char *var_name, Interpreter *context);
CarrotObj *carrot_bool(int bool_val);
CarrotObj *carrot_int(int int_val);
CarrotObj *carrot_list(CarrotObj **list_items);
CarrotObj *carrot_float(float float_val);
CarrotObj *carrot_str(char *str_val);

CarrotObj *carrot_eval(Interpreter *interpreter, char *source);

void carrot_finalize();
void carrot_free(CarrotObj *root);
void carrot_init();
void interpreter_free(Interpreter *interpreter);

#endif
