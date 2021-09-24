#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/logutils.h"

#define STB_DS_IMPLEMENTATION
#include "../lib/include/stb_ds.h"

void free_node(CarrotObj *node) {
	if (node->type == N_LIST) {
		int len = arrlen(node->list_items);
		for (int i = 0; i < len; i++) {
			free_node(&node->list_items[i]);
		}
		arrfree(node->list_items);
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

void parser_free(Parser *parser) {
	for (int i = 0; i < parser->node_cnt; i++) {
		if (parser->nodes[i].type == N_STATEMENTS) {
			arrfree(parser->nodes[i].statements);
		}
	}
}

CarrotObj parser_parse(Parser *parser) {
	return parser_parse_script(parser);
}

CarrotObj parser_parse_identifier(Parser *parser) {
	Token id_token = parser_consume(parser);
	Token next_token = parser_consume(parser);

	if (strcmp(next_token.text, "as") == 0) {
		/* Handle variable declaration */

		// holding data type
		Token data_type_token = parser_consume(parser);

		if (parser->current_token.tok_kind == T_EQUAL) {
			/* The variable is initialized with some value 
			 * since we found equal char
			 */
			parser_consume(parser);
			Token var_value_token = parser_consume(parser);

			CarrotObj n;
			n.type = N_VAR_DEF;
			strcpy(n.var_name, id_token.text);
			return n;
		} else {
			/* Otherwise, just return the node
			 * TODO: default value based on the type
			 */
			CarrotObj n;
			n.type = N_VAR_DEF;
			strcpy(n.var_name, id_token.text);
			return n;
		}

	} else if (next_token.tok_kind == T_LPAREN) {
		/* Handle function call */

		// handle args
		// args = ....
		parser_consume(parser);
		// ...

		// consume R_PAREN
		parser_consume(parser);

		CarrotObj obj;
		obj.type = N_FUNC_CALL;
		strcpy(obj.func_name, id_token.text);
		return obj;
	} else if (next_token.tok_kind == T_EQUAL) {
		/* Handle assignment */
	}

	printf("Variable declaration or expression is expected.");
	exit(1);
}

CarrotObj parser_parse_keyword(Parser *parser) {
	
}

CarrotObj parser_parse_script(Parser *parser) {
	CarrotObj *statements = NULL;
	while (parser->current_token.tok_kind != T_EOF) {
		CarrotObj stmt = parser_parse_statement(parser);
		arrput(statements, stmt);
	}

	CarrotObj list_node;
	list_node.type = N_LIST;
	list_node.list_items = statements;
	return list_node;
}

CarrotObj parser_parse_statement(Parser *parser) {
	Token t = parser->current_token;
	if (t.tok_kind == T_KEYWORD) {
		return parser_parse_keyword(parser);
	} else if (t.tok_kind == T_ID) {
		return parser_parse_identifier(parser);
	}

	CarrotObj n;
	n.type = N_STATEMENT;
	parser->nodes[parser->node_cnt++] = n;
	return n;
}

CarrotObj parser_parse_statements(Parser *parser){
	//printf("%s\n", tok_kind_to_str(t.tok_kind));
	//CarrotObj n;
	CarrotObj *statements = NULL;

	CarrotObj statement = parser_parse_statement(parser);
	arrput(statements, statement);

	CarrotObj n;
	n.statements = statements;
	n.type = N_STATEMENTS;
	parser->nodes[parser->node_cnt++] = n;

	return n;
}

CarrotObj parser_parse_variable_def(Parser *parser) {
}
