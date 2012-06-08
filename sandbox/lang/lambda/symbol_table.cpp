
#include <cassert>
#include "symbol_table.hpp"

using namespace std;

symbol* 
symbol_table::get(symbol_kind kind)
{
  auto i = kinds.find(kind);
  if (i != kinds.end())
    return i->second;
  else
    return nullptr;
}

symbol* 
symbol_table::get(symbol_kind kind) const
{
  return const_cast<symbol_table*>(this)->get(kind);
}


symbol* 
symbol_table::get(const string& str)
{
  auto i = strings.find(str);
  if (i != strings.end())
    return i->second;
  else 
    return nullptr;
}

symbol* 
symbol_table::get(const string& str) const
{
  return const_cast<symbol_table*>(this)->get(str);
}

symbol*
symbol_table::put(symbol_kind kind, string str)
{
  switch (kind) {
    case sym_identifier: {
      // FIXME: This is not terribly efficient. I'm computing the insertion
      // point for the symbol twice.
      auto i = strings.find(str);
      if (i == strings.end()) {
        symbol* s = new symbol{kind, str};
        strings[str] = s;
        return s;
      } else {
        return i->second;
      }
    }
    default: {
      assert(kinds.count(kind) == 0);
      assert(strings.count(str) == 0);

      symbol* s = new symbol{kind, str};
      kinds[kind] = s;
      strings[str] = s;
      return s;
    }
  }
}


symbol* 
symbol_table::get(const lchar* first, const lchar* last)
{
  return get(lstring(first, last));
}

symbol* 
symbol_table::get(const lchar* first, const lchar* last) const 
{ 
  return get(lstring(first, last));
}

symbol* 
symbol_table::put(symbol_kind kind, const lchar* first, const lchar* last)
{
  return put(kind, lstring(first, last));
}
