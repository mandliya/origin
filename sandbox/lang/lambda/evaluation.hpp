
#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <iosfwd>

#include "syntax.hpp"
#include "context.hpp"
#include "identifier_table.hpp"


// Reduce the term using the call-by-value strategy.
Term* step_by_value(Context& cxt, Term* term);
Term* call_by_value(Context& cxt, Term* term);

// Reduce the term using the call-by-name strategy.
Term* step_by_name(Context& cxt, Term* term);
Term* call_by_name(Context& cxt, Term* term);

// Reduce the term using a normal order reduction.
Term* step_normal_order(Context& cxt, Term* term);
Term* reduce_normal_order(Context& cxt, Term* term);

#endif
