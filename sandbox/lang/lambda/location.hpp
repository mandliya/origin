
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iosfwd>

// Location
//
// The location class enacpsulates the notion of a source location. It encodes
// the file, line number, and column number of a token. 
//
// TODO: Implement support for files. Probably just a pointer or reference to a
// file object for the time being.
struct Location
{
  Location() 
    : line{-1}, column{-1} 
  { }

  Location(int l, int c)
    : line{l}, column{c}
  { }

  int line;
  int column;
};



// Streaming
// Write the location to the output stream.
template <typename C, typename T>
  inline std::basic_ostream<C, T>& 
  operator<<(std::basic_ostream<C, T>& os, const Location& loc)
  {
    return os << loc.line << ":" << loc.column;
  }

#endif
