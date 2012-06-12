
#include <cassert>
#include <string>
#include <iostream>

#include "symbol.hpp"
#include "lexer.hpp"
#include "parser.hpp"

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

  // Read the input buffer.
  String buf;
  while (1) {
    Char txt[1024];
    cin.read(txt, 1024);
    buf.append(txt, txt + cin.gcount());
    if (!cin)
      break;
  }

  // Set up program context.
  Context cxt;
  
  // Configure the lexer for the input.
  Lexer lex{sym, buf};

  // Build a parser for the context.
  Parser parse{cxt, lex};
  
  // Parse and print.
  parse();
  for (auto s : cxt.stmts)
    cout << sexpr(s);
}
