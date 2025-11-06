#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <variant>

// Forward declarations
struct ParseTree;
class Interpreter;

// Represents the different data types our language can handle.
// Using std::variant for a type-safe union.
using ValueVariant = std::variant<std::monostate, bool, int, double, std::string>;

// A helper struct to wrap the variant and provide utility functions.
struct Value {
    ValueVariant data;

    Value() : data(std::monostate{}) {}
    Value(bool val) : data(val) {}
    Value(int val) : data(val) {}
    Value(double val) : data(val) {}
    Value(const std::string& val) : data(val) {}
    Value(const char* val) : data(std::string(val)) {}

    // Helper to print the value
    void print() const {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                std::cout << "null";
            } else if constexpr (std::is_same_v<T, bool>) {
                std::cout << (arg ? "true" : "false");
            } else {
                std::cout << arg;
            }
        }, data);
    }

    // Helper to check for truthiness (like in Python)
    bool is_truthy() const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) return false;
            if constexpr (std::is_same_v<T, bool>) return arg;
            if constexpr (std::is_same_v<T, int>) return arg != 0;
            if constexpr (std::is_same_v<T, double>) return arg != 0.0;
            if constexpr (std::is_same_v<T, std::string>) return !arg.empty();
            return false;
        }, data);
    }
};

// Represents a node in the Parse Tree (Abstract Syntax Tree)
enum class NodeType {
    PROGRAM,
    BLOCK,
    ASSIGNMENT,
    IF_STATEMENT,
    WHILE_LOOP,
    FUNCTION_CALL,
    BINARY_OP,
    UNARY_OP,
    VARIABLE,
    INT_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL
};

struct ParseTree {
    NodeType type;
    std::string value; // Stores operator, variable name, literal value
    std::vector<ParseTree*> children;

    ParseTree(NodeType t, std::string val = "") : type(t), value(val) {}

    // Destructor to clean up child nodes
    ~ParseTree() {
        for (ParseTree* child : children) {
            delete child;
        }
    }
};

// The main interpreter class
class Interpreter {
private:
    std::vector<std::map<std::string, Value>> scopes;

    // --- Scope Management ---
    void push_scope() {
        scopes.emplace_back();
    }

    void pop_scope() {
        if (!scopes.empty()) {
            scopes.pop_back();
        }
    }

    void set_variable(const std::string& name, const Value& val) {
        // Search from the innermost scope outwards
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->count(name)) {
                (*it)[name] = val;
                return;
            }
        }
        // If not found, define it in the current (innermost) scope
        scopes.back()[name] = val;
    }

    Value get_variable(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->count(name)) {
                return (*it).at(name);
            }
        }
        throw std::runtime_error("Runtime Error: Variable '" + name + "' not defined.");
    }

    // --- Expression Evaluation ---
    Value evaluate_expression(ParseTree* node) {
        switch (node->type) {
            case NodeType::INT_LITERAL:
                return Value(std::stoi(node->value));
            case NodeType::DOUBLE_LITERAL:
                return Value(std::stod(node->value));
            case NodeType::STRING_LITERAL:
                return Value(node->value);
            case NodeType::BOOL_LITERAL:
                return Value(node->value == "true");
            case NodeType::VARIABLE:
                return get_variable(node->value);
            case NodeType::BINARY_OP:
                return evaluate_binary_op(node);
            case NodeType::UNARY_OP:
                 return evaluate_unary_op(node);
            case NodeType::FUNCTION_CALL: // Can be an expression if it returns a value
                return execute_function_call(node);
            default:
                throw std::runtime_error("Runtime Error: Invalid expression node.");
        }
    }

    Value evaluate_unary_op(ParseTree* node) {
        Value right = evaluate_expression(node->children[0]);
        if (node->value == "-") {
            if (std::holds_alternative<int>(right.data)) return Value(-std::get<int>(right.data));
            if (std::holds_alternative<double>(right.data)) return Value(-std::get<double>(right.data));
        } else if (node->value == "!") {
             return Value(!right.is_truthy());
        }
        throw std::runtime_error("Runtime Error: Invalid unary operator '" + node->value + "'.");
    }


    Value evaluate_binary_op(ParseTree* node) {
        Value left = evaluate_expression(node->children[0]);
        Value right = evaluate_expression(node->children[1]);
        const std::string& op = node->value;

        // Numeric operations
        if (std::holds_alternative<int>(left.data) && std::holds_alternative<int>(right.data)) {
            int l = std::get<int>(left.data);
            int r = std::get<int>(right.data);
            if (op == "+") return Value(l + r);
            if (op == "-") return Value(l - r);
            if (op == "*") return Value(l * r);
            if (op == "/") return Value(l / r);
            if (op == ">") return Value(l > r);
            if (op == "<") return Value(l < r);
            if (op == ">=") return Value(l >= r);
            if (op == "<=") return Value(l <= r);
        }

        // Promote int to double if mixing types
        double l = std::holds_alternative<int>(left.data) ? std::get<int>(left.data) : std::get<double>(left.data);
        double r = std::holds_alternative<int>(right.data) ? std::get<int>(right.data) : std::get<double>(right.data);

        if (op == "+") return Value(l + r);
        if (op == "-") return Value(l - r);
        if (op == "*") return Value(l * r);
        if (op == "/") return Value(l / r);
        if (op == ">") return Value(l > r);
        if (op == "<") return Value(l < r);
        if (op == ">=") return Value(l >= r);
        if (op == "<=") return Value(l <= r);

        // String Concatenation
        if (op == "+" && std::holds_alternative<std::string>(left.data) && std::holds_alternative<std::string>(right.data)) {
            return Value(std::get<std::string>(left.data) + std::get<std::string>(right.data));
        }

        // Equality operators (can work on mixed types)
        if (op == "==") return Value(left.data == right.data);
        if (op == "!=") return Value(left.data != right.data);

        throw std::runtime_error("Runtime Error: Invalid binary operation between types.");
    }

    // --- Statement Execution ---
    void execute_statement(ParseTree* node) {
        if (!node) return;

        switch (node->type) {
            case NodeType::PROGRAM:
            case NodeType::BLOCK:
                execute_block(node);
                break;
            case NodeType::ASSIGNMENT: {
                std::string var_name = node->children[0]->value;
                Value val = evaluate_expression(node->children[1]);
                set_variable(var_name, val);
                break;
            }
            case NodeType::IF_STATEMENT: {
                Value condition = evaluate_expression(node->children[0]);
                if (condition.is_truthy()) {
                    execute_statement(node->children[1]);
                } else if (node->children.size() > 2) { // Else clause exists
                    execute_statement(node->children[2]);
                }
                break;
            }
            case NodeType::WHILE_LOOP: {
                while (evaluate_expression(node->children[0]).is_truthy()) {
                    execute_statement(node->children[1]);
                }
                break;
            }
            case NodeType::FUNCTION_CALL:
                execute_function_call(node);
                break;
            default:
                throw std::runtime_error("Runtime Error: Invalid statement node.");
        }
    }

    void execute_block(ParseTree* block_node) {
        push_scope();
        for (ParseTree* statement : block_node->children) {
            execute_statement(statement);
        }
        pop_scope();
    }


    Value execute_function_call(ParseTree* node) {
        std::string func_name = node->value;
        if (func_name == "print") {
            for (size_t i = 0; i < node->children.size(); ++i) {
                evaluate_expression(node->children[i]).print();
                if (i < node->children.size() - 1) {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
            return Value(); // print returns null
        }
        throw std::runtime_error("Runtime Error: Undefined function '" + func_name + "'.");
    }


public:
    void interpret(ParseTree* root) {
        if (root == nullptr) return;
        if (root->type != NodeType::PROGRAM) {
            throw std::runtime_error("Interpreter Error: Root node must be a PROGRAM.");
        }
        // Global scope
        push_scope();
        execute_statement(root);
        pop_scope();
    }
};

// --- Main function to build a tree and run the interpreter ---
int main() {
    // Let's manually build a parse tree for the following code:
    //
    // x = 10;
    // y = 0;
    // while (x > 5) {
    //   y = y + x;
    //   x = x - 1;
    //   if (x == 7) {
    //     print("x is seven!");
    //   }
    // }
    // print("Final y:", y);

    auto* program = new ParseTree(NodeType::PROGRAM);

    // x = 10;
    auto* assign_x = new ParseTree(NodeType::ASSIGNMENT);
    assign_x->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    assign_x->children.push_back(new ParseTree(NodeType::INT_LITERAL, "10"));
    program->children.push_back(assign_x);

    // y = 0;
    auto* assign_y = new ParseTree(NodeType::ASSIGNMENT);
    assign_y->children.push_back(new ParseTree(NodeType::VARIABLE, "y"));
    assign_y->children.push_back(new ParseTree(NodeType::INT_LITERAL, "0"));
    program->children.push_back(assign_y);

    // while (x > 5) { ... }
    auto* while_loop = new ParseTree(NodeType::WHILE_LOOP);
    // Condition: x > 5
    auto* while_cond = new ParseTree(NodeType::BINARY_OP, ">");
    while_cond->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    while_cond->children.push_back(new ParseTree(NodeType::INT_LITERAL, "5"));
    while_loop->children.push_back(while_cond);

    // While block
    auto* while_block = new ParseTree(NodeType::BLOCK);
    // y = y + x
    auto* assign_y_loop = new ParseTree(NodeType::ASSIGNMENT);
    assign_y_loop->children.push_back(new ParseTree(NodeType::VARIABLE, "y"));
    auto* add_op = new ParseTree(NodeType::BINARY_OP, "+");
    add_op->children.push_back(new ParseTree(NodeType::VARIABLE, "y"));
    add_op->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    assign_y_loop->children.push_back(add_op);
    while_block->children.push_back(assign_y_loop);

    // x = x - 1
    auto* assign_x_loop = new ParseTree(NodeType::ASSIGNMENT);
    assign_x_loop->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    auto* sub_op = new ParseTree(NodeType::BINARY_OP, "-");
    sub_op->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    sub_op->children.push_back(new ParseTree(NodeType::INT_LITERAL, "1"));
    assign_x_loop->children.push_back(sub_op);
    while_block->children.push_back(assign_x_loop);

    // if (x == 7) { print("x is seven!"); }
    auto* if_stmt = new ParseTree(NodeType::IF_STATEMENT);
    auto* if_cond = new ParseTree(NodeType::BINARY_OP, "==");
    if_cond->children.push_back(new ParseTree(NodeType::VARIABLE, "x"));
    if_cond->children.push_back(new ParseTree(NodeType::INT_LITERAL, "7"));
    if_stmt->children.push_back(if_cond);
    auto* if_block = new ParseTree(NodeType::BLOCK);
    auto* print_if = new ParseTree(NodeType::FUNCTION_CALL, "print");
    print_if->children.push_back(new ParseTree(NodeType::STRING_LITERAL, "x is seven!"));
    if_block->children.push_back(print_if);
    if_stmt->children.push_back(if_block);
    while_block->children.push_back(if_stmt);

    while_loop->children.push_back(while_block);
    program->children.push_back(while_loop);

    // print("Final y:", y);
    auto* final_print = new ParseTree(NodeType::FUNCTION_CALL, "print");
    final_print->children.push_back(new ParseTree(NodeType::STRING_LITERAL, "Final y:"));
    final_print->children.push_back(new ParseTree(NodeType::VARIABLE, "y"));
    program->children.push_back(final_print);

    // Run the interpreter
    Interpreter interpreter;
    try {
        interpreter.interpret(program);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    // Clean up the manually allocated tree
    delete program;

    return 0;
}
