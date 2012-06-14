
#include <cassert>
#include <string>
#include <iostream>

#include "symbol.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluation.hpp"
#include "sexpr.hpp"

using namespace std;

int main()
{
  // Initialize the language table.
  symbol_table sym;

  // Register pseudo symbols
  sym.put(Symbol::Eof, "<eof>");
  sym.put(Symbol::Error, "<error>");

  // Register punctuation
  sym.put(Symbol::Lparen, "(");
  sym.put(Symbol::Rparen, ")");
  sym.put(Symbol::Backslash, "\\");
  sym.put(Symbol::Dot, ".");
  sym.put(Symbol::Semicolon, ";");
  sym.put(Symbol::Equal, "=");

  /*
  // Read the input buffer.
  String buf;
  while (1) {
    Char txt[1024];
    cin.read(txt, 1024);
    buf.append(txt, txt + cin.gcount());
    if (!cin)
      break;
  }
  */

  // Set up program context.
  Context cxt;

  // Configure the lexer for the input.
  Lexer lex{sym, cin};

  /*
  // Dump the token stream.
  Token tok = lex();
  while (tok) {
    cout << tok << '\n';
    tok = lex();
  }
  */

  // Build a parser for the context.
  Parser parse{cxt, lex};
  
  // Parse and print.
  parse();

  Program* prog = cxt.program();

  for (auto s : prog->statements()) {
    if(Evaluation* eval = as<Evaluation>(s)) {
      Term* term = eval->term();
      
      // Try a bunch of different strategies
      Term* bn = call_by_name(cxt, term);
      Term* bv = call_by_value(cxt, term);
      Term* no = reduce_normal_order(cxt, term);
      
      cout << "init: " << sexpr(term) << '\n';
      cout << "bn ==> " << sexpr(bn) << '\n';
      cout << "bv ==> " << sexpr(bv) << '\n';
      cout << "no ==> " << sexpr(no) << '\n';
    }
  }
}
