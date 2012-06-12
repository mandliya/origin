
#include <cassert>
#include "symbol_table.hpp"

using namespace std;

Symbol* 
symbol_table::get(Symbol::Kind kind)
{
  auto i = kinds.find(kind);
  if (i != kinds.end())
    return i->second;
  else
    return nullptr;
}

Symbol* 
symbol_table::get(Symbol::Kind kind) const
{
  return const_cast<symbol_table*>(this)->get(kind);
}


Symbol* 
symbol_table::get(const string& str)
{
  auto i = strings.find(str);
  if (i != strings.end())
    return i->second;
  else 
    return nullptr;
}

Symbol* 
symbol_table::get(const string& str) const
{
  return const_cast<symbol_table*>(this)->get(str);
}

Symbol*
symbol_table::put(Symbol::Kind kind, string str)
{
  switch (kind) {
    case Symbol::Identifier: {
      // FIXME: This is not terribly efficient. I'm computing the insertion
      // point for the symbol twice.
      auto i = strings.find(str);
      if (i == strings.end()) {
        Symbol* s = new Symbol{kind, str};
        strings[str] = s;
        return s;
      } else {
        return i->second;
      }
    }
    default: {
      assert(kinds.count(kind) == 0);
      assert(strings.count(str) == 0);

      Symbol* s = new Symbol{kind, str};
      kinds[kind] = s;
      strings[str] = s;
      return s;
    }
  }
}


Symbol* 
symbol_table::get(const Char* first, const Char* last)
{
  return get(String(first, last));
}

Symbol* 
symbol_table::get(const Char* first, const Char* last) const 
{ 
  return get(String(first, last));
}

Symbol* 
symbol_table::put(Symbol::Kind kind, const Char* first, const Char* last)
{
  return put(kind, String(first, last));
}
