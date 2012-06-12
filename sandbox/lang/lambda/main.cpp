
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
  sym.put(Symbol::Eof, "<eof>");
  sym.put(Symbol::Error, "<error>");

  // Register punctuation
  sym.put(Symbol::Lparen, "(");
  sym.put(Symbol::Rparen, ")");
  sym.put(Symbol::Backslash, "\\");
  sym.put(Symbol::Dot, ".");
  sym.put(Symbol::Semicolon, ";");
  sym.put(Symbol::Equal, "=");

  // Keywords
  sym.put(Symbol::Eval, "eval");
  sym.put(Symbol::Let, "let");


  // Read the input buffer.
  String buf;
  while (1) {
    Char txt[1024];
    cin.read(txt, 1024);
    buf.append(txt, txt + cin.gcount());
    if (!cin)
      break;
  }

  Lexer l{&sym, buf};
  Parser p{l};
  p();
}
