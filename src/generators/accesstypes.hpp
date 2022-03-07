#include <parse.hpp>

namespace Martin {

    class ArrayTypesTreeNode : public TreeNodeBase {
    public:
        ArrayTypesTreeNode(TokenNode sizes, TokenNode right) : sizes(sizes), right(right) {}

        Type GetType() const override {
            return Type::Access_Array;
        }

        std::string GetName() const override {
            return "Array";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($, $)", GetName(), *sizes, *right);
            
        }

        const TokenNode sizes;
        const TokenNode right;
    };

    class ReferenceTypesTreeNode : public TreeNodeBase {
    public:
        ReferenceTypesTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::Access_Reference;
        }

        std::string GetName() const override {
            return "Reference";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
            
        }

        const TokenNode right;
    };

    class SharedTypesTreeNode : public TreeNodeBase {
    public:
        SharedTypesTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::Access_Shared;
        }

        std::string GetName() const override {
            return "Shared";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
            
        }

        const TokenNode right;
    };

    class UniqueTypesTreeNode : public TreeNodeBase {
    public:
        UniqueTypesTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::Access_Unique;
        }

        std::string GetName() const override {
            return "Unique";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
            
        }

        const TokenNode right;
    };

    class PointerTypesTreeNode : public TreeNodeBase {
    public:
        PointerTypesTreeNode(TokenNode right) : right(right) {}

        Type GetType() const override {
            return Type::Access_Pointer;
        }

        std::string GetName() const override {
            return "Pointer";
        }

        void Serialize(std::string& serial) const override {
            serial = Format("$($)", GetName(), *right);
            
        }

        const TokenNode right;
    };

    class AccessTypesTreeGenerator : public TreeNodeGenerator {
    public:
        bool IsReversed() const override {
            return true;
        }

        size_t ProcessBranch(Tree tree, size_t index, size_t end) override {
            Token sym = GetIndexOrNullToken(tree, index);
            if (sym && (sym->GetType() == TokenType::Type::KW_Array)) {
                TokenNode sizes = GetIndexOrNull(tree, index+1);
                TokenNode right = GetIndexOrNull(tree, index+2);

                if (sizes && right) {
                    TreeNode op = TreeNode(new ArrayTypesTreeNode(sizes, right));

                    TokenNode token_node = TokenNode(new TokenNodeBase);
                    token_node->node = op;
                    ReplaceTreeWithTokenNode(tree, token_node, index, 3);

                    return 3;
                }
            } else if (sym && (
                (sym->GetType() == TokenType::Type::KW_Reference) ||
                (sym->GetType() == TokenType::Type::KW_Shared) ||
                (sym->GetType() == TokenType::Type::KW_Unique) ||
                (sym->GetType() == TokenType::Type::KW_Pointer)
            )) {
                TokenNode right = GetIndexOrNull(tree, index+1);

                if (right) {
                    TreeNode op;

                    switch (sym->GetType()) {
                        case TokenType::Type::KW_Reference:
                            op = TreeNode(new ReferenceTypesTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Pointer:
                            op = TreeNode(new PointerTypesTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Unique:
                            op = TreeNode(new UniqueTypesTreeNode(right));
                            break;
                        
                        case TokenType::Type::KW_Shared:
                            op = TreeNode(new SharedTypesTreeNode(right));
                            break;
                    }

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