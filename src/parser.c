#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/logutils.h"

#define STB_DS_IMPLEMENTATION
#include "../lib/include/stb_ds.h"

void free_node(Node *node) {
	if (node->type == N_STATEMENTS) {
		int len;
		len = arrlen(node->list_items);
		for (int i = 0; i < len; i++) {
			free_node(&node->list_items[i]);
		}
		if (node->list_items != NULL) arrfree(node->list_items);
	} else if (node->type == N_FUNC_CALL) {
		for (int i = 0; i < arrlen(node->func_args); i++) {
			free_node(&node->func_args[i]);
		}
		if (node->func_args != NULL) arrfree(node->func_args);
	} else if (node->type == N_VAR_DEF) {
		if (arrlen(node->list_items) >0 ) arrfree(node->list_items);
	}

}

Token parser_consume(Parser *parser) {
	Token token = parser->current_token;

	parser->i++;
	parser->current_token = parser->lexer.tokens[parser->i];
	return token;
}

void parser_init(Parser *parser, char *source) {
	lexer_init(&parser->lexer, source);
	lexer_lex(&parser->lexer);
	parser->i = 0;
	parser->current_token = parser->lexer.tokens[0];
	parser->node_cnt = 0;
}

Token parser_lookahed(Parser *parser) {
	return parser->lexer.tokens[parser->i + 1];
}

Node parser_parse(Parser *parser) {
	return parser_parse_script(parser);
}

Node parser_parse_arith(Parser *parser) {
	Node left = parser_parse_term(parser);

	while (parser->current_token.tok_kind == T_PLUS) {
	}

	return left;
}

Node parser_parse_atom(Parser *parser) {
	tok_kind_t kind = parser->current_token.tok_kind;

	if (kind == T_ID) {
		return parser_parse_identifier(parser);
	} else if (kind == T_STR ||
		   kind == T_INT ||
		   kind == T_FLOAT) {
		Node val_node;
		val_node.type = N_LITERAL;
		val_node.value_token = parser->current_token;
		parser_consume(parser);
		return val_node;
	} 
	printf("ERROR: literal or expression expected\n");
	exit(1);
}


Node parser_parse_call(Parser *parser) {
	Node atom = parser_parse_atom(parser);
	if (parser->current_token.tok_kind == T_LPAREN) {
	}
	return atom;
}

Node parser_parse_com(Parser *parser) {
	// 1) parse NOT
	// ...

	// left term
	Node left = parser_parse_arith(parser);

	// 2) parse ==, >, <, >=, <=
	// ...
	
	return left;
}

Node parser_parse_expression(Parser *parser) {
	Node left = parser_parse_com(parser);

	/* and, or, ... */

	return left;
}

Node parser_parse_factor(Parser *parser) {
	Node left = parser_parse_power(parser);
	return left;
}

Node parser_parse_identifier(Parser *parser) {
	/* 3 possibilities:
	 * - variable declaration with dtype
	 * - variable declaration
	 * - variable access
	 */
	Token id_token = parser_consume(parser);
	Token next_token = parser->current_token; //parser_consume(parser);

	if (strcmp(next_token.text, "as") == 0) {
		parser_consume(parser);
		/* Handle variable declaration */

		// holding data type
		Token data_type_token = parser_consume(parser);

		if (parser->current_token.tok_kind == T_EQUAL) {
			/* The variable is initialized with some value 
			 * since we found equal char
			 */
			parser_consume(parser);
			Token var_value_token = parser->lexer.tokens[parser->i];

			return parser_parse_variable_def(parser, 
							 id_token,
					                 data_type_token.text,
							 var_value_token,
							 1);

		} else {
			/* Otherwise, just return the node */
			Token var_value_token; // dummy var_value
			var_value_token.tok_kind = T_UNKNOWN;
			return parser_parse_variable_def(parser, 
							 id_token,
					                 data_type_token.text,
							 var_value_token,
							 0);
		}

	} else if (next_token.tok_kind == T_LPAREN) {
		parser_consume(parser);
		/* Handle function call */

		Node *args = NULL;

		if (parser->current_token.tok_kind == T_RPAREN) {
			// handle call without args
			parser_consume(parser);
		} else {
			// handle call with args
			Node arg = parser_parse_expression(parser);
			arrput(args, arg);
			while (parser->current_token.tok_kind == T_COMMA) {
				parser_consume(parser);
				Node arg = parser_parse_expression(parser);
				arrput(args, arg);
			}
			if (parser->current_token.tok_kind != T_RPAREN) {
				printf("ERROR: \")\" expected.\n");
			}
			
			// consume R_PAREN
			parser_consume(parser);
		}

		Node obj;
		obj.type = N_FUNC_CALL;
		obj.func_args = args;
		strcpy(obj.func_name, id_token.text);
		return obj;
	} else if (next_token.tok_kind == T_EQUAL) {
		/* Handle assignment */
	} else {
		/* Handle variable access */
		Node obj;
		obj.type = N_VAR_ACCESS;
		strcpy(obj.var_name, id_token.text);
		return obj;
	}

	printf("ERROR: Invalid syntax.\n");
	printf("%s.\n", parser->current_token.text);
	exit(1);
}

