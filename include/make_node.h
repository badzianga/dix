#pragma once
#include "lexer.h"
#include "parser.h"
#include "value.h"

ASTNode* make_node_binary(int line, ASTNode* left, TokenType op, ASTNode* right);
ASTNode* make_node_unary(int line, TokenType op, ASTNode* right);
ASTNode* make_node_literal(int line, Value value);
ASTNode* make_node_cast(int line, ValueType target_type, ASTNode* expression);
