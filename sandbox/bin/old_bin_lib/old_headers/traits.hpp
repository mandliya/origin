
#ifndef TRAITS_FUNC_HPP
#define TRAITS_FUNC_HPP


#include <algorithm>

// TODO We can reduce dependencies on these tags since they exist in different
// places. In Boost, they're in pending/container_traits.hpp. In origin, they're
// in the origin/containers module. That means we can get rid of the following
// include statements.

#include <vector>
#include <list>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <set>
#include <unordered_map>

#include <boost/next_prior.hpp>
#include <boost/function.hpp> // added by Brian

// tag classes.

    /** A Container is an object that stores other objects (its elements), and
     * that has methods for accessing its elements. In particular, every type
     * that is a model of Container has an associated iterator type that can be
     * used to iterate through the Container's elements.
     */
    struct container_tag { };

    /** A Forward Container is a Container whose elements are arranged in a
     * definite order: the ordering will not change spontaneously from iteration
     * to iteration. The requirement of a definite ordering allows the
     * definition of element-by-element equality (if the container's element
     * type is Equality Comparable) and of lexicographical ordering
     *(if the container's element type is LessThan Comparable).
     */
    struct forward_container_tag
        : virtual public container_tag { };

    /** Bidirectionaly iterable container */
    struct reversible_container_tag
        : virtual public forward_container_tag { };

    /** A container for which indexing is done in constant time. */
    struct random_access_container_tag
        : virtual public reversible_container_tag { };

    /** A Sequence is a variable-sized Container whose elements are arranged in
     * a strict linear order. It supports insertion and removal of elements.
     */
    struct sequence_tag
        : virtual public forward_container_tag { };

    /** An Associative Container is a variable-sized Container that supports
     * efficient retrieval of elements (values) based on keys. It supports
     * insertion and removal of elements, but differs from a Sequence in that
     * it does not provide a mechanism for inserting an element at a specific
     * position.
     */
    struct associative_container_tag
        : virtual public forward_container_tag { };

    /** A Sorted Associative Container is a type of Associative Container.
     * Sorted Associative Containers use an ordering relation on their keys; two
     * keys are considered to be equivalent if neither one is less than the
     * other. (If the ordering relation is case-insensitive string comparison,
     * for example, then the keys "abcde" and "aBcDe" are equivalent.)
     */
    struct sorted_associative_container_tag
        : virtual public associative_container_tag,
          virtual public reversible_container_tag { };

    /** A Front Insertion Sequence is a Sequence where it is possible to insert
     * an element at the beginning, or to access the first element, in amortized
     * constant time. Front Insertion Sequences have special member functions as
     * a shorthand for those operations.
     */
    struct front_insertion_sequence_tag
        : virtual public sequence_tag { };

    /** A Back Insertion Sequence is a Sequence where it is possible to append
     * an element to the end, or to access the last element, in amortized
     * constant time. Back Insertion Sequences have special member functions as
     * a shorthand for those operations.
     */
    struct back_insertion_sequence_tag
        : virtual public sequence_tag { };

    /** A Unique Associative Container is an Associative Container with the
     * property that each key in the container is unique: no two elements in a
     * Unique Associative Container have the same key.
     */
    struct unique_associative_container_tag
        : virtual public associative_container_tag { };

    /** A Multiple Associative Container is an Associative Container in which
     * there may be more than one element with the same key. That is, it is an
     * Associative Container that does not have the restrictions of a Unique
     * Associative Container.
     */
    struct multiple_associative_container_tag
        : virtual public associative_container_tag { };

    /** A Simple Associative Container is an Associative Container where
     * elements are their own keys. A key in a Simple Associative Container is
     * not associated with any additional value.
     */
    struct simple_associative_container_tag
        : virtual public associative_container_tag { };

    /** A Pair Associative Container is an Associative Container that associates
     * a key with some other object. The value type of a Pair Associative
     * Container is pair<const key_type, data_type>.
     */
    struct pair_associative_container_tag
        : virtual public associative_container_tag { };

    /** Stable iterators are iterators whose values don't become invalid on a
     * resize.
     */
    struct stable_tag { };

    /** Unstable iterators are iterators whose values become invalid on a
     * resize.
     */
    struct unstable_tag { };

    /** The tag vector has associated random acces container tag and back
     * insertion sequence tag.
     */
    struct vector_tag
        : virtual public random_access_container_tag,
          virtual public back_insertion_sequence_tag { };

    /** List tag returned with tag dispatch container_category is called given
     * a list.
     * Inherits reversible container tag and back insertion sequence tag
     * Notice: list is also a front insertion sequence but there are problems
     * with ambiguity when push_dispatch is called.
     */
    struct list_tag
        : virtual public reversible_container_tag,
          virtual public back_insertion_sequence_tag
    { };

    /** Tag associated with set and its concepts.
     * Concepts include:
     * sorted associative container
     * simple associative container
     * unique associative container
     */
    struct set_tag
        : virtual public sorted_associative_container_tag,
          virtual public simple_associative_container_tag,
          virtual public unique_associative_container_tag
    { };

    /** The tag corosponding to std::multiset and the concepts associated with
     * it.
     * Included Concepts:
     * sorted associative container
     * simple associative container
     * multiple associative container
     */
    struct multiset_tag
        : virtual public sorted_associative_container_tag,
          virtual public simple_associative_container_tag,
          virtual public multiple_associative_container_tag
    { };

    /** Corosponds to std::map.*/
    struct map_tag
        : virtual public sorted_associative_container_tag,
          virtual public pair_associative_container_tag,
          virtual public unique_associative_container_tag
    { };

    /** Corosponds to std::multimap.*/
    struct multimap_tag
        : virtual public sorted_associative_container_tag,
          virtual public pair_associative_container_tag,
          virtual public multiple_associative_container_tag
    { };

    /** The tag corosponding to a container adaptor.
     * This is a concept which matches to containers of elements where the
     * internal implimentation is based off of another data structure.
     */
    struct adaptor_tag
    { };

    /** A concept for adaptor of containers who support the enqueue operation.
     * includes the concept adaptor tag.
     */
    struct enqueueable_adaptor_tag
        : virtual public adaptor_tag
    {};

    /** Supports the adaptor deque.
     * includes concept back insertion sequence.
     */
    struct deque_adaptor_tag
        : virtual public back_insertion_sequence_tag
    { };

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * This one matches std::vector.
     */
    template <class T, class Alloc>
    vector_tag container_category(const std::vector<T,Alloc>&)
    { return vector_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::list.
     */
    template <class T, class Alloc>
    list_tag container_category(const std::list<T,Alloc>&)
    { return list_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::set.
     */
    template <class Key, class Cmp, class Alloc>
    set_tag container_category(const std::set<Key,Cmp,Alloc>&)
    { return set_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::multiset.
     */
    template <class Key, class Cmp, class Alloc>
    multiset_tag container_category(const std::multiset<Key,Cmp,Alloc>&)
    { return multiset_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to queue.
     */
    template <class T, class Alloc>
    enqueueable_adaptor_tag container_category(const std::queue<T,Alloc>&)
    { return enqueueable_adaptor_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::deque.
     */
    template <class T, class Alloc>
    deque_adaptor_tag container_category(const std::deque<T,Alloc>&)
    { return deque_adaptor_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to a stack
     */
    template <class T, class Alloc>
    adaptor_tag container_category(const std::stack<T,Alloc>&)
    { return adaptor_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::map.
     */
    template <class Key, class T, class Cmp, class Alloc>
    map_tag container_category(const std::map<Key,T,Cmp,Alloc>&)
    { return map_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::multimap.
     */
    template <class Key, class T, class Cmp, class Alloc>
    multimap_tag container_category(const std::multimap<Key,T,Cmp,Alloc>&)
    { return multimap_tag(); }

    /** Container traits is the function which match type and
     * returns the corosponding tag for tag dispatch.
     * matches to std::map.
     */
    template <class Key, class T, class Eq, class Hash, class Alloc>
    map_tag container_category(const std::unordered_map<Key,T,Eq,Hash,Alloc>&)
    { return map_tag(); }

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch.
     * tag match: adaptor tag
     */
    template <class Container, class T>
    void push_dispatch(Container& c, const T& x, adaptor_tag )
    {c.push(x);}

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch.
     * tag match: back_insertion_sequence_tag
     */
    template <class Container, class T>
    void push_dispatch(Container& c, const T& v, back_insertion_sequence_tag)
    {c.push_back(v);}

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch.
     * tag match: front_insertion_sequence_tag
     */
    template <class Container, class T>
    void push_dispatch(Container& c, const T& v, front_insertion_sequence_tag)
    {c.push_front(v);}

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch.
     * tag match: unique_associative_container_tag
     */
    template <class AssociativeContainer, class T>
    void push_dispatch(AssociativeContainer& c, const T& v, unique_associative_container_tag)
    {c.insert(v);}

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch. movable
     * tag match: unique_associative_container_tag
     */
    template <class AssociativeContainer, class T>
    void push_dispatch(AssociativeContainer& c, T&& v, unique_associative_container_tag)
    {c.insert(v);}

    /** push_dispatch is a function which is used for basic insetion through tag
     * dispatch.
     * tag match: multiple associative container tag
     */
    template <class AssociativeContainer, class T>
    void push_dispatch(AssociativeContainer& c, const T& v, multiple_associative_container_tag)
    {c.insert(v);}

#endif