Node parser_parse_keyword(Parser *parser) {
	exit(1);	
}

Node parser_parse_list(Parser *parser) {
	Node obj;
	Node *list_items = NULL;
	if (parser->current_token.tok_kind == T_RBRACKET) {
		parser_consume(parser);
	} else {
		Node item = parser_parse_expression(parser);
		arrput(list_items, item);
		while (parser->current_token.tok_kind == T_COMMA) {
			parser_consume(parser);
			Node item = parser_parse_expression(parser);
			arrput(list_items, item);
		}

		if (parser->current_token.tok_kind != T_RBRACKET) {
			printf("ERROR: ] expected\n");
			exit(1);
		}
		parser_consume(parser);
	}
	obj.list_items = list_items;
	return obj;
}


Node parser_parse_power(Parser *parser) {
	Node left = parser_parse_call(parser);
	return left;
}

Node parser_parse_script(Parser *parser) {
	Node *statements = NULL;
	while (parser->current_token.tok_kind != T_EOF) {
		Node stmt = parser_parse_statement(parser);
		arrput(statements, stmt);
	}

	Node list_node;
	list_node.type = N_STATEMENTS;
	list_node.list_items = statements;
	return list_node;
}

Node parser_parse_statement(Parser *parser) {
	return parser_parse_expression(parser);
}

Node parser_parse_statements(Parser *parser){
	Node *statements = NULL;

	Node statement = parser_parse_statement(parser);
	arrput(statements, statement);

	Node n;
	n.statements = statements;
	n.type = N_STATEMENTS;
	parser->nodes[parser->node_cnt++] = n;

	return n;
}

Node parser_parse_term(Parser *parser) {
	Node left = parser_parse_factor(parser);

	while (parser->current_token.tok_kind == T_MULT) {
	}

	return left;
}

Node parser_parse_value(Parser *parser) {
	exit(1);
}

Node parser_parse_variable_def(Parser *parser,
		                    Token id_token,
		                    char *var_type_str, 
				    Token var_value_token,
				    int initialized) {
	Node obj;
	obj.type = N_VAR_DEF;
	strcpy(obj.var_name, id_token.text);
	
	if (strcmp(var_type_str, "str") == 0) {
		obj.var_type = DT_STR;
		if (initialized) strcpy(obj.str_val, var_value_token.text);
		else strcpy(obj.str_val, "");
	} else if (strcmp(var_type_str, "int") == 0) {
		obj.var_type = DT_INT;
		if (initialized) obj.int_val = atoi(var_value_token.text);
		else obj.int_val = 0;
	} else if (strcmp(var_type_str, "float") == 0) {
		obj.var_type = DT_FLOAT;
		if (initialized) obj.float_val = atof(var_value_token.text);
		else obj.float_val = 0.0;
	} else if (strcmp(var_type_str, "list") == 0) {
		if (parser->current_token.tok_kind != T_LBRACKET) {
			printf("[ expected");
			exit(1);
		}
		parser_consume(parser);

		obj.var_type = DT_LIST;
		Node *list_items = NULL;
		if (parser->current_token.tok_kind == T_RBRACKET) {
			parser_consume(parser);
		} else {
			Node item = parser_parse_expression(parser);
			arrput(list_items, item);
			while (parser->current_token.tok_kind == T_COMMA) {
				parser_consume(parser);
				Node item = parser_parse_expression(parser);
				arrput(list_items, item);
			}

			if (parser->current_token.tok_kind != T_RBRACKET) {
				printf("] expected");
			}

			parser_consume(parser);
		}
		obj.list_items = list_items;

	} else if (strcmp(var_type_str, "any") == 0) {
		// TODO: make a separate type inference function
		if (initialized) {
			if (var_value_token.tok_kind == T_INT) {
				return parser_parse_variable_def(parser,
						          id_token,
						          "int",
							  var_value_token,
							  1);
			}
						          
		} else {
			obj.var_type = DT_NULL;
		}
	} else {
		printf("ERROR: unknown data type: %s\n", var_type_str);
		exit(1);
	}
	return obj;
}

int  carrot_get_args_len(Node *args) {
	return arrlen(args);
}

#define string_equals(a, b) (strcmp(a, b) == 0)
void carrot_type_check(tok_kind_t token_kind, char *data_type_str) {
	// check builtin type
	if (string_equals("str", data_type_str)) {
		if (token_kind != T_STR) {
			printf("ERROR: expected string value\n");
			exit(1);
		}
	} 
	if (string_equals("int", data_type_str)) {
		if (token_kind != T_INT) {
			printf("ERROR: expected int value\n");
			exit(1);
		}
	} 
	if (string_equals("float", data_type_str)) {
		if (token_kind != T_FLOAT) {
			printf("ERROR: expected float value\n");
			exit(1);
		}
	} 
}
