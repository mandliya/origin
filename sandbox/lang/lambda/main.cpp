
#include <cassert>
#include <string>
#include <iostream>

#include "symbol.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;


int main()
{
  // Initialize the language table.
  symbol_table sym;

  // Register pseudo symbols
  sym.put(sym_eof, "<eof>");
  sym.put(sym_error, "<error>");

  // Register punctuation
  sym.put(sym_lparen, "(");
  sym.put(sym_rparen, ")");
  sym.put(sym_backslash, "\\");
  sym.put(sym_dot, ".");


  // Read the input buffer.
  lstring buf;
  while (1) {
    lchar txt[1024];
    cin.read(txt, 1024);
    buf.append(txt, txt + cin.gcount());
    if (!cin)
      break;
  }

  lexer l{&sym, buf};

  /*
  while (1) {
    token tok = l();
    if (!tok.sym)
      break;
    else
      std::cout << tok << '\n';
  }
  */

  parser p{l};
  p();
}
