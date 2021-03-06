#ifndef MARTIN_GENERATORS_CLASSACCESS
#define MARTIN_GENERATORS_CLASSACCESS

#include <parse.hpp>

namespace Martin {

    class ClassAccessPublicTreeNode : public TreeNodeBase {
    public:
        ClassAccessPublicTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::ClassAccess_Public;
        }

        std::string GetName() const override {
            return "Public";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!right) return false;

            if (right->is_token) {
                if (right->token->GetType() != TokenType::Type::Identifier) return false;
            } else {
                if (right->node->GetType() != Type::Struct_Curly) return false;
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            if (right->is_token) {
                return {};
            }

            std::vector<TreeNode> list;
            if (right->node->GetType() == type) {
                list.push_back(right->node);
            }
            auto list2 = right->node->GetAllNodesOfType(type);
            list.insert(list.end(), list2.begin(), list2.end());

            return list;
        }

        const TokenNode right;
    };

    class ClassAccessProtectedTreeNode : public TreeNodeBase {
    public:
        ClassAccessProtectedTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::ClassAccess_Protected;
        }

        std::string GetName() const override {
            return "Protected";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!right) return false;

            if (right->is_token) {
                if (right->token->GetType() != TokenType::Type::Identifier) return false;
            } else {
                if (right->node->GetType() != Type::Struct_Curly) return false;
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            if (right->is_token) {
                return {};
            }

            std::vector<TreeNode> list;
            if (right->node->GetType() == type) {
                list.push_back(right->node);
            }
            auto list2 = right->node->GetAllNodesOfType(type);
            list.insert(list.end(), list2.begin(), list2.end());

            return list;
        }

        const TokenNode right;
    };

    class ClassAccessPrivateTreeNode : public TreeNodeBase {
    public:
        ClassAccessPrivateTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::ClassAccess_Private;
        }

        std::string GetName() const override {
            return "Private";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!right) return false;

            if (right->is_token) {
                if (right->token->GetType() != TokenType::Type::Identifier) return false;
            } else {
                if (right->node->GetType() != Type::Struct_Curly) return false;
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            if (right->is_token) {
                return {};
            }

            std::vector<TreeNode> list;
            if (right->node->GetType() == type) {
                list.push_back(right->node);
            }
            auto list2 = right->node->GetAllNodesOfType(type);
            list.insert(list.end(), list2.begin(), list2.end());

            return list;
        }

        const TokenNode right;
    };

    class ClassAccessFriendTreeNode : public TreeNodeBase {
    public:
        ClassAccessFriendTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::ClassAccess_Friend;
        }

        std::string GetName() const override {
            return "Friend";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
        }

        bool Valid() const override {
            if (!NodeValid()) {
                Fatal("Node $ is invalid on line $\n", GetName(), GetLineNumber());
            }
            return true;
        }

        bool NodeValid() const {
            if (!right) return false;

            if (right->is_token) {
                if (right->token->GetType() != TokenType::Type::Identifier) return false;
            } else {
                if (right->node->GetType() != Type::Struct_Curly) return false;
            }

            return true;
        }

        std::vector<TreeNode> GetAllNodesOfType(Type type) const override {
            if (right->is_token) {
                return {};
            }

            std::vector<TreeNode> list;
            if (right->node->GetType() == type) {
                list.push_back(right->node);
            }
            auto list2 = right->node->GetAllNodesOfType(type);
            list.insert(list.end(), list2.begin(), list2.end());

            return list;
        }

        const TokenNode right;
    };

    class ClassAccessTreeGenerator : public TreeNodeGenerator {
    public:
        bool IsReversed() const {
            return true;
        }

        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (
                (sym->GetType() == TokenType::Type::KW_Public) ||
                (sym->GetType() == TokenType::Type::KW_Protected) ||
                (sym->GetType() == TokenType::Type::KW_Private) ||
                (sym->GetType() == TokenType::Type::KW_Friend)
            )) {
                TokenNode right = GetIndexOrNull(tree, index+1);

                if (right) {
                    TreeNode op;

                    switch (sym->GetType()) {
                        case TokenType::Type::KW_Public:
                            op = TreeNode(new ClassAccessPublicTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Protected:
                            op = TreeNode(new ClassAccessProtectedTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Private:
                            op = TreeNode(new ClassAccessPrivateTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Friend:
                            op = TreeNode(new ClassAccessFriendTreeNode(right));
                            break;
                    }

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

}

#endif