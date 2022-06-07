#include "translator.h"

int main() {
    translator our_trans;
    bool lex = our_trans.lex_analysis("testpr.txt", "token.txt", "errors.txt");
    if (lex) {
        our_trans.parse("token.txt", "lex errors.txt");
        our_trans.out_tree("tree.txt");
        our_trans.generate_code("prog.asm");
    }
    system("pause");

    return 0;
}