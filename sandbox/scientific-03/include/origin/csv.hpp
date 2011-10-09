// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CSV_HPP
#define ORIGIN_CSV_HPP

#include <cassert>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

namespace origin
{
  // The CSV error class represents an error that occurs when reading or
  // writing CSV data.
  class csv_error : std::runtime_error
  {
  public:
    csv_error(std::size_t row, std::size_t col, std::string const& what)
      : std::runtime_error(make_error(row, col, what))
    { }

  private:
    static std::string make_error(std::size_t row,
                                  std::size_t col,
                                  std::string const& what)
    {
      std::stringstream ss;
      ss << "csv error: " << row << ':' << col << ':' << what;
      return ss.str();
    }
  };

  // The CSV Row class defines a row of textual CSV data.
  template<typename T = std::string>
    class csv_row
    {
    public:

      bool empty() const { return data.size(); }
      std::size_t size() const { return data.size(); }

      // Return the nth tuple.
      T&       operator[](std::size_t n)       { return data[n]; }
      T const& operator[](std::size_t n) const { return data[n]; }

      // IO helpers
      template<typename Char, typename Traits>
        void read(std::basic_istream<Char, Traits>& is);

      template<typename Char, typename Traits>
        void write(std::basic_ostream<Char, Traits>& is);

    private:
        void read(std::string const& line, std::size_t& n);

    private:
      std::vector<T> data;  // Parsed elements
      std::size_t row;      // The row index
    };

  template<typename T>
    template<typename Char, typename Traits>
      void csv_row<T>::read(std::basic_istream<Char, Traits>& is)
      {
        ++row;
        std::string line;
        std::getline(is, line);

        std::size_t n = 0;
        while(n < line.size()) {
          read(line, n);
        }
      }

  // Read the current field from the given line. The field starts at position
  // n. After updating, the data will contain the read field, and n will be
  // the start of the next field (or past the end).
  template<typename T>
    void csv_row<T>::read(std::string const& line, std::size_t& n)
    {
      assert(( n < line.size() ));

      if(line[n] == '"') {
        // Find the enclosing quote and get the text in between.
        std::size_t p = line.find_first_of('"', n + 1);
        if(p == line.npos)
          throw csv_error(row, n, "unmatched quotation mark");
        data.push_back(line.substr(n + 1, p - n - 1));
        boost::trim(data.back());

        // Find ',' after the closing quote.
        p = line.find_first_of(',', p + 1);
        if(p == line.npos)
          p = line.size();
        n = p + 1;
      } else {
        // Otherwise, just scan for the next ',' and cache the characters
        // in between. If there is no next comma, then we're at the end
        std::size_t p = line.find_first_of(',', n + 1);
        if(p == line.npos)
          p = line.size();
        data.push_back(line.substr(n, p - n));
        boost::trim(data.back());
        n = p + 1;
      }
    }

  template<typename Char, typename Traits, typename T>
    inline std::basic_istream<Char, Traits>&
    operator>>(std::basic_istream<Char, Traits>& is, csv_row<T>& row)
    {
      row.read(is);
      return is;
    }


} // namespace origin


#endif
