// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_TESTING_HPP
#define ORIGIN_TYPE_TESTING_HPP

#include <functional>
#include <limits>
#include <random>

#include "type.hpp"

namespace origin
{
  namespace testing
  {
    // A type used to force ADL lookups in this namespace. 
    struct adl_t { };
  
  } // namespace testing

  //////////////////////////////////////////////////////////////////////////////
  // Testing
  //
  // This library contains features supporting the writing of unit tests
  // against components in this library. In particular, this library includes
  // archetypes for standard concepts (including the core framework), and
  // testable properties for those concepts' axioms.
  //////////////////////////////////////////////////////////////////////////////


// Support for randomized tessting.
#include "testing.impl/random.hpp"

// The basic testing context and support functions.
#include "testing.impl/context.hpp"

// Include test support for the type library.
#include "testing.impl/properties.hpp"
#include "testing.impl/concepts.hpp"

/*
// Include archetypes
#include "testing/archetypes.hpp"


  //////////////////////////////////////////////////////////////////////////////
  // Dummy Type
  //
  // The dummy type used by the constructor of the archetype class as an
  // indicator of a non-standard constructor. For example, an archetype of the
  // regular concept wrapping an int can be initialized using:
  //
  //    Regular_archetype<int> x {dummy, 0};
  //
  struct dummy_t { };


  //////////////////////////////////////////////////////////////////////////////
  // Archetype
  //
  // The archetype class wraps a given value, and exposes a specified set
  // of interface, indicated by a sequence of selector types.
  //
  // Note that T must be a complete type.
  template <typename T, typename... Sels>
    struct archetype : Sels::template impl<archetype<T, Sels...>, T>...
    {
      using This_type = archetype<T, Sels...>;
      using Value_type = T;

      // The archetype owns the interface-constrained value. Note that the
      // value can be moved into the archetype.
      T value;

      // FIXME: I could make this more effecient by aggregating flags instead
      // of recursing over sequences. Basically, this is deducing the set of
      // properties that will be enforced by the various constructors and
      // assignment operators.
      static constexpr bool Is_default_constructible = 
        Has_selector<use_default_constructible, Sels...>();
      
      static constexpr bool Is_move_constructible = 
        Has_selector<use_move_constructible, Sels...>();
      
      static constexpr bool Is_copy_constructible = 
        Has_selector<use_copy_constructible, Sels...>();

      static constexpr bool Is_movable = Has_selector<use_movable, Sels...>();
      static constexpr bool Is_copyable = Has_selector<use_copyable, Sels...>();

      // Initialization
      // This forwarding constructor allows the underlying value to be 
      // initialized over the given arguments. The dummy argument makes it
      // unlikely that this will be called by any algorithms.
      template <typename... Args>
        explicit archetype(dummy_t, Args&&... args)
          : value(std::forward<Args>(args)...)
        { }

      // Factory
      // This factory provides an interface for creating archetype values that
      // deprecates the need to use the dummy_t argument.
      template <typename... Args>
        static archetype create(Args&&... args)
        {
          return archetype(dummy_t {}, std::forward<Args>(args)...);
        }

      // Default initialization
      archetype()
        : value()
      {
        static_assert(Is_default_constructible, "");
      }

      // Movable
      archetype(archetype&& x)
        : value(std::move(x.value))
      { 
        // We allow this to be used for move and copy constructible types just
        // in case we found copy but not move.
        static_assert(Is_move_constructible || Is_copy_constructible, "");
      }

      archetype& operator=(archetype&& x)
      {
        // Copyable refines movable so we don't also need to check for Copyable.
        static_assert(Is_movable, "");
        value = std::move(x.value);
        return *this;
      }


      // Copyable
      archetype(const archetype& x)
        : value(x.value)
      {
        static_assert(Is_copy_constructible, "");
      }

      archetype& operator=(const archetype& x)
      {
        static_assert(Is_copyable, "");
        value = x.value;
        return *this;
      }
    };



  // Archetype constructors
  // These aliases make it easier to create basic archetypes.


  // Unconstrained archetype (archetype)
  // The unconstrained archetype creates types that are immovable, 
  // non-copyable, and not default constructible.
  template <typename T>
    using Unconstrained_archetype = archetype<T>;


  // Default constructible (archetype)
  // The default constructible archetype creates types that can only be
  // default constructed. They cannot be moved or copied.
  template <typename T>
    using Default_constructible_archetype = archetype<T, use_default_constructible>;


  // Move constructible (archetype)
  // The move constructible archetype creates types that are only move
  // move constructible.
  template <typename T>
    using Move_constructible_archetype = archetype<T, use_move_constructible>;


  // Copy constructible (archetype)
  // The copy constructible archetype creates types that are move
  // constructible and copy constructible.
  template <typename T>
    using Copy_constructible_archtype = archetype<T, use_copy_constructible>;


  // Movable (archetype)
  // The movable archetpye creates types that are move constructible and 
  // assignable type.
  template <typename T>
    using Movable_archetype = archetype<T, use_movable>;


  // Copyable (archetype)
  // The copyable archetype creates types that are movable, copy 
  //constructible, and copy assignable.
  template <typename T>
    using Copyable_archetype = archetype<T, use_copyable>;


  // FIXME: For the comparison archetypes, it might be useful to accept a
  // sequence of "other" types in order to construct a number of common
  // type relational operators in one shot. The ideal is:
  //
  //    Equality_comparable_archetype<Base, {int, T*}>
  //
  // where Base is the type being wrapped, and int and T* will eventually be
  // cross-type comparable with Base. Note that Base should be comparable
  // to itself since that is a requirement of any cross-type relation.

  template <typename T, typename U = default_t>
    using Equality_comparable_archetype = archetype<T, use_equality_comparable<U>>;

  template <typename T, typename U = default_t>
    using Weakly_ordered_archetype = archetype<T, use_weakly_ordered<U>>;

  template <typename T, typename U = default_t>
    using Totally_ordered_archetype = archetype<T, use_totally_ordered<U>>;

  template <typename T>
    using Regular_archetype = archetype<T, use_regular>;

  // FIXME: As with above, can we construct multiple call profiles with a
  // single using declaration? I would have to wrap different argument sets
  // in a tuple or type list.

  template <typename F, typename... Args>
    using Function_archetype = archetype<F, use_function<Args...>>;

  template <typename P, typename... Args>
    using Predicate_archetype = archetype<P, use_predicate<Args...>>;

  template <typename R, typename T, typename U = default_t>
    using Relation_archetype = archetype<R, use_relation<T, U>>;

*/

} // namespace origin


#endif
