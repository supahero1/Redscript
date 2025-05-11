#include "lang.hpp"
#include "rbc.hpp"
#define COMP_ERROR(_ec, _message, _trace, ...)                                                              \
    {                                                                                                       \
        err = rs_error(_message, *program.context->content, _trace, program.context->fName, ##__VA_ARGS__); \
        err.trace.ec = _ec;                                                                                 \
        return;                                                                                             \
    }

#pragma region variables
void rs_variable::rbc_create(rbc_program &program, rs_error &err)
{
    // TODO
    // we assume all compile errors have been avoided, ex: there are no variables with duplicate names.
    // rbc_command create(rbc_instruction::SAVE, ...);
}
#pragma endregion variables

#pragma region expressions
#define EXPR_ERROR(_ec, _message, _trace, ...)                                                               \
    {                                                                                                        \
        *err = rs_error(_message, *program.context->content, _trace, program.context->fName, ##__VA_ARGS__); \
        err->trace.ec = _ec;                                                                                 \
        return root;                                                                                         \
    }
bst_operation<token> make_bst(rbc_program &program, token_list &tlist, size_t &start, rs_error *err, bool br, bool oneNode)
{
    const size_t S = tlist.size();
    bst_operation<token> root;
    bool shouldBreak = false;
    do
    {
        token &current = tlist.at(start);

        switch (current.type)
        {
        case token_type::LINE_END:
            return root;
        case token_type::BRACKET_CLOSED:
            if (!br)
                EXPR_ERROR(RS_SYNTAX_ERROR, "Unclosed bracket found.", current.trace);
            shouldBreak = true;
            break;
        case token_type::BRACKET_OPEN:
        {
            bst_operation<token> child = make_bst(program, tlist, ++start, err, true, false);
            if (err->trace.ec)
                return root;

            if (!root.assignNext(child))
                EXPR_ERROR(RS_SYNTAX_ERROR, "Missing operator.", tlist.at(start).trace);
            start ++;
            if (oneNode || current.type == token_type::LINE_END)
                return root;
            break;
        }
        case token_type::OPERATOR:
        {
            if (root.operation != bst_operation_type::NONE)
                EXPR_ERROR(RS_SYNTAX_ERROR, "Unexpected token.", current.trace);
            if (current.repr.length() > 1)
                EXPR_ERROR(RS_SYNTAX_ERROR, "Unexpected operator.", current.trace);
            const char op = current.info;

            if (!root.setOperation(op))
                EXPR_ERROR(RS_SYNTAX_ERROR, "Unsupported operator.", current.trace);

            break;
        }
        case token_type::INT_LITERAL:
        case token_type::FLOAT_LITERAL:
        case token_type::STRING_LITERAL:
        case token_type::LIST_LITERAL:
        case token_type::OBJECT_LITERAL:
        case token_type::WORD:
        {

            if (current.type == token_type::WORD)
            {
                if (!program.getVariable(current.repr))
                    EXPR_ERROR(RS_SYNTAX_ERROR, "Unexpected token in expression.", current.trace);
                if (!root.assignNext(current))
                    EXPR_ERROR(RS_SYNTAX_ERROR, "Missing operator.", current.trace);
            }
            else if (!root.assignNext(current))
                EXPR_ERROR(RS_SYNTAX_ERROR, "Missing operator.", current.trace);

            if (oneNode)
                return root;
            break;
        }
        default:
            EXPR_ERROR(RS_SYNTAX_ERROR, "Unexpected token in expression.", current.trace);
        }
        if (root.right)
        {
            token* next;
            while (start + 1 < S && (next = &tlist.at(start + 1))->type == token_type::OPERATOR)
            {
                if (next->repr.length() != 1) // todo remove 
                    EXPR_ERROR(RS_SYNTAX_ERROR, "Unsupported operator.", current.trace);
                const int pLeft = operatorPrecedence(root.operation), pRight = operatorPrecedence(next->info);

                
                // shift over
                start += 2;
                if (start >= S)
                    EXPR_ERROR(RS_SYNTAX_ERROR, "Expected expression, not EOF.", next->trace);
                // check if we are in a bracket, and advance to next token
                bool isBracketNode = tlist.at(start).type == token_type::BRACKET_OPEN;
                    
                if (pRight < pLeft)
                {
                    // parse the next node only, concat root.right and new right to make more favorable node
                    bst_operation<token> right = make_bst(program, tlist, start, err, isBracketNode, true);
                    if (err->trace.ec)
                        return root;

                    right.right = root.right;

                    std::swap(*right.left, *right.right);

                    root.right = std::make_shared<bst_operation<token>::_ValueT>(right);
                } else
                {
                    // we encapsulate root node with another node.
                    bst_operation<token> newRoot;
                    newRoot.setOperation(pLeft);
                    // assign left
                    newRoot.assignNext(root);

                    bst_operation<token> right = make_bst(program, tlist, start, err, isBracketNode, false);
                    if (err->trace.ec)
                        return root;
                    // assign right
                    newRoot.assignNext(right);

                    return newRoot;
                }
            }
            return root;
        }
    } while (!shouldBreak && ++start < S);

    if (start == S)
        EXPR_ERROR(RS_EOF_ERROR, "Expected expression, not EOF.", tlist.at(S - 1).trace);

    return root;
}
#undef EXPR_ERROR
rs_expression expreval(rbc_program &program, token_list &tlist, size_t &start, rs_error *err)
{
    rs_expression expr;

    bst_operation<token> bst = make_bst(program, tlist, start, err);
    if (err->trace.ec)
        return expr;

    expr.operation = bst;

    /**
     * Navigate to the left most expression:
     * ((1+2)+3)+2;left most: 1+2
     */

    // TODO: prune constant branches, eg 1 + 3, or constexpr math functions like math.sin() etc.
    return expr;
}
#pragma endregion expressions
#undef COMP_ERROR