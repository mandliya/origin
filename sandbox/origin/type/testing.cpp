
// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>

#include "testing.hpp"

namespace origin
{
  namespace testing
  {
    ////////////////////////////////////////////////////////////////////////////
    // Context Implementation

    context::context()
      : prng(), os(&std::cerr), repeat(100), fail(0)
    { }



    ////////////////////////////////////////////////////////////////////////////
    // Testable Implementation

    testable::testable(context& c)
      : cxt(c)
    { }


    ////////////////////////////////////////////////////////////////////////////
    // Relational Property Implementations

    reflexive::reflexive(context& cxt)
      : testable(cxt)
    { }

    irreflexive::irreflexive(context& cxt)
      : testable(cxt)
    { }

    symmetric::symmetric(context& cxt)
      : testable(cxt)
    { }

    asymmetric::asymmetric(context& cxt)
      : testable(cxt)
    { }

    antisymmetric::antisymmetric(context& cxt)
      : testable(cxt)
    { }

    transitive::transitive(context& cxt)
      : testable(cxt)
    { }

    trichotomous::trichotomous(context& cxt) 
      : testable(cxt) 
    { }

    equivalence::equivalence(context& cxt)
      : testable(cxt), refl(cxt), sym(cxt), trans(cxt)
    { }

    strict_ordering::strict_ordering(context& cxt)
      : testable(cxt), irrefl(cxt), asym(cxt), trans(cxt)
    { }

    strict_weak_ordering::strict_weak_ordering(context& cxt) 
      : testable(cxt), ord(cxt), eq(cxt) 
    { }

    strict_total_ordering::strict_total_ordering(context& cxt) 
      : testable(cxt), trans(cxt), tri(cxt)
     { }




  } // namespace tessting
} // namespace origin


