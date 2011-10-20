// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <origin/utility.hpp>

#include <scifair/data/csv.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace origin;

// using namespace origin;

// The format type describes types of formats that ScienceFair knows how
// to import.
//
// TODO: We should be able to deal with pluggable formats.
enum format_t {
  fmt_unspecified,
  fmt_csv,   // Comma separated values
  fmt_tsv,   // Tab separated values
  fmt_wsf,    // Whitespace formatted tabular data
};

char const* format_name(format_t f)
{
  switch(f) {
    case fmt_unspecified: return "unspecified";
    case fmt_csv: return "csv";
    case fmt_tsv: return "tsv";
    case fmt_wsf: return "wsf";
  }
  throw logic_error("unreachable code");
}

struct options
{
  options()
    : format(fmt_unspecified), headers(false), force(false)
  { }

  format_t format;  // Input format?
  bool headers;     // First row as header data?
  bool force;       // Overwrite existing files?
};

// Command line parsing
format_t parse_format(string const& str);
format_t guess_format(path const& p);

// Data import
path create_file(path const& p);
void import_file(path const& arg);
void create_tsv_file(path const& arg);
void create_wsv_file(path const& arg);

// Global data.
static options opts;

int main(int argc, char *argv[])
{
  // FIXME: Extract the option parser. I want to support argument parsing
  // as a more general framework.
  vector<string> args;
  for(int i = 1; i < argc; ) {
    if(argv[i][0] == '-') {
      string arg = argv[i];
      if(arg == "-help") {
        ++i;
      } else if(arg == "-version") {
        ++i;
      } else if(arg == "-force") {
        opts.force = true;
        ++i;
      } else if(arg == "-format") {
        opts.format = parse_format(argv[++i]);
        ++i;
      } else {
        stringstream ss;
        ss << "error: unknown option: '" << arg << "'";
        throw std::runtime_error(ss.str());
      }

      // parse option
      i += 2; // skip it form now
    } else {
      args.push_back(argv[i]);
      ++i;
    }
  }
  
  if(args.size() > 1) {
    cerr << "warning: import will only create the first file\n";
  }
  
  path p = args[0];
  if(opts.format == fmt_unspecified) {
    cerr << "note: guessing format of " << p << "\n";
    opts.format = guess_format(p);
    cerr << "note: guessed '" << format_name(opts.format) << "'\n";
  }
  
  // Try to create file directory
  path dir = create_file(p);
  
  // Try to create a new science fair file.
  // FIXME: Wrap this in a a class?
  try {
    import_file(args.front());
  } catch(...) {
    return -1;
  }
  
  return 0;
}

// Parse the format specifier from the command line argument.
format_t parse_format(string const& str)
{
  if(str == "csv") return fmt_csv;
  else if(str == "tsv") return fmt_tsv;
  else if(str == "wsf") return fmt_wsf;
  else {
    stringstream ss;
    ss << "error: unknown file format '" << str << "'";
    throw runtime_error(ss.str());
  }
}

// Try to guess the format based on the file extension. 
format_t guess_format(path const& p)
{
  string ext = p.extension().string();
  if(ext == ".csv") return fmt_csv;
  else if(ext == ".txt") return fmt_wsf;
  else {
    stringstream ss;
    ss << "error: could not guess format of '" <<  ext << "'";
    throw runtime_error(ss.str());
  }
}

// Create a a ScienceFair file as "stem(p).science" where stem(p) is the path
// minus its current extension.
path create_file(path const& p)
{
  path d = p;
  d.replace_extension(".science");
  bool result = create_directory(d);
  
  // If we force an overwrite, then don't emit an error here.
  if(!result && !opts.force) {
    stringstream ss;
    ss << "error: could not create directory " << d;
    throw runtime_error(ss.str());
  }
  
  return d;
}

// Import the data file.
void import_file(path const& p)
{
  assert(( opts.format != fmt_unspecified ));

  typedef csv_row<string> Row;
  typedef vector<Row> Table;
  
  // Open the file.
  ifstream f(p.string().c_str());
  if(!f) {
    stringstream ss;
    ss << "error: could not read file " << p;
    throw runtime_error(ss.str());
  }

  // Read its contents
  Table t;
  Row r;
  if(opts.format == fmt_csv || fmt_tsv) {
    // FIXME: This is gross. I should be doing a better job with this. We
    // should be able to do arbitrary field separation.
    char sep = (opts.format == fmt_csv ? ',': '\t');
    while(f >> separated(r, sep)) {
      cout << separated(r, '\t') << "\n";
      t.push_back(r);
    }
  }
}
