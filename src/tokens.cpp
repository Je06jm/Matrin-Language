#include <tokens.hpp>

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <algorithm>
#include <stdlib.h>

#include "strhelper.hpp"

#ifdef MARTIN_DEBUG
#define FixedToken(name, pname, type, str)\
    class name : public TokenType {\
    public:\
        Type GetType() const override {\
            return type;\
        }\
        void Process(std::string& in) override {\
            in.erase(0, std::strlen(str));\
        }\
        std::string GetName() const override {\
            return str;\
        }\
    };\
    class pname : public PatternType {\
    public:\
        bool IsMatch(const std::string& in) const override {\
            return StrHelper::IsFirstMatch(in.c_str(), str);\
        }\
        Token CreateToken() const override {\
            return Token(new name);\
        }\
    };
#else
#define FixedToken(name, pname, type, str)\
    class name : public TokenType {\
    public:\
        Type GetType() const override {\
            return type;\
        }\
        void Process(std::string& in) override {\
            in.erase(0, std::strlen(str));\
        }\
    };\
    class pname : public PatternType {\
    public:\
        bool IsMatch(const std::string& in) const override {\
            return StrHelper::IsFirstMatch(in.c_str(), str);\
        }\
        Token CreateToken() const override {\
            return Token(new name);\
        }\
    };
#endif

namespace Martin {
    unsigned int line_number = 1;

    enum class NumberType {
        Decimal,
        Hexidecimal,
        Octal,
        Binary
    };

    class NewLineToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::Ignore;
        }

        void Process(std::string& in) override {
            in.erase(0, 1);

            line_number++;
        }
    };

    class WhiteSpaceToken : public TokenType {
        Type GetType() const override {
            return Type::Ignore;
        }

        void Process(std::string& in) override {
            in.erase(0, 1);
        }
    };

    class CommentSingleLineToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::Ignore;
        }

        void Process(std::string& in) override {
            size_t i = 0;
            for (; i < in.length(); i++) {
                if (in[i] == '\n')
                    break;
            }

            in.erase(0, i);
        }
    };

    class CommentMultiLineToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::Ignore;
        }

        void Process(std::string& in) override {
            size_t i = 2;
            for (; i < in.length(); i++) {
                if ((in[i-1] == '*') && (in[i] == '/'))
                    break;
                
                else if (in[i] == '\n')
                    line_number++;
            }

            in.erase(0, i + 1);
        }
    };

    class FloatingSingleToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::FloatingSingle;
        }

        void Process(std::string& in) override {
            size_t index = in.find("f");
            std::string float_str = in.substr(0, index);
            in.erase(0, index+1);
            value = std::stof(float_str);
        }

        std::shared_ptr<void> GetData() override {
            return std::make_shared<float>(value);
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return std::string("FloatingSingle ") + std::to_string(value);
        }
#endif
    private:
        float value = 0.0f;
    };

    class FloatingDoubleToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::FloatingDouble;
        }

        void Process(std::string& in) override {
            size_t index = in.find_first_not_of("1234567890.");
            std::string double_str = in.substr(0, index);
            in.erase(0, index);
            value = std::stod(double_str);
        }

        std::shared_ptr<void> GetData() override {
            return std::make_shared<double>(value);
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return std::string("FloatingDouble ") + std::to_string(value);
        }
#endif
    private:
        double value = 0.0f;
    };

    class UIntegerToken : public TokenType {
    public:
        Type GetType() const override { return Type::UInteger; }

        void Process(std::string& in) override {
            NumberType type = NumberType::Decimal;
            char c;

            in.erase(0, 1);
            
            if (in.length() >= 2) {
                switch (in[1]) {
                    case 'x':
                    case 'X':
                        type = NumberType::Hexidecimal;
                        in.erase(0, 2);
                        break;
                    case 'o':
                    case 'O':
                        type = NumberType::Octal;
                        in.erase(0, 2);
                        break;
                    case 'b':
                    case 'B':
                        type = NumberType::Binary;
                        in.erase(0, 2);
                        break;
                }
            }

            while (true) {
                if (in.length() >= 1) {
                    c = in[0];

                    switch (type) {
                        case NumberType::Hexidecimal:
                            if (c != '_') {
                                value *= 16;
                                if ((c >= '0') && (c < '9'))
                                    value += c - '0';
                            
                                else if ((c >= 'a') && (c <= 'f'))
                                    value += c - 'a' + 10;
                            
                                else if ((c >= 'A') && (c <= 'F'))
                                    value += c - 'A' + 10;
                                
                                else
                                    return;
                            }
                            
                            break;
                        
                        case NumberType::Octal:
                            if (c != '_') {
                                value *= 8;
                                if ((c >= '0') && (c <= '7'))
                                    value += c - '0';
                                
                                else
                                    return;
                            }
                            
                            break;
                        
                        case NumberType::Decimal:
                            if (c != '_') {
                                value *= 10;
                                if ((c >= '0') && (c <= '9'))
                                    value += c - '0';
                                
                                else
                                    return;
                            }

                            break;
                            
                        case NumberType::Binary:
                            if (c != '_') {
                                value *= 2;
                                if ((c == '0') || (c == '1'))
                                    value += c - '0';
                                
                                else
                                    return;
                            }
                            
                            break;
                    }

                    in.erase(0, 1);
                    
                } else {
                    break;
                }
            }

        }
    
        std::shared_ptr<void> GetData() override { return std::make_shared<uintmax_t>(value); }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return std::string("UInteger ") + std::to_string(value);
        }
#endif
    private:
        uintmax_t value = 0;
    };

    class IntegerToken : public TokenType {
    public:
        Type GetType() const override { return Type::Integer; }

        void Process(std::string& in) override {
            char c;

            bool negative = false;

            if (in[0] == '-') {
                negative = true;
                in.erase(0, 1);
            }

            while (true) {
                if (in.length() >= 1) {
                    c = in[0];

                    if (c != '_') {
                        if ((c >= '0') && (c <= '9')) {
                            value *= 10;
                            value += c - '0';
                        } else
                            break;
                    }

                    in.erase(0, 1);
                    
                } else {
                    break;
                }
            }

            if (negative)
                value = -value;
        }
    
        std::shared_ptr<void> GetData() override { return std::make_shared<intmax_t>(value); }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return std::string("Integer ") + std::to_string(value);
        }
#endif
    private:
        intmax_t value = 0;
    };

    class BooleanToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::Boolean;
        }

        void Process(std::string& in) override {
            if (in[0] == 't') {
                value = true;
                in.erase(0, std::strlen("true"));
            } else {
                value = false;
                in.erase(0, std::strlen("false"));
            }
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return std::string("Boolean ") + (value ? "true" : "false");
        }
#endif
    private:
        bool value = false;
    };

    class String8Token : public TokenType {
    public:
        Type GetType() const override {
            return Type::String8;
        }

        void Process(std::string& in) override {
            if (in[0] == '8')
                in.erase(0, 1);

            value = StrHelper::Process(in, UnicodeType_8Bits);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            const char* str = (const char*)value.get();
            return std::string("String8 ") + str;
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String16Token : public TokenType {
    public:
        Type GetType() const override {
            return Type::String16;
        }

        void Process(std::string& in) override {
            in.erase(0, 2);
            value = StrHelper::Process(in, UnicodeType_16Bits);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String16";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String32Token : public TokenType {
    public:
        Type GetType() const override {
            return Type::String32;
        }

        void Process(std::string& in) override {
            in.erase(0, 2);
            value = StrHelper::Process(in, UnicodeType_32Bits);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String32";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String16lToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::String16l;
        }

        void Process(std::string& in) override {
            in.erase(0, 3);
            value = StrHelper::Process(in, UnicodeType_16BitsLittle);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String16l";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String32lToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::String32l;
        }

        void Process(std::string& in) override {
            in.erase(0, 3);
            value = StrHelper::Process(in, UnicodeType_32BitsLittle);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String16b";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String16bToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::String16b;
        }

        void Process(std::string& in) override {
            in.erase(0, 3);
            value = StrHelper::Process(in, UnicodeType_16BitsBig);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String16b";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class String32bToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::String32b;
        }

        void Process(std::string& in) override {
            in.erase(0, 3);
            value = StrHelper::Process(in, UnicodeType_32BitsBig);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            return "String32b";
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class IdentifierToken : public TokenType {
    public:
        Type GetType() const override {
            return Type::Identifier;
        }

        void Process(std::string& in) override {
            size_t i = 0;
            std::string id = "";

            for (; i < in.length(); i++) {
                if ((in[i] >= 'a') && (in[i] <= 'z'))
                    id += in[i];
                
                else if ((in[i] >= 'A') && (in[i] <= 'Z'))
                    id += in[i];
                
                else if ((in[i] >= '0') && (in[i] <= '9'))
                    id += in[i];
                
                else if (in[i] == '_')
                    id += in[i];
                
                else
                    break;
            }

            uint8_t* str = new uint8_t[i + 1];
            memcpy(str, in.c_str(), i);
            str[i] = '\0';
            in.erase(0, i);

            value = std::shared_ptr<uint8_t[]>(str);
        }

        std::shared_ptr<void> GetData() override {
            return value;
        }
#ifdef MARTIN_DEBUG
        std::string GetName() const override {
            const char* str = (const char*)value.get();
            return std::string("Identifier ") + str;
        }
#endif
    private:
        std::shared_ptr<uint8_t[]> value;
    };

    class FloatingSinglePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            bool period = false;
            bool before = false;
            bool after = false;
            bool found_f = false;

            for (size_t i = 0; i < in.length(); i++) {
                if ((in[i] >= '0') && (in[i] <= '9')) {
                    if (period)
                        after = true;
                    
                    else
                        before = true;
                } else if (in[i] == '.')
                    period = true;

                else if (in[i] == 'f')
                    found_f = true;
                
                else
                    break;
            }

            return period && before && after && found_f;
        }

        Token CreateToken() const override {
            return Token(new FloatingSingleToken);
        }
    };

    class FloatingDoublePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            bool period = false;
            bool before = false;
            bool after = false;

            for (size_t i = 0; i < in.length(); i++) {
                if ((in[i] >= '0') && (in[i] <= '9')) {
                    if (period)
                        after = true;
                    
                    else
                        before = true;
                } else if (in[i] == '.')
                    period = true;
                
                else
                    break;
            }

            return period && before && after;
        }

        Token CreateToken() const override {
            return Token(new FloatingDoubleToken);
        }
    };

    class NewLinePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            return in[0] == '\n';
        }

        Token CreateToken() const override {
            return Token(new NewLineToken);
        }
    };

    class WhiteSpacePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            return (in[0] == ' ') || (in[0] == '\t') || (in[0] == '\r');
        }

        Token CreateToken() const override {
            return Token(new WhiteSpaceToken);
        }
    };

    class CommentSingleLinePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if (in.length() < 2)
                return false;

            return (in[0] == '/') && (in[1] == '/');
        }

        Token CreateToken() const override {
            return Token(new CommentSingleLineToken);
        }
    };

    class CommentMultiLinePattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if (in.length() < 2)
                return false;
            
            if ((in[0] != '/') || (in[1] != '*'))
                return false;
            
            for (size_t i = 3; i < in.length(); i++) {
                if ((in[i-1] == '*') && (in[i] == '/'))
                    return true;
            }

            return false;
        }

        Token CreateToken() const override {
            return Token(new CommentMultiLineToken);
        }
    };

    class UIntegerPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            char c = in[0];
            
            if (c != 'u')
                return false;
            
            c = in[1];

            if ((c >= '0') && (c <= '9')) {
                if (in.length() >= 3) {
                    switch (in[2]) {
                        case 'x':
                        case 'X':
                            if (in.length() >= 4) {
                                c = in[3];
                                if (((c >= '0') && (c <= '9')) ||
                                    ((c >= 'a') && (c <= 'f')) ||
                                    ((c >= 'A') && (c <= 'F')))
                                    return true;
                            }
                            // TODO error
                        case 'o':
                        case 'O':
                            if (in.length() >= 4) {
                                c = in[3];
                                if ((c >= '0') && (c <= '7'))
                                    return true;
                            }
                            // TODO error
                        case 'b':
                        case 'B':
                            if (in.length() >= 4) {
                                c = in[3];
                                if ((c == '0') || (c == '1'))
                                    return true;
                            }
                            // TODO error
                    }
                }

                return true;
            }

            return false;
        };

        Token CreateToken() const {
            return Token(new UIntegerToken);
        }
    };

    class IntegerPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            char c = in[0];
            size_t index = 0;

            if (c == '-')
                index++;

            if ((c >= '0') && (c <= '9')) {
                if (in.length() >= index + 2) {
                    switch (in[index + 1]) {
                        case 'x':
                        case 'X':
                            if (in.length() >= index + 3) {
                                c = in[index + 2];
                                if (((c >= '0') && (c <= '9')) ||
                                    ((c >= 'a') && (c <= 'f')) ||
                                    ((c >= 'A') && (c <= 'F')))
                                    return true;
                            }
                            // TODO error
                        case 'o':
                        case 'O':
                            if (in.length() >= index + 3) {
                                c = in[index + 2];
                                if ((c >= '0') && (c <= '7'))
                                    return true;
                            }
                            // TODO error
                        case 'b':
                        case 'B':
                            if (in.length() >= index + 3) {
                                c = in[index + 2];
                                if ((c == '0') || (c == '1'))
                                    return true;
                            }
                            // TODO error
                    }
                }

                return true;
            }

            return false;
        };

        Token CreateToken() const {
            return Token(new IntegerToken);
        }
    };

    class BooleanPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if (StrHelper::IsFirstMatch(in.c_str(), "true"))
                return true;
            
            else if (StrHelper::IsFirstMatch(in.c_str(), "false"))
                return true;
            
            return false;
        }

        Token CreateToken() const override {
            return Token(new BooleanToken);
        }
    };

    class String8Pattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if (in[0] == '8')
                return StrHelper::IsMatch(in.substr(1));

            return StrHelper::IsMatch(in);
        }

        Token CreateToken() const override {
            return Token(new String8Token);
        }
    };

    class String16Pattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '1') || (in[1] == '6'))
                return false;

            return StrHelper::IsMatch(in.substr(2));
        }

        Token CreateToken() const override {
            return Token(new String16Token);
        }
    };

    class String32Pattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '3') || (in[1] != '2'))
                return false;

            return StrHelper::IsMatch(in.substr(2));
        }

        Token CreateToken() const override {
            return Token(new String32Token);
        }
    };

    class String16lPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '1') || (in[1] != '6') || (in[2] != 'l'))
                return false;

            return StrHelper::IsMatch(in.substr(3));
        }

        Token CreateToken() const override {
            return Token(new String16lToken);
        }
    };

    class String32lPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '3') || (in[1] != '2') || (in[2] != 'l'))
                return false;

            return StrHelper::IsMatch(in.substr(3));
        }

        Token CreateToken() const override {
            return Token(new String32lToken);
        }
    };

    class String16bPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '1') || (in[1] != '6') || (in[2] != 'b'))
                return false;

            return StrHelper::IsMatch(in.substr(3));
        }

        Token CreateToken() const override {
            return Token(new String16bToken);
        }
    };

    class String32bPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            if ((in[0] != '3') || (in[1] != '2') || (in[2] != 'b'))
                return false;

            return StrHelper::IsMatch(in.substr(3));
        }

        Token CreateToken() const override {
            return Token(new String32bToken);
        }
    };

    class IdentifierPattern : public PatternType {
    public:
        bool IsMatch(const std::string& in) const override {
            size_t i = 0;
            char c = in[i];
            while (c == '_') {
                i++;
                if (i >= in.length())
                    return false;
                
                c = in[i];
            }

            if ((in[i] >= 'a') && (in[i] <= 'z'))
                return true;
            
            else if ((in[i] >= 'A') && (in[i] <= 'Z'))
                return true;
            
            else if ((in[i] >= '0') && (in[i] <= '9') && (i != 0))
                return true;
            
            return false;
        }

        Token CreateToken() const override {
            return Token(new IdentifierToken);
        }
    };

    FixedToken(KWFromToken, KWFromPattern, Type::KW_From, "from ")
    FixedToken(KWImportToken, KWImportPattern, Type::KW_Import, "import ")
    FixedToken(KWAsToken, KWAsPattern, Type::KW_As, "as ")
    FixedToken(KWStructToken, KWStructPattern, Type::KW_Struct, "struct ")
    FixedToken(KWUnionToken, KWUnionPattern, Type::KW_Union, "union ")
    FixedToken(KWEnumToken, KWEnumPattern, Type::KW_Enum, "enum ")
    FixedToken(KWTypedefToken, KWTypedefPattern, Type::KW_Typedef, "typedef ")
    FixedToken(KWLetToken, KWLetPattern, Type::KW_Let, "let ")
    FixedToken(KWSetToken, KWSetPattern, Type::KW_Set, "set ")
    FixedToken(KWConstToken, KWConstPattern, Type::KW_Const, "const ")
    FixedToken(KWConstexprToken, KWConstexprPattern, Type::KW_Constexpr, "constexpr ")
    FixedToken(KWArrayToken, KWArrayPattern, Type::KW_Array, "array ")
    FixedToken(KWReferenceToken, KWReferencePattern, Type::KW_Reference, "reference ")
    FixedToken(KWSharedToken, KWSharedPattern, Type::KW_Shared, "shared ")
    FixedToken(KWPointerToken, KWPointerPattern, Type::KW_Pointer, "pointer ")
    FixedToken(KWExternToken, KWExternPattern, Type::KW_Extern, "extern ")
    FixedToken(KWUnsafeToken, KWUnsafePattern, Type::KW_Unsafe, "unsafe ")
    FixedToken(KWFuncToken, KWFuncPattern, Type::KW_Func, "func ")
    FixedToken(KWClassToken, KWClassPattern, Type::KW_Class, "class ")
    FixedToken(KWPublicToken, KWPublicPattern, Type::KW_Public, "public")
    FixedToken(KWProtectedToken, KWProtectedPattern, Type::KW_Protected, "protected")
    FixedToken(KWPrivateToken, KWPrivatePattern, Type::KW_Private, "private")
    FixedToken(KWVirtualToken, KWVirtualPattern, Type::KW_Virtual, "virtual ")
    FixedToken(KWOverrideToken, KWOverridePattern, Type::KW_Override, "override ")
    FixedToken(KWStaticToken, KWStaticPattern, Type::KW_Static, "static ")
    FixedToken(KWSuperToken, KWSuperPattern, Type::KW_Super, "super")
    FixedToken(KWIfToken, KWIfPattern, Type::KW_If, "if ")
    FixedToken(KWElifToken, KWElifPattern, Type::KW_Elif, "elif ")
    FixedToken(KWElseToken, KWElsePattern, Type::KW_Else, "else ")
    FixedToken(KWForToken, KWForPattern, Type::KW_For, "for ")
    FixedToken(KWForeachToken, KWForeachPattern, Type::KW_Foreach, "foreach ")
    FixedToken(KWWhileToken, KWWhilePattern, Type::KW_While, "while ")
    FixedToken(KWContinueToken, KWContinuePattern, Type::KW_Continue, "continue")
    FixedToken(KWBreakToken, KWBreakPattern, Type::KW_Break, "break")
    FixedToken(KWMatchToken, KWMatchPattern, Type::KW_Match, "match ")
    FixedToken(KWReturnToken, KWReturnPattern, Type::KW_Return, "return ")
    FixedToken(KWLambdaToken, KWLambdaPattern, Type::KW_Lambda, "lambda ")
    FixedToken(KWAndToken, KWAndPattern, Type::KW_And, "and ")
    FixedToken(KWOrToken, KWOrPattern, Type::KW_Or, "or ")
    FixedToken(KWNotToken, KWNotPattern, Type::KW_Not, "not ")
    
    FixedToken(SYMCommaToken, SYMCommaPattern, Type::SYM_Comma, ",")
    FixedToken(SYMPeriodToken, SYMPeriodPattern, Type::SYM_Period, ".")
    FixedToken(SYMOpenCurlyToken, SYMOpenCurlyPattern, Type::SYM_OpenCurly, "{")
    FixedToken(SYMCloseCurlyToken, SYMCloseCurlyPattern, Type::SYM_CloseCurly, "}")
    FixedToken(SYMOpenBracketToken, SYMOpenBracketPattern, Type::SYM_OpenBracket, "[")
    FixedToken(SYMCloseBracketToken, SYMCloseBracketPattern, Type::SYM_CloseBracket, "]")
    FixedToken(SYMOpenParenthesesToken, SYMOpenParenthesesPattern, Type::SYM_OpenParentheses, "(")
    FixedToken(SYMCloseParenthesesToken, SYMCloseParenthesesPattern, Type::SYM_CloseParentheses, ")")
    FixedToken(SYMSemiColonToken, SYMSemiColonPattern, Type::SYM_SemiColon, ";")
    FixedToken(SYMColonToken, SYMColonPattern, Type::SYM_Colon, ":")
    FixedToken(SYMArrowToken, SYMArrowPattern, Type::SYM_Arrow, "->")
    FixedToken(SYMAddToken, SYMAddPattern, Type::SYM_Add, "+")
    FixedToken(SYMSubToken, SYMSubPattern, Type::SYM_Sub, "-")
    FixedToken(SYMMulToken, SYMMulPattern, Type::SYM_Mul, "*")
    FixedToken(SYMDivToken, SYMDivPattern, Type::SYM_Div, "/")
    FixedToken(SYMModToken, SYMModPattern, Type::SYM_Mod, "%")
    FixedToken(SYMPowToken, SYMPowPattern, Type::SYM_Pow, "**")
    FixedToken(SYMBitAndToken, SYMBitAndPattern, Type::SYM_BitAnd, "&")
    FixedToken(SYMBitOrToken, SYMBitOrPattern, Type::SYM_BitOr, "|")
    FixedToken(SYMBitXOrToken, SYMBitXOrPattern, Type::SYM_BitXOr, "^")
    FixedToken(SYMBitNotToken, SYMBitNotPattern, Type::SYM_BitNot, "~")
    FixedToken(SYMBitShiftLeftToken, SYMBitShiftLeftPattern, Type::SYM_BitShiftLeft, "<<")
    FixedToken(SYMBitShiftRightToken, SYMBitShiftRightPattern, Type::SYM_BitShiftRight, ">>")
    FixedToken(SYMAssignToken, SYMAssignPattern, Type::SYM_Assign, "=")
    FixedToken(SYMTypeAssignToken, SYMTypeAssignPattern, Type::SYM_TypeAssign, ":=")
    FixedToken(SYMAssignAddToken, SYMAssignAddPattern, Type::SYM_AssignAdd, "+=")
    FixedToken(SYMAssignSubToken, SYMAssignSubPattern, Type::SYM_AssignSub, "-=")
    FixedToken(SYMAssignMulToken, SYMAssignMulPattern, Type::SYM_AssignMul, "*=")
    FixedToken(SYMAssignDivToken, SYMAssignDivPattern, Type::SYM_AssignDiv, "/=")
    FixedToken(SYMAssignModToken, SYMAssignModPattern, Type::SYM_AssignMod, "%=")
    FixedToken(SYMAssignPowToken, SYMAssignPowPattern, Type::SYM_AssignPow, "**=")
    FixedToken(SYMAssignBitAndToken, SYMAssignBitAndPattern, Type::SYM_AssignBitAnd, "&=")
    FixedToken(SYMAssignBitOrToken, SYMAssignBitOrPattern, Type::SYM_AssignBitOr, "|=")
    FixedToken(SYMAssignBitXOrToken, SYMAssignBitXOrPattern, Type::SYM_AssignBitXOr, "^=")
    FixedToken(SYMAssignBitShiftLeftToken, SYMAssignBitShiftLeftPattern, Type::SYM_AssignBitShiftLeft, "<<=")
    FixedToken(SYMAssignBitShiftRightToken, SYMAssignBitShiftRightPattern, Type::SYM_AssignBitShiftRight, ">>=")
    FixedToken(SYMEqualsToken, SYMEqualsPattern, Type::SYM_Equals, "==")
    FixedToken(SYMNotEqualsToken, SYMNotEqualsPattern, Type::SYM_NotEquals, "!=")
    FixedToken(SYMLessThanToken, SYMLessThanPattern, Type::SYM_LessThan, "<")
    FixedToken(SYMGreaterThanToken, SYMGreaterThanPattern, Type::SYM_GreaterThan, ">")
    FixedToken(SYMLessThanEqualsToken, SYMLessThanEqualsPattern, Type::SYM_LessThanEquals, "<=")
    FixedToken(SYMGreaterThanEqualsToken, SYMGreaterThanEqualsPattern, Type::SYM_GreaterThanEquals, ">=")

    std::vector<Pattern> patterns;

    void InitTokenizer() {
        patterns.push_back(Pattern(new NewLinePattern));
        patterns.push_back(Pattern(new WhiteSpacePattern));
        patterns.push_back(Pattern(new CommentSingleLinePattern));
        patterns.push_back(Pattern(new CommentMultiLinePattern));
        patterns.push_back(Pattern(new String8Pattern));
        patterns.push_back(Pattern(new String16lPattern));
        patterns.push_back(Pattern(new String32lPattern));
        patterns.push_back(Pattern(new String16bPattern));
        patterns.push_back(Pattern(new String32bPattern));
        patterns.push_back(Pattern(new String16Pattern));
        patterns.push_back(Pattern(new String32Pattern));
        patterns.push_back(Pattern(new FloatingSinglePattern));
        patterns.push_back(Pattern(new FloatingDoublePattern));
        patterns.push_back(Pattern(new UIntegerPattern));
        patterns.push_back(Pattern(new IntegerPattern));
        patterns.push_back(Pattern(new BooleanPattern));
        patterns.push_back(Pattern(new KWFromPattern));
        patterns.push_back(Pattern(new KWImportPattern));
        patterns.push_back(Pattern(new KWAsPattern));
        patterns.push_back(Pattern(new KWStructPattern));
        patterns.push_back(Pattern(new KWUnionPattern));
        patterns.push_back(Pattern(new KWEnumPattern));
        patterns.push_back(Pattern(new KWTypedefPattern));
        patterns.push_back(Pattern(new KWLetPattern));
        patterns.push_back(Pattern(new KWSetPattern));
        patterns.push_back(Pattern(new KWConstPattern));
        patterns.push_back(Pattern(new KWConstexprPattern));
        patterns.push_back(Pattern(new KWArrayPattern));
        patterns.push_back(Pattern(new KWReferencePattern));
        patterns.push_back(Pattern(new KWSharedPattern));
        patterns.push_back(Pattern(new KWPointerPattern));
        patterns.push_back(Pattern(new KWExternPattern));
        patterns.push_back(Pattern(new KWUnsafePattern));
        patterns.push_back(Pattern(new KWFuncPattern));
        patterns.push_back(Pattern(new KWClassPattern));
        patterns.push_back(Pattern(new KWPublicPattern));
        patterns.push_back(Pattern(new KWProtectedPattern));
        patterns.push_back(Pattern(new KWPrivatePattern));
        patterns.push_back(Pattern(new KWVirtualPattern));
        patterns.push_back(Pattern(new KWOverridePattern));
        patterns.push_back(Pattern(new KWStaticPattern));
        patterns.push_back(Pattern(new KWSuperPattern));
        patterns.push_back(Pattern(new KWIfPattern));
        patterns.push_back(Pattern(new KWElifPattern));
        patterns.push_back(Pattern(new KWElsePattern));
        patterns.push_back(Pattern(new KWForPattern));
        patterns.push_back(Pattern(new KWForeachPattern));
        patterns.push_back(Pattern(new KWWhilePattern));
        patterns.push_back(Pattern(new KWContinuePattern));
        patterns.push_back(Pattern(new KWBreakPattern));
        patterns.push_back(Pattern(new KWMatchPattern));
        patterns.push_back(Pattern(new KWReturnPattern));
        patterns.push_back(Pattern(new KWLambdaPattern));
        patterns.push_back(Pattern(new KWAndPattern));
        patterns.push_back(Pattern(new KWOrPattern));
        patterns.push_back(Pattern(new KWNotPattern));
        patterns.push_back(Pattern(new SYMCommaPattern));
        patterns.push_back(Pattern(new SYMPeriodPattern));
        patterns.push_back(Pattern(new SYMOpenCurlyPattern));
        patterns.push_back(Pattern(new SYMCloseCurlyPattern));
        patterns.push_back(Pattern(new SYMOpenBracketPattern));
        patterns.push_back(Pattern(new SYMCloseBracketPattern));
        patterns.push_back(Pattern(new SYMOpenParenthesesPattern));
        patterns.push_back(Pattern(new SYMCloseParenthesesPattern));
        patterns.push_back(Pattern(new SYMSemiColonPattern));
        patterns.push_back(Pattern(new SYMColonPattern));
        patterns.push_back(Pattern(new SYMArrowPattern));
        patterns.push_back(Pattern(new SYMAssignAddPattern));
        patterns.push_back(Pattern(new SYMAssignSubPattern));
        patterns.push_back(Pattern(new SYMAssignMulPattern));
        patterns.push_back(Pattern(new SYMAssignDivPattern));
        patterns.push_back(Pattern(new SYMAssignModPattern));
        patterns.push_back(Pattern(new SYMAssignPowPattern));
        patterns.push_back(Pattern(new SYMAssignBitAndPattern));
        patterns.push_back(Pattern(new SYMAssignBitOrPattern));
        patterns.push_back(Pattern(new SYMAssignBitXOrPattern));
        patterns.push_back(Pattern(new SYMAssignBitShiftLeftPattern));
        patterns.push_back(Pattern(new SYMAssignBitShiftRightPattern));
        patterns.push_back(Pattern(new SYMAddPattern));
        patterns.push_back(Pattern(new SYMSubPattern));
        patterns.push_back(Pattern(new SYMMulPattern));
        patterns.push_back(Pattern(new SYMDivPattern));
        patterns.push_back(Pattern(new SYMModPattern));
        patterns.push_back(Pattern(new SYMPowPattern));
        patterns.push_back(Pattern(new SYMBitAndPattern));
        patterns.push_back(Pattern(new SYMBitOrPattern));
        patterns.push_back(Pattern(new SYMBitXOrPattern));
        patterns.push_back(Pattern(new SYMBitNotPattern));
        patterns.push_back(Pattern(new SYMBitShiftLeftPattern));
        patterns.push_back(Pattern(new SYMBitShiftRightPattern));
        patterns.push_back(Pattern(new SYMTypeAssignPattern));
        patterns.push_back(Pattern(new SYMAssignPattern));
        patterns.push_back(Pattern(new SYMEqualsPattern));
        patterns.push_back(Pattern(new SYMNotEqualsPattern));
        patterns.push_back(Pattern(new SYMLessThanEqualsPattern));
        patterns.push_back(Pattern(new SYMGreaterThanEqualsPattern));
        patterns.push_back(Pattern(new SYMLessThanPattern));
        patterns.push_back(Pattern(new SYMGreaterThanPattern));
        patterns.push_back(Pattern(new IdentifierPattern));
    }

    std::unique_ptr<std::vector<Token>> Tokenize(std::string input) {
        std::vector<Token> tokens;
        size_t str_length;

        while (input.length() != 0) {
            str_length = input.length();

            for (auto pattern : patterns) {
                if (pattern->IsMatch(input)) {
                    Token t = pattern->CreateToken();
                    if (t != nullptr) {
                        t->SetLineNumber(line_number);
                        t->Process(input);

                        if (t->GetType() != TokenType::Type::Ignore) 
                            tokens.push_back(t);
                        
                        break;
                    }
                }
            }

            if (str_length == input.length()) {
                // TODO error
                input.erase(std::remove(input.begin(), input.end(), '\r'));
                size_t index = input.find("\n");
                if (index != std::string::npos)
                    input = input.substr(0, index);
                
                std::cerr << "No matching token type for: \"" << input << "\" on line " << line_number << "\n";
                exit(EXIT_FAILURE);
            }
        }

        return std::make_unique<std::vector<Token>>(tokens);
    }

}