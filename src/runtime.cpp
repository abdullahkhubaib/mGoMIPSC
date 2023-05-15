//
// Created by abdul on 2023-05-15.
//

#include "../include/runtime.h"

const std::string runtime::header = "        .text\n"
                              "        .globl main\n"
                              "main:\n"
                              "        jal Lmain\n"
                              "Phalt:\n"
                              "        li $v0, 10\n"
                              "        syscall\n";

// Hashmap defining pre-defined functions.
const std::unordered_map<std::string, std::string> runtime::funcs = {
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

void runtime::include(const std::string& name) {
    used.emplace(name);
}

std::string runtime::get() {
    std::stringstream ss;
    // Halt is already defined in the runtime.
    used.erase("Phalt");
    // Define only the runtime functions that are needed.
    for(const std::string& p: used)
        ss << funcs.at(p);
    return ss.str();
}
