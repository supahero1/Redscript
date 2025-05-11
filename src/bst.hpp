#pragma once
#include <vector>
#include <memory>
#include <variant>

template<typename _Storage>
struct bst_node
{
    std::vector<std::shared_ptr<bst_node<_Storage>>> children;
    bst_node* parent;

    std::shared_ptr<_Storage> value;

    std::shared_ptr<bst_node<_Storage>>& makeChild(_Storage& _child)
    {
        bst_node<_Storage> child;

        child.value = std::make_shared<_Storage>(_child);
        child.parent = this;
        std::shared_ptr<bst_node<_Storage>> ptr = std::make_shared<bst_node<_Storage>>(_child);
    
        children.push_back(ptr);

        return ptr;
    }
};

enum class bst_operation_type
{
    ADD,SUB,
    MUL,DIV,
    MOD,XOR, POW,

    NONE
};

inline int operatorPrecedence(bst_operation_type ot)
{
    switch(ot)
    {
        case bst_operation_type::XOR:
            return 0;
        case bst_operation_type::MOD:
            return 1;
        case bst_operation_type::MUL:
            return 2;
        case bst_operation_type::DIV:
            return 3;
        case bst_operation_type::ADD:
            return 4;
        case bst_operation_type::SUB:
            return 5;
        default:
            return -1;
    }
}
inline int operatorPrecedence(char op)
{
    switch(op)
    {
        case '^':
            return 0;
        case '%':
            return 1;
        case '*':
            return 2;
        case '/':
            return 3;
        case '+':
            return 4;
        case '-':
            return 5;
        default:
            return -1;
    }
}
template<typename _Storage>
struct bst_operation
{

    using _ValueT = std::variant<bst_operation<_Storage>, _Storage>;

    std::shared_ptr<_ValueT> left;
    std::shared_ptr<_ValueT> right;
    bst_operation_type operation = bst_operation_type::NONE;
    inline bool assignNext(_Storage& s)
    {
        // TODO FIX
        if (right) return false;

        auto val = std::make_shared<_ValueT>(s);

        if(left)
            right = std::make_shared<_ValueT>(s);
        else
            left = std::make_shared<_ValueT>(s);

        return true;
    }
    inline bool assignNext(bst_operation<_Storage>& s)
    {
        if (right) return false;

        if(left)
            right = std::make_shared<_ValueT>(s);
        else
            left = std::make_shared<_ValueT>(s);

        return true;
    }
    inline bool setOperation(char c)
    {
        switch(c)
        {
            case '+':
                operation = bst_operation_type::ADD;
                break;
            case '-':
                operation = bst_operation_type::SUB;
                break;
            case '*':
                operation = bst_operation_type::MUL;
                break;
            case '/':
                operation = bst_operation_type::DIV;
                break;
            case '%':
                operation = bst_operation_type::MOD;
                break;
            case '^':
                operation = bst_operation_type::XOR;
                break;
            default:
                return false;
        }
        return true;
    }
};

template<typename _Storage>
using bst = bst_node<_Storage>;

