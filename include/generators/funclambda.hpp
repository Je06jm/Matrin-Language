#ifndef MARTIN_GENERATORS_FUNCLAMBDA
#define MARTIN_GENERATORS_FUNCLAMBDA

#include <parse.hpp>
#include <string>

namespace Martin {

    class FuncTreeNode : public TreeNodeBase {
    public:
        FuncTreeNode(TokenNode arrow, TokenNode scope) : arrow(arrow), scope(scope) {}

        Type GetType() const override {
            return Type::Misc_Func;
        }

        std::string GetName() const override {
            return "Func";
        }

        void Serialize(std::string& serial) const override {
            if (scope)
                serial = Format("$($, $)", GetName(), *arrow, *scope);
            else
                serial = Format("$($, nullptr)", GetName(), *arrow);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!arrow) return false;
            
            if (arrow->is_token) return false;
            if (!arrow->node->Valid()) return false;
            if (arrow->node->GetType() != Type::Misc_Arrow) return false;

            if (scope) {
                if (scope->is_token) return false;
                if (!scope->node->Valid()) return false;
                if (scope->node->GetType() != Type::Struct_Curly) return false;
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!arrow->is_token) {
                if (arrow->node->GetType() == type) {
                    list.push_back(arrow->node);
                }
                auto list2 = arrow->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            if (scope &&(!scope->is_token)) {
                if (scope->node->GetType() == type) {
                    list.push_back(scope->node);
                }
                auto list2 = scope->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode arrow;
        const TokenNode scope;
    };

    class LambdaTreeNode : public TreeNodeBase {
    public:
        LambdaTreeNode(TokenNode arrow, TokenNode scope) : arrow(arrow), scope(scope) {}

        Type GetType() const override {
            return Type::Misc_Lambda;
        }

        std::string GetName() const override {
            return "Lambda";
        }

        void Serialize(std::string& serial) const override {
            if (scope)
                serial = Format("$($, $)", GetName(), *arrow, *scope);
            else
                serial = Format("$($, nullptr)", GetName(), *arrow);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!arrow || !scope) return false;

            if (arrow->is_token) return false;
            if (!arrow->node->Valid()) return false;
            if (arrow->node->GetType() != Type::Misc_Arrow) return false;

            if (scope->is_token) return false;
            if (!scope->node->Valid()) return false;
            if (scope->node->GetType() != Type::Struct_Curly) return false;

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            std::vector<TreeNode> list;

            if (!arrow->is_token) {
                if (arrow->node->GetType() == type) {
                    list.push_back(arrow->node);
                }
                auto list2 = arrow->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            if (!scope->is_token) {
                if (scope->node->GetType() == type) {
                    list.push_back(scope->node);
                }
                auto list2 = scope->node->GetAllNodesOfType(type);
                list.insert(list.end(), list2.begin(), list2.end());
            }

            return list;
        }

        const TokenNode arrow;
        const TokenNode scope;

        bool has_name = false;
        std::string name;
    };

    class FuncTreeGenerator : public TreeNodeGenerator {
    public:
        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (sym->GetType() == TokenType::Type::KW_Func)) {
                TokenNode arrow = GetIndexOrNull(tree, index+1);
                TokenNode scope = GetIndexOrNull(tree, index+2);

                if (arrow && scope && (!scope->is_token) && (scope->node->GetType() == TreeNodeBase::Type::Struct_Curly)) {
                    TreeNode op = TreeNode(new FuncTreeNode(arrow, scope));

                    op->SetLineNumber(sym->GetLineNumber());

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index, 3);

                    return 3;
                } else if (arrow) {
                    TreeNode op = TreeNode(new FuncTreeNode(arrow, nullptr));

                    op->SetLineNumber(sym->GetLineNumber());

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index, 2);

                    return 2;
                }
            }
            return 0;
        }
    };

    class LambdaTreeGenerator : public TreeNodeGenerator {
    public:
        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (sym->GetType() == TokenType::Type::KW_Lambda)) {
                TokenNode arrow = GetIndexOrNull(tree, index+1);
                TokenNode scope = GetIndexOrNull(tree, index+2);

                if (arrow && scope) {
                    TreeNode op = TreeNode(new LambdaTreeNode(arrow, scope));

                    op->SetLineNumber(sym->GetLineNumber());

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index, 3);

                    return 3;
                }
            }
            return 0;
        }
    };

}

#endif