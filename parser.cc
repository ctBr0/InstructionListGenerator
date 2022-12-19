#include <iostream>
#include <string>
#include "parser.h"

using namespace std;

struct InstructionNode* parse_generate_intermediate_representation()
{
    Parser parser;
    return parser.parse_program();
}

Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    return t;
}

struct InstructionNode* Parser::parse_program()
{
    struct InstructionNode* instList;

    parse_var_section();
    instList = parse_body();
    parse_inputs();
    expect(END_OF_FILE);
    
    return instList;
}

void Parser::parse_var_section()
{
    parse_id_list();
    expect(SEMICOLON);
}

void Parser::parse_id_list()
{
    Token var_name = expect(ID);

    location_table.insert(make_pair(var_name.lexeme,next_available)); // add mapping of variable name to index in mem array
    mem[next_available] = 0; // all variables are initialized to 0
    next_available++;     

    Token t = lexer.peek(1);

    if (t.token_type == COMMA)
    {
        expect(COMMA);
        parse_id_list();
    }
    if (t.token_type == SEMICOLON)
    {
        return;
    }
}

struct InstructionNode* Parser::parse_body()
{
    struct InstructionNode* stmtList;

    expect(LBRACE);
    stmtList = parse_stmt_list();
    expect(RBRACE);

    return stmtList;
}

struct InstructionNode* Parser::parse_stmt_list()
{
    struct InstructionNode* instList1;
    struct InstructionNode* instList2;

    instList1 = parse_stmt();
    Token t = lexer.peek(1);

    if (t.token_type == RBRACE)
    {
        return instList1;
    }
    if (t.token_type == ID || t.token_type == OUTPUT || t.token_type == INPUT)
    {
        instList2 = parse_stmt_list();

        struct InstructionNode* temp;
        temp = instList1;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = instList2;
        return instList1;
    }
    if (t.token_type == SWITCH)
    {
        instList2 = parse_stmt_list();

        struct InstructionNode* temp;
        temp = instList1;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = instList2;
        return instList1;
    }
    if (t.token_type == FOR)
    {
        instList2 = parse_stmt_list();

        struct InstructionNode* temp;
        temp = instList1;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = instList2;
        
        return instList1;
    }
    if (t.token_type == WHILE)
    {
        instList2 = parse_stmt_list();

        struct InstructionNode* temp;
        temp = instList1;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = instList2;

        return instList1;
    }
    if (t.token_type == IF)
    {
        instList2 = parse_stmt_list();

        struct InstructionNode* temp;
        temp = instList1;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = instList2;

        return instList1;
    }
}

struct InstructionNode* Parser::parse_stmt()
{
    Token t = lexer.peek(1);

    if (t.token_type == ID)
    {
        return parse_assign_stmt();
    }
    if (t.token_type == WHILE)
    {
        return parse_while_stmt();
    }
    if (t.token_type == IF)
    {
        return parse_if_stmt();
    }
    if (t.token_type == SWITCH)
    {
        return parse_switch_stmt();
    }
    if (t.token_type == FOR)
    {
        return parse_for_stmt();
    }
    if (t.token_type == OUTPUT)
    {
        return parse_output_stmt();
    }
    if (t.token_type == INPUT)
    {
        return parse_input_stmt();
    }
}

struct InstructionNode* Parser::parse_assign_stmt()
{
    struct InstructionNode* inst = new InstructionNode;
    inst->type = ASSIGN;
    inst->next = NULL;

    Token id = expect(ID);
    inst->assign_inst.left_hand_side_index = location(id.lexeme);
    expect(EQUAL);
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    if ((t1.token_type == ID || t1.token_type == NUM) && (t2.token_type == SEMICOLON))
    {
        inst->assign_inst.opernd1_index = parse_primary();
        inst->assign_inst.op = OPERATOR_NONE;
        expect(SEMICOLON);

        return inst;
    }
    if ((t1.token_type == ID || t1.token_type == NUM) && (t2.token_type == PLUS || t2.token_type == MINUS || t2.token_type == MULT || t2.token_type == DIV))
    {
        struct InstructionNode* expr = parse_expr(location(id.lexeme));
        expect(SEMICOLON);
        return expr;
    }
}

struct InstructionNode* Parser::parse_expr(int lhs_index)
{
    struct InstructionNode* inst = new InstructionNode;

    inst->type = ASSIGN;
    inst->assign_inst.left_hand_side_index = lhs_index;
    inst->assign_inst.opernd1_index = parse_primary();
    inst->assign_inst.op = parse_op();
    inst->assign_inst.opernd2_index = parse_primary();
    inst->next = NULL;

    return inst;
}

int Parser::parse_primary()
{
    Token t = lexer.peek(1);

    if (t.token_type == ID)
    {
        Token id = expect(ID);
        return location(id.lexeme);
    }
    if (t.token_type == NUM)
    {
        Token num = expect(NUM);
        // cout << num.lexeme;
        location_table.insert(make_pair(num.lexeme,next_available)); // add mapping of variable name to index in mem array
        mem[next_available] = stoi(num.lexeme); // value of int
        next_available++;
        return location(num.lexeme);
    }
}

ArithmeticOperatorType Parser::parse_op()
{
    Token t = lexer.peek(1);

    if (t.token_type == PLUS)
    {
        expect(PLUS);
        return OPERATOR_PLUS;
    }
    if (t.token_type == MINUS)
    {
        expect(MINUS);
        return OPERATOR_MINUS;
    }
    if (t.token_type == MULT)
    {
        expect(MULT);
        return OPERATOR_MULT;
    }
    if (t.token_type == DIV)
    {
        expect(DIV);
        return OPERATOR_DIV;
    }
}

struct InstructionNode* Parser::parse_output_stmt()
{
    struct InstructionNode* inst = new InstructionNode;
    inst->type = OUT;
    inst->next = NULL;

    expect(OUTPUT);
    Token id = expect(ID);
    inst->output_inst.var_index = location(id.lexeme);
    expect(SEMICOLON);

    return inst;
}

struct InstructionNode* Parser::parse_input_stmt()
{
    struct InstructionNode* inst = new InstructionNode;
    inst->type = IN;
    inst->next = NULL;

    expect(INPUT);
    Token id = expect(ID);
    inst->input_inst.var_index = location(id.lexeme);
    expect(SEMICOLON);

    return inst;
}

struct InstructionNode* Parser::parse_while_stmt()
{
    struct InstructionNode* inst;

    expect(WHILE);
    inst = parse_condition();
    inst->type = CJMP;
    inst->next = parse_body();

    struct InstructionNode* jump = new InstructionNode;
    jump->type = JMP;
    jump->next = NULL;
    jump->jmp_inst.target = inst;

    struct InstructionNode* temp;
    temp = inst;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = jump;

    struct InstructionNode* noOpInst = new InstructionNode;
    noOpInst->type = NOOP;
    noOpInst->next = NULL;

    jump->next = noOpInst;
    inst->cjmp_inst.target = noOpInst;

    return inst;
}

struct InstructionNode* Parser::parse_if_stmt()
{
    struct InstructionNode* inst;

    expect(IF);
    inst = parse_condition();
    inst->type = CJMP;
    inst->next = parse_body();

    struct InstructionNode* noOpInst = new InstructionNode;
    noOpInst->type = NOOP;
    noOpInst->next = NULL;

    struct InstructionNode* temp;
    temp = inst;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = noOpInst;
    inst->cjmp_inst.target = noOpInst;

    return inst;
}

struct InstructionNode* Parser::parse_condition()
{
    struct InstructionNode* inst = new InstructionNode;

    inst->cjmp_inst.opernd1_index = parse_primary();
    inst->cjmp_inst.condition_op = parse_relop();
    inst->cjmp_inst.opernd2_index = parse_primary();
    inst->next = NULL;

    return inst;
}

ConditionalOperatorType Parser::parse_relop()
{
    Token t = lexer.peek(1);

    if (t.token_type == GREATER)
    {
        expect(GREATER);
        return CONDITION_GREATER;
    }
    if (t.token_type == LESS)
    {
        expect(LESS);
        return CONDITION_LESS;
    }
    if (t.token_type == NOTEQUAL)
    {
        expect(NOTEQUAL);
        return CONDITION_NOTEQUAL;
    }
}

struct InstructionNode* Parser::parse_switch_stmt()
{
    struct InstructionNode* inst;

    struct InstructionNode* noOpInst = new InstructionNode;
    noOpInst->type = NOOP;
    noOpInst->next = NULL;

    expect(SWITCH);
    Token id = expect(ID);
    int var_index = location(id.lexeme);

    expect(LBRACE);

    inst = parse_case_list(noOpInst, var_index);

    Token t = lexer.peek(1);

    if (t.token_type == RBRACE)
    {
        struct InstructionNode* temp;
        temp = inst;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = noOpInst;

        expect(RBRACE);

        return inst;
    }
    if (t.token_type == DEFAULT)
    {
        struct InstructionNode* temp;
        temp = inst;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = parse_default_case(noOpInst);

        expect(RBRACE);

        return inst;
    }
}

struct InstructionNode* Parser::parse_case_list(struct InstructionNode* noOp, int var_index)
{
    struct InstructionNode* inst;

    inst = parse_case(noOp, var_index);
    Token t = lexer.peek(1);

    if (t.token_type == RBRACE || t.token_type == DEFAULT)
    {
        return inst;
    }
    if (t.token_type == CASE)
    {
        struct InstructionNode* temp;
        temp = inst;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }

        temp->next = parse_case_list(noOp, var_index);
        return inst;
    }
}

struct InstructionNode* Parser::parse_case(struct InstructionNode* noOp, int var_index)
{

    struct InstructionNode* inst = new InstructionNode;
    inst->type = CJMP;

    expect(CASE);
    inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    inst->cjmp_inst.opernd1_index = var_index;

    Token num = expect(NUM);
    location_table.insert(make_pair(num.lexeme,next_available)); // add mapping of variable name to index in mem array
    mem[next_available] = stoi(num.lexeme); // value of int
    next_available++;

    inst->cjmp_inst.opernd2_index = location(num.lexeme);

    expect(COLON);

    inst->cjmp_inst.target = parse_body();

    inst->next = NULL;

    struct InstructionNode* temp;
    temp = inst->cjmp_inst.target;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = noOp;

    return inst;
}

struct InstructionNode* Parser::parse_default_case(struct InstructionNode* noOp)
{
    struct InstructionNode* inst;

    expect(DEFAULT);
    expect(COLON);
    inst = parse_body();

    struct InstructionNode* temp;
    temp = inst;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = noOp;

    return inst;
}

struct InstructionNode* Parser::parse_for_stmt()
{
    struct InstructionNode* inst;
    
    expect(FOR);
    expect(LPAREN);

    inst = parse_assign_stmt();
    inst->type = ASSIGN;
    inst->next = parse_condition();
    inst->next->type = CJMP;

    expect(SEMICOLON);
    
    struct InstructionNode* assign2;
    assign2 = parse_assign_stmt();

    expect(RPAREN);

    inst->next->next = parse_body();

    struct InstructionNode* jump = new InstructionNode;
    jump->type = JMP;
    jump->next = NULL;
    jump->jmp_inst.target = inst->next;

    struct InstructionNode* temp;
    temp = inst;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = assign2;
    assign2->next = jump;

    struct InstructionNode* noOpInst = new InstructionNode;
    noOpInst->type = NOOP;
    noOpInst->next = NULL;

    jump->next = noOpInst;
    inst->next->cjmp_inst.target = noOpInst;

    return inst;
}

void Parser::parse_inputs()
{
    parse_num_list();
}

void Parser::parse_num_list()
{
    Token input = expect(NUM);
    inputs.push_back(stoi(input.lexeme));
    
    Token t = lexer.peek(1);

    if (t.token_type == END_OF_FILE)
    {
        return;
    }
    if (t.token_type == NUM)
    {
        parse_num_list();
    }
}

int Parser::location(string var_name)
{
    unordered_map<string,int>::const_iterator element = location_table.find(var_name);
    return element->second;
}