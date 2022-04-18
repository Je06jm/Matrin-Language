#ifndef MARTIN_GENERATORS_ARROW
#define MARTIN_GENERATORS_ARROW

#include <parse.hpp>
#include "enclosures.hpp"
#include "comma.hpp"

namespace Martin {

    class ArrowTreeNode : public TreeNodeBase {
    public:
        ArrowTreeNode(TokenNode left, TokenNode right) : left(left), right(right) {}

        Type GetType() const override {
            return Type::Misc_Arrow;
        }

        std::string GetName() const override {
            return "->";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *left, *right);
        }

        bool Valid() const override {
            if (!left || !right) return false;

            if (left->is_token) return false;
            if (right->is_token) {
                if (right->token->GetType() != TokenType::Type::Identifier) return false;
            } else {
                if (right->node->GetType() != Type::Struct_Curly) return false;
                auto curly = std::static_pointer_cast<StructCurlyTreeNode>(right->node);
                auto tree = curly->inside;

                if (tree->size() == 0) return false;
                
                if ((*tree)[0]->is_token) {
                    if ((*tree)[0]->token->GetType() != TokenType::Type::Identifier) return false;
                    for (auto it : *(tree)) {
                        if (!it->is_token) return false;
                        if (it->token->GetType() != TokenType::Type::Identifier) return false;
                    }
                } else {
                    if ((*tree)[0]->node->GetType() != Type::Struct_Comma) return false;
                    auto comma = std::static_pointer_cast<StructCommaTreeNode>((*tree)[0]->node);
                    for (auto it : comma->nodes) {
                        if (!it->is_token) return false;
                        if (it->token->GetType() != TokenType::Type::Identifier) return false;
                    }
                }
            }

            switch (left->node->GetType()) {
                case Type::Struct_Parentheses:
                case Type::Definition_Typedef:
                    break;
                
                default:
                    return false;
            }

            return left->node->Valid();
        }
        
        const TokenNode left;
        const TokenNode right;
    };

    class ArrowTreeGenerator : public TreeNodeGenerator {
    public:
        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (sym->GetType() == TokenType::Type::SYM_Arrow)) {
                TokenNode left = GetIndexOrNull(tree, index-1);
                TokenNode right = GetIndexOrNull(tree, index+1);

                if (left && right) {
                    TreeNode op = TreeNode(new ArrowTreeNode(left, right));

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