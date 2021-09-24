#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../include/parser.h"

typedef struct SymTable_t {
	char *key;
	CarrotObj value;
} SymTable;

typedef struct INTERPRETER {
	SymTable *sym_table;
	struct INTERPRETER *parent;
} Interpreter;

Interpreter create_interpreter();

void interpreter_init(Interpreter *interpreter, CarrotObj *node);
void interpreter_interpret(Interpreter *interpreter, CarrotObj *node);
void interpreter_visit(Interpreter *context, CarrotObj *node);
void interpreter_visit_func_call(Interpreter *context, CarrotObj *node);
void interpreter_visit_list(Interpreter *context, CarrotObj *node);
void interpreter_visit_var_def(Interpreter *context, CarrotObj *node);

#endif
