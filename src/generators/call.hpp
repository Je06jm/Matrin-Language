#include <parse.hpp>

namespace Martin {

    class CallTreeNode : public TreeNodeBase {
    public:
        CallTreeNode(Token id, TokenNode right) : id(id), right(right) {}

        Type GetType() const override {
            return Type::Misc_Call;
        }

        std::string GetName() const override {
            return "Call";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), id->GetName(), *right);
        }

        const Token id;
        const TokenNode right;
    };

    class CallTreeGenerator : public TreeNodeGenerator {
    public:
        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (sym->GetType() == TokenType::Type::Identifier)) {
                TokenNode right = GetIndexOrNull(tree, index+1);

                if (right && !right->is_token && (right->node->GetType() == TreeNodeBase::Type::Struct_Parentheses)) {
                    TreeNode op = TreeNode(new CallTreeNode(sym, right));

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index, 2);

                    return 2;
                }
            }
            return 0;
        }
    };

}