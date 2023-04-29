//
// Created by abdul on 2023-03-27.
//

#include "../include/gen.h"

// Hashmap defining pre-defined functions.
const std::unordered_map<std::string, std::string> gen::predef = {
        {"Pprints", "Pprints:\n"
                    "        li $v0, 4\n"
                    "        syscall\n"
                    "        jr $ra\n"},
        {"Pprinti", "Pprinti:\n"
                    "        li $v0, 1\n"
                    "        syscall\n"
                    "        jr $ra\n"},
        {"Pprintc", "Pprintc:\n"
                    "        li $v0, 11\n"
                    "        syscall\n"
                    "        jr $ra\n"},
        {"Pprintb", "Pprintb:\n"
                    "        li $v0, 4\n"
                    "        beqz $a0, P1printb\n"
                    "        la $a0, Ptrue\n"
                    "        j P2printb\n"
                    "P1printb:\n"
                    "        la $a0, Pfalse\n"
                    "P2printb:\n"
                    "        syscall\n"
                    "        jr $ra\n"
                    "        .data\n"
                    "Ptrue:\n"
                    "        .asciiz \"true\"\n"
                    "Pfalse:\n"
                    "        .asciiz \"false\"\n"
                    "        .text\n"},
        {"Plen",    "Plen:\n"
                    "        subu $sp, $sp, 8\n"
                    "        sw $t0, 0($sp) # address of char\n"
                    "        sw $t1, 4($sp) # current char\n"
                    "        move $t0, $a0\n"
                    "P1len:\n"
                    "        lb $t1, 0($t0)\n"
                    "        beqz $t1, P2len\n"
                    "        addi $t0, $t0, 1\n"
                    "        j P1len\n"
                    "P2len:\n"
                    "        subu $v0, $t0, $a0 # end - start\n"
                    "        lw $t0, 0($sp)\n"
                    "        lw $t1, 4($sp)\n"
                    "        addu $sp, $sp, 8\n"
                    "        jr $ra\n"},
        {"Pgetchar","Pgetchar:\n"
                    "        subu $sp, $sp, 8\n"
                    "        sw $a0, 0($sp)\n"
                    "        sw $a1, 4($sp)\n"
                    "        li $v0, 8\n"
                    "        la $a0, Pinput\n"
                    "        li $a1, 2\n"
                    "        syscall\n"
                    "        lb $v0, Pinput\n"
                    "        bnez $v0, P1getchar\n"
                    "        li $v0, -1\n"
                    "        .data\n"
                    "Pinput: .space 2\n"
                    "        .align 2\n"
                    "        .text\n"
                    "P1getchar:\n"
                    "        lw $a0, 0($sp)\n"
                    "        lw $a1, 4($sp)\n"
                    "        addu $sp, $sp, 8\n"
                    "        jr $ra\n"},
        {"Perror",  "Perror:\n"
                    "        li $v0, 4\n"
                    "        syscall\n"
                    "        li $v0, 17\n"
                    "        li $a0, 1\n"
                    "        syscall\n"},
        {"PDMChk",  "PDMChk:\n"
                    "        beqz $a1, P2DMChk\n"
                    "        bne $a0, -2147483648, P3DMChk\n"
                    "        bne $a1, -1, P3DMChk\n"
                    "        li $v0, 1\n"
                    "        jr $ra\n"
                    "        .data\n"
                    "P1DMChk:\n"
                    "        .asciiz \"error: division by zero\\n\"\n"
                    "        .text\n"
                    "P2DMChk:\n"
                    "        la $a0, P1DMChk\n"
                    "        j Perror\n"
                    "P3DMChk:\n"
                    "        move $v0, $a1\n"
                    "        jr $ra\n"}
};

gen::gen(Node *AST): AST(AST), strings{{"", "S0"}} {}


std::string gen::generate() {
    std::stringstream out;
    out << "        .text\n"
           "        .globl main\n"
           "main:\n"
           "        jal Lmain\n"
           "Phalt:\n"
           "        li $v0, 10\n"
           "        syscall\n";

    std::stringstream ss;
    // Statically allocate global variables and define function labels
    for(Node* n : AST->children) {
        if(n->type == GLOBVAR_DEF) {
            if(glob_vars.empty())
                ss << emit(".data");
            // Generate a label for the global variable and allocate it.
            ss << emit_label(glob_vars.emplace(n->sym, "G" + std::to_string(glob_vars.size())).first->second);
            // Default for strings is an empty string and 0 for everything else.
            if(n->children[1]->attr == "string")
                ss << emit(".word S0");
            else
                ss << emit(".word 0");
        } else if(n->type == FUNC_DEF)
            // Store the starting label for all functions.
            funcs.emplace(n->sym, "L" + n->children[0]->attr);
    }
    if(!glob_vars.empty())
        ss << emit(".text");

    // Generate code for all functions in order that they are defined
    for(Node* n : AST->children)
        if(n->type == FUNC_DEF)
            gen_func(n, ss);


    // Halt is already defined in the runtime.
    predef_used.erase("Phalt");

    // Define only the runtime functions that are needed.
    for(const std::string& p: predef_used)
        out << predef.at(p);

    // Add function definitions to the output.
    out << ss.str();

    // Define all the string literals.
    out << emit(".data");
    // Sort the strings in lexical order to make comparisons easier.
    std::vector<std::pair<std::string, std::string>> s_strings(strings.size());
    std::copy(strings.begin(), strings.end(), s_strings.begin());
    std::sort(s_strings.begin(), s_strings.end());
    for(auto& [str, label]: s_strings) {
        out << emit_label(label);
        std::stringstream chars;
        for(const char& c : str)
            chars << std::to_string(c) << ", ";
        out << emit(".byte " + chars.str() + "0");
    }
    out << emit(".align 2");
    return out.str();
}

void gen::gen_func(Node *func, std::stringstream &ss) {
    int lNum = 0;
    bool isLeaf = true;
    bool isVoid = func->children[1]->children[1]->attr == "$void";
    std::stringstream body;
    // Local variables are register allocated and a total of 9 can be created.
    local_vars.clear();
    local_vars.reserve(9);
    // 9 registers are used for general purpose use.
    registers = std::vector<bool>(9);
    ss << emit_label("L" + func->children[0]->attr);

    // Basic implementation of constant folding for addition, subtraction and multiplication.
    func->postorder([](Node* n, bool running) {
        if((n->type == ADD_EXPR || n->type == SUBTRACT_EXPR || n->type == MULTIPLY_EXPR) &&
           (n->children[0]->type == INT_T && n->children[1]->type == INT_T)) {
            auto op1 = std::make_unsigned_t<int>(std::stoi(n->children[0]->attr));
            auto op2 = std::make_unsigned_t<int>(std::stoi(n->children[1]->attr));
            unsigned int res = 0;
            switch (n->type) {
                case ADD_EXPR:
                    res = op1 + op2;
                    break;
                case SUBTRACT_EXPR:
                    res = op1 - op2;
                    break;
                case MULTIPLY_EXPR:
                    res = op1 * op2;
                default:
                    break;
            }
            n->type = INT_T;
            n->attr = std::to_string(static_cast<int>(res));
            n->children.clear();
        }
    });

    // A pre-scan to optimize the tree and to allocate registers for local variables.
    func->preorder([this, &isLeaf](Node* n, bool running){
        switch(n->type) {
            // Add all the local variables to a hashmap and assign registers for them.
            case VAR_DEF:
                if(local_vars.size() < 8)
                    local_vars.emplace(n->sym, "$s" + std::to_string(local_vars.size()));
                else
                    local_vars.emplace(n->sym, "$t" + std::to_string(17 - local_vars.size()));
                break;
            case FUNCCALL:
                isLeaf = false;
                break;
            // Optimizes the tree to minimize register usage. Works by flipping the operands of commutative operators
            // to create a more left heavy tree. Left register is always reused before right and as a result, a left
            // heavy tree is more reuse friendly.
            case ADD_EXPR:
            case MULTIPLY_EXPR:
            case EQUAL_EXPR:
            case NOT_EQUAL_EXPR:
                if(n->children[0]->max_depth() < n->children[1]->max_depth())
                    std::swap(n->children[0], n->children[1]);
            default:
                break;
        }
    });


    //func->display();

    // Add function arguments to the hashmap
    int arg_i = 0;
    for(Node* arg: func->children[1]->children[0]->children)
        local_vars.emplace(arg->children[0]->sym, "$a" + std::to_string(arg_i++));

    // Body
    gen_block(func->children[2], body, func, lNum);

    // Function does not return a value.
    if(!isVoid) {
        predef_used.emplace("Perror");
        std::string message = "error: function '" + func->children[0]->attr + "' must return a value\n";
        body << emit("la $a0, " + stoLabel(message));
        body << emit("j Perror");
    }
    // Prologue
    ss << emit("# prologue:");
    const std::string& b = body.str();
    // Find out what t and s registers are being used and save them to the stack.
    std::regex reg("\\$(t|s)\\d");
    // std::set allows automatic sorting and ensures that every element is unique.
    std::set<std::string> matches(std::sregex_token_iterator(b.begin(), b.end(), reg), std::sregex_token_iterator());

    std::vector<std::string> store(matches.begin(), matches.end());
    // Verifies if the function is a leaf function.
    if(isLeaf && b.find("jal") != std::string::npos)
        isLeaf = false;
    if(!isLeaf)
        store.insert(store.begin(), "$ra");
    // Store previous register values on stack. 4 bytes for every register
    if(!store.empty())
        ss << emit("subu $sp, $sp, " + std::to_string(store.size() * 4));
    for(int i = 0; i < (int) store.size(); i++)
        ss << emit("sw " + store[i] + ", " + std::to_string(i * 4) + "($sp)");

    ss << emit("# body");
    ss << b;

    // Epilogue
    ss << emit_label("R" + func->children[0]->attr);
    ss << emit("# epilogue:");
    for(int i = (int) store.size() - 1; i >= 0; i--)
        ss << emit("lw " + store[i] + ", " + std::to_string(i * 4) + "($sp)");
    ss << emit("addu $sp, $sp, " + std::to_string(store.size() * 4));
    ss << emit("jr $ra");
}


// Generates code for the block and outputs it to the provided stream.
void gen::gen_block(Node *block, std::stringstream &ss, Node *func, int &lNum) {
    for(Node* statement : block->children) {
        switch (statement->type) {
            case VAR_DEF:
                // Initialize to zero/empty string
                if(statement->children[1]->attr == "string")
                    ss << emit("la " + local_vars.at(statement->sym) + ", S0");
                else
                    ss << emit("move " + local_vars.at(statement->sym) + ", $zero");
                break;
            case ASSIGN_EXPR:
                gen_short_circuit(statement->children[1], ss, func, lNum);
                if(local_vars.count(statement->children[0]->sym))
                    ss << emit("move " + local_vars.at(statement->children[0]->sym) + ", " + statement->children[1]->sig);
                else
                    ss << emit("sw " + statement->children[1]->sig + ", " + glob_vars.at(statement->children[0]->sym));
                free_register(statement->children[1]->sig);
                break;
            case EXPR_STMT:
                gen_expression(statement->children[0], ss);
                free_register(statement->children[0]->sig);
                break;
            case RETURN_STMT:
                if(!statement->children.empty()) {
                    gen_expression(statement->children[0], ss);
                    ss << emit("move $v0, " + statement->children[0]->sig);
                    free_register(statement->children[0]->sig);
                }
                ss << emit("j R" + func->children[0]->attr);
                break;
            case IF_STMT:
            case IFELSE_STMT:
                gen_if_else(statement, ss, func, lNum, "");
                break;
            case FOR_STMT:
                gen_for(statement, ss, func, lNum);
                break;
            case BLOCK:
                gen_block(statement, ss, func, lNum);
                break;
            case BREAK_STMT:
                ss << emit("j " + loop_end.top());
                break;
            default:
                break;
        }
    }
}

// op_to_i[OP->type - 24] = instruction for that binary operator.
static const std::string op_to_i[] = {
        "addu", "subu", "mul", "div", "rem", "and", "or", "", "seq", "sne", "slt", "sgt",
        "", "sle", "sge"
};

// Parses the expression in a bottom-up manner and generates code for it.
void gen::gen_expression(Node *expression, std::stringstream &ss) {
    expression->postorder([this, &ss](Node* n, bool running) {
        switch(n->type) {
            // Loads literals into registers.
            case INT_T:
                n->sig = get_register();
                ss << emit("li " + n->sig + ", " + n->attr);
                break;
            case STRING_T:
                n->sig = get_register();
                ss << emit("la " + n->sig + ", " + stoLabel(n->attr));
                break;
            case BOOL_T:
                n->sig = get_register();
                ss << emit("li " + n->sig + ", " + ((n->attr == "false") ? "0" : "1"));
                break;
            case ID_T:
                if(local_vars.count(n->sym))
                    n->sig = local_vars.at(n->sym);
                else if(glob_vars.count(n->sym)) {
                    n->sig = get_register();
                    ss << emit("lw " + n->sig + ", " + glob_vars.at(n->sym));
                }
                break;
            // Handle unary operators.
            case UNARY_MINUS:
                n->sig = n->children[0]->sig;
                ss << emit("negu " + n->sig +", " + n->sig);
                break;
            case NOT_EXPR:
                n->sig = n->children[0]->sig;
                ss << emit("xori " + n->sig +", " + n->sig + ", 1");
                break;
            // Stores the current a-registers onto the stack and loads the new value into it.
            case ACTUALS:
                if(!n->children.empty()) {
                    ss << emit("subu $sp, $sp, " + std::to_string(4 * n->children.size()));
                    for(int i = 0; i < (int) n->children.size(); i++) {
                        ss << emit("sw $a" + std::to_string(i) + ", " + std::to_string(4 * i) + "($sp)");
                        ss << emit("move $a" + std::to_string(i) + ", " + n->children[i]->sig);
                        free_register(n->children[i]->sig);
                    }
                }
                break;
            case FUNCCALL:
                // if function definition is not defined in the file then it's assumed to be pre-defined.
                if(const auto& f = funcs.find(n->children[0]->sym); f != funcs.end())
                    ss << emit("jal " + f->second);
                else {
                    predef_used.emplace("P" + n->children[0]->attr);
                    ss << emit("jal P" + n->children[0]->attr);
                }
                // Retrieves the arguments from the stack after the function call returns
                if(auto& args = n->children[1]->children; !args.empty()) {
                    for(int i = (int) args.size() - 1; i >= 0; i--)
                        ss << emit("lw $a" + std::to_string(i) + ", " + std::to_string(4 * i) + "($sp)");
                    ss << emit("addu $sp, $sp, " + std::to_string(4 * args.size()));
                }
                if(n->sig != "void") {
                    n->sig = get_register();
                    ss << emit("move " + n->sig + ", $v0");
                }
                break;
            // Handle division by 0 errors and other division/mod checks. Falls through to evaluate the operator.
            case DIVIDE_EXPR:
            case MOD_EXPR:
                predef_used.emplace("PDMChk");
                predef_used.emplace("Perror");
                ss << emit("subu $sp, $sp, 8");
                ss << emit("sw $a0, 0($sp)");
                ss << emit("sw $a1, 4($sp)");
                ss << emit("move $a0, " + n->children[0]->sig);
                ss << emit("move $a1, " + n->children[1]->sig);
                ss << emit("jal PDMChk");
                ss << emit("move " + n->children[1]->sig + ", $v0");
                ss << emit("lw $a1, 4($sp)");
                ss << emit("lw $a0, 0($sp)");
                ss << emit("addu $sp, $sp, 8");
            default:
                // Handle all binary operators and reuses registers if possible. Tries reusing left first and then right.
                // Using the following format: instruction $t0, $t1, $t2
                if (n->type >= 24) {
                    free_register(n->children[0]->sig);
                    free_register(n->children[1]->sig);
                    n->sig = get_register();
                    ss << emit(op_to_i[n->type - 24] + " " + n->sig + ", " + n->children[0]->sig + ", " + n->children[1]->sig);
                }
        }
    });

}

// Parses AND and OR expressions in a top-down manner and generates code with short-circuiting.
void gen::gen_short_circuit(Node *n, std::stringstream &ss, Node *func, int &lNum) {
    if(n->type != AND_EXPR && n->type != OR_EXPR) {
        gen_expression(n, ss);
        return;
    }
    std::string L = "L" + std::to_string(++lNum) + func->children[0]->attr;
    gen_short_circuit(n->children[0], ss, func, lNum);
    // Skips the second if first is false for AND and true for OR.
    if(n->type == AND_EXPR)
        ss << emit("beqz " + n->children[0]->sig + ", " + L);
    else
        ss << emit("bnez " + n->children[0]->sig + ", " + L);
    gen_short_circuit(n->children[1], ss, func, lNum);
    n->sig = n->children[0]->sig;
    free_register(n->children[1]->sig);
    ss << emit(op_to_i[n->type - 24] + " " + n->sig + ", " + n->children[0]->sig + ", " + n->children[1]->sig);
    ss << emit_label(L);
}

// Uses recursion to generate code for if else chains.
void gen::gen_if_else(Node *statement, std::stringstream &ss, Node *func, int &lNum, std::string endL) {
    gen_short_circuit(statement->children[0], ss, func, lNum);
    free_register(statement->children[0]->sig);
    std::stringstream ss_block;
    gen_block(statement->children[1], ss_block, func, lNum);
    std::string elseL = "L" + std::to_string(++lNum) + func->children[0]->attr;
    ss << emit("beqz " + statement->children[0]->sig + ", " + elseL);
    ss << ss_block.str();

    if(statement->type == IFELSE_STMT) {
        std::stringstream else_block;
        bool first = false;
        if(endL.empty()) {
            first = true;
            endL = "L" + std::to_string(++lNum) + func->children[0]->attr;
        }

        if(statement->children[2]->type == BLOCK)
            gen_block(statement->children[2], else_block, func, lNum);
        else
            gen_if_else(statement->children[2], else_block, func, lNum, endL);

        ss << emit("j " + endL);
        ss << emit_label(elseL);
        ss << else_block.str();
        if(first)
            ss << emit_label(endL);
    } else {
        ss << emit_label(elseL);
    }
}

// Handles all for-loop code generation. Uses a stack to handle break statements for nested loops.
void gen::gen_for(Node *statement, std::stringstream &ss, Node *func, int &lNum)  {
    std::string startL = "L" + std::to_string(++lNum) + func->children[0]->attr;
    ss << emit_label(startL);
    std::string endL = "L" + std::to_string(++lNum) + func->children[0]->attr;
    loop_end.emplace(endL);
    if(statement->children[0]->attr != "$true") {
        gen_short_circuit(statement->children[0], ss, func, lNum);
        free_register(statement->children[0]->sig);
        ss << emit("beqz " + statement->children[0]->sig + ", " + endL);
    }
    gen_block(statement->children[1], ss, func, lNum);
    ss << emit("j " + startL);
    ss << emit_label(endL);
    loop_end.pop();
}

// Finds the first free t-register, allocates it and returns a string containing the name of that register.
std::string gen::get_register() {
    for(int i = 0; i < (int) registers.size(); i++) {
        if(!registers[i]) {
            registers[i] = true;
            return "$t" + std::to_string(i);
        }
    }
    error("expression too complicated");
    return "";
}

// If the provided register is a t-register than it is deallocated.
void gen::free_register(const std::string &s) {
    if(s[0] == '$' && s[1] == 't')
        registers[s[2] - '0'] = false;
}

// Adds a colon and a newline character.
std::string gen::emit_label(const std::string& label) {
    return label + ":\n";
}

// Prints with an indentation of 8 and ends it with a newline character.
std::string gen::emit(const std::string& str) {
    return "        " + str + "\n";
}

// Retrieves the label for string literals and creates one if it's not in already.
const std::string& gen::stoLabel(std::string& string) {
    if(auto label = strings.find(string); label != strings.end())
        return label->second;
    // Insert and return the label assigned to the string.
    return strings.emplace(string, "S" + std::to_string(strings.size())).first->second;
}











