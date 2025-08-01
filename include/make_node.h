#pragma once
#include "lexer.h"
#include "parser.h"
#include "value.h"

ASTNode* make_node_binary(ASTNode* left, TokenType op, ASTNode* right);
ASTNode* make_node_unary(TokenType op, ASTNode* right);
ASTNode* make_node_literal(Value value);
ASTNode* make_node_cast(ValueType target_type, ASTNode* expression);
