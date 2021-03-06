#ifndef MARTIN_GENERATORS_ASSIGNMENTS
#define MARTIN_GENERATORS_ASSIGNMENTS

#include <parse.hpp>
#include "addsub.hpp"
#include "bitwise.hpp"
#include "dot.hpp"
#include "enclosures.hpp"
#include "funclambda.hpp"
#include "muldivmod.hpp"
#include "pow.hpp"
#include "definitions.hpp"

namespace Martin {

    class AssignTreeNode : public TreeNodeBase {
    public:
        AssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_Assign;
        }

        std::string GetName() const override {
            return "=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (left->is_token && (left->token->GetType() != TokenType::Type::Identifier)) return false;
            else if (!left->is_token) {
                switch (left->node->GetType()) {
                    case Type::Definition_Let:
                    case Type::Definition_Set:
                    case Type::Definition_Const:
                    case Type::Definition_Constexpr:
                        return left->node->Valid();
                    
                    default:
                        return false;
                }
            }
            
            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::String8:
                    case TokenType::Type::String16:
                    case TokenType::Type::String32:
                    case TokenType::Type::String16l:
                    case TokenType::Type::String32l:
                    case TokenType::Type::String16b:
                    case TokenType::Type::String32b:
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Boolean:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::Misc_Lambda:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return right->node->Valid();
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class TypeAssignTreeNode : public TreeNodeBase {
    public:
        TypeAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_TypeAssign;
        }

        std::string GetName() const override {
            return ":=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (left->is_token && (left->token->GetType() != TokenType::Type::Identifier)) return false;
            else if (!left->is_token) {
                switch (left->node->GetType()) {
                    case Type::Definition_Let:
                    case Type::Definition_Set:
                    case Type::Definition_Const:
                    case Type::Definition_Constexpr: {
                        // left->node will not be Valid here
                        auto def = std::static_pointer_cast<LetTreeNode>(left->node);
                        if (def->ids.size() == 0) return false;
                        if (def->types != nullptr) return false;
                        break;
                    }
                    
                    default:
                        return false;
                }
            }

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::String8:
                    case TokenType::Type::String16:
                    case TokenType::Type::String32:
                    case TokenType::Type::String16l:
                    case TokenType::Type::String32l:
                    case TokenType::Type::String16b:
                    case TokenType::Type::String32b:
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Boolean:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::Misc_Lambda:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return right->node->Valid();
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class AddAssignTreeNode : public TreeNodeBase {
    public:
        AddAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_AddAssign;
        }

        std::string GetName() const override {
            return "+=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::String8:
                    case TokenType::Type::String16:
                    case TokenType::Type::String32:
                    case TokenType::Type::String16l:
                    case TokenType::Type::String32l:
                    case TokenType::Type::String16b:
                    case TokenType::Type::String32b:
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class SubAssignTreeNode : public TreeNodeBase {
    public:
        SubAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_SubAssign;
        }

        std::string GetName() const override {
            return "-=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class MulAssignTreeNode : public TreeNodeBase {
    public:
        MulAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_MulAssign;
        }

        std::string GetName() const override {
            return "*=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class DivAssignTreeNode : public TreeNodeBase {
    public:
        DivAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_DivAssign;
        }

        std::string GetName() const override {
            return "/=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class ModAssignTreeNode : public TreeNodeBase {
    public:
        ModAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_ModAssign;
        }

        std::string GetName() const override {
            return "%=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }


        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class PowAssignTreeNode : public TreeNodeBase {
    public:
        PowAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_PowAssign;
        }

        std::string GetName() const override {
            return "**=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class BitAndAssignTreeNode : public TreeNodeBase {
    public:
        BitAndAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitAndAssign;
        }

        std::string GetName() const override {
            return "&=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class BitOrAssignTreeNode : public TreeNodeBase {
    public:
        BitOrAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitOrAssign;
        }

        std::string GetName() const override {
            return "|=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class BitXOrAssignTreeNode : public TreeNodeBase {
    public:
        BitXOrAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitXOrAssign;
        }

        std::string GetName() const override {
            return "^=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };
    
    class BitNotAssignTreeNode : public TreeNodeBase {
    public:
        BitNotAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitNotAssign;
        }

        std::string GetName() const override {
            return "~=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;
                
                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class BitShiftLeftAssignTreeNode : public TreeNodeBase {
    public:
        BitShiftLeftAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitShiftLeftAssign;
        }

        std::string GetName() const override {
            return "<<=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }
        
        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;

                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class BitShiftRightAssignTreeNode : public TreeNodeBase {
    public:
        BitShiftRightAssignTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Assignment_BitShiftRightAssign;
        }

        std::string GetName() const override {
            return ">>=";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!left || !right) return false;

            if (!left->is_token) return false;
            if (left->token->GetType() != TokenType::Type::Identifier) return false;

            if (right->is_token) {
                switch (right->token->GetType()) {
                    case TokenType::Type::Integer:
                    case TokenType::Type::UInteger:
                    case TokenType::Type::FloatingSingle:
                    case TokenType::Type::FloatingDouble:
                    case TokenType::Type::Identifier:
                        return true;
                    
                    default:
                        return false;
                }
            } else {
                if (!right->node->Valid()) return false;
                
                switch (right->node->GetType()) {
                    case Type::Misc_Call:
                    case Type::OP_Add:
                    case Type::OP_Sub:
                    case Type::OP_Mul:
                    case Type::OP_Div:
                    case Type::OP_Mod:
                    case Type::OP_Pow:
                    case Type::OP_Dot:
                    case Type::Struct_Parentheses:
                        return true;
                    
                    default:
                        return false;
                }
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!left->is_token) {
                if (left->node->GetType() == type) {
                    list.push_back(left->node);
                }
                auto list2 = left->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }
            
            if (!right->is_token) {
                if (right->node->GetType() == type) {
                    list.push_back(right->node);
                }
                auto list2 = right->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode left;
        const TokenNode right;
    };

    class AssignmentsTreeGenerator : public TreeNodeGenerator {
    public:
        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (
                (sym->GetType() == TokenType::Type::SYM_Assign) ||
                (sym->GetType() == TokenType::Type::SYM_TypeAssign) ||
                (sym->GetType() == TokenType::Type::SYM_AssignAdd) ||
                (sym->GetType() == TokenType::Type::SYM_AssignSub) ||
                (sym->GetType() == TokenType::Type::SYM_AssignMul) ||
                (sym->GetType() == TokenType::Type::SYM_AssignDiv) ||
                (sym->GetType() == TokenType::Type::SYM_AssignMod) ||
                (sym->GetType() == TokenType::Type::SYM_AssignPow) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitAnd) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitOr) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitXOr) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitNot) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitShiftLeft) ||
                (sym->GetType() == TokenType::Type::SYM_AssignBitShiftRight)
            )) {
                TokenNode left = GetIndexOrNull(tree, index-1);
                TokenNode right = GetIndexOrNull(tree, index+1);

                if (left && right) {
                    TreeNode op;

                    switch (sym->GetType()) {
                        case TokenType::Type::SYM_Assign:
                            op = TreeNode(new AssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_TypeAssign:
                            op = TreeNode(new TypeAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignAdd:
                            op = TreeNode(new AddAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignSub:
                            op = TreeNode(new SubAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignMul:
                            op = TreeNode(new MulAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignDiv:
                            op = TreeNode(new DivAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignMod:
                            op = TreeNode(new ModAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignPow:
                            op = TreeNode(new PowAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignBitAnd:
                            op = TreeNode(new BitAndAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignBitOr:
                            op = TreeNode(new BitOrAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignBitXOr:
                            op = TreeNode(new BitXOrAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignBitNot:
                            op = TreeNode(new BitNotAssignTreeNode(left, right));
                            break;

                        case TokenType::Type::SYM_AssignBitShiftLeft:
                            op = TreeNode(new BitShiftLeftAssignTreeNode(left, right));
                            break;
                        
                        case TokenType::Type::SYM_AssignBitShiftRight:
                            op = TreeNode(new BitShiftRightAssignTreeNode(left, right));
                            break;
                    }

                    op->SetLineNumber(sym->GetLineNumber());

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index-1, 3);

                    return 3;
                }
            }
            return 0;
        }
    };

}

#endif