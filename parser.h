#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector>
#include <unordered_map>
// #include "execute.h" from excluded source files
// #include "lexer.h" from excluded source files

class Parser
{
    public:
        struct InstructionNode* parse_program();
    private:
        LexicalAnalyzer lexer;
        std::unordered_map<std::string,int> location_table;
        int location(std::string variable_name);
        Token expect(TokenType expected_type);
        void parse_var_section();
        void parse_id_list();
        struct InstructionNode* parse_body();
        struct InstructionNode* parse_stmt_list();
        struct InstructionNode* parse_stmt();
        struct InstructionNode* parse_assign_stmt();
        struct InstructionNode* parse_expr(int lhs_index);
        int parse_primary();
        ArithmeticOperatorType parse_op();
        struct InstructionNode* parse_output_stmt();
        struct InstructionNode* parse_input_stmt();
        struct InstructionNode* parse_while_stmt();
        struct InstructionNode* parse_if_stmt();
        struct InstructionNode* parse_condition();
        ConditionalOperatorType parse_relop();
        struct InstructionNode* parse_switch_stmt();
        struct InstructionNode* parse_for_stmt();
        struct InstructionNode* parse_case_list(struct InstructionNode* noOp, int var_index);
        struct InstructionNode* parse_case(struct InstructionNode* noOp, int var_index);
        struct InstructionNode* parse_default_case(struct InstructionNode* noOp);
        void parse_inputs();
        void parse_num_list();
};

#endif