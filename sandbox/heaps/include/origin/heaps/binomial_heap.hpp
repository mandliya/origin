// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINOMIAL_HEAP_HPP
#define ORIGIN_HEAPS_BINOMIAL_HEAP_HPP

// FIXME: No Boost includes.

#include <boost/config/no_tr1/cmath.hpp>
#include <iosfwd>
#include <vector>
#include <boost/config.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>

/*
 * An adaptation of CLRS's Binomial heap implementation
 * in Chapter 19, 3rd Ed, pages .
 */

/* Class: Binomial Heap
 * Template parameters
 * T : Value type - Type of data to be stored in the heap
 * Compare : Binary function predicate - This gives the ordering between the
 *           element (max/min heap)
 * ItemToIndexMap : Property Map - External property map used to find the
 *                  index of element in the heap
 * RandomAccessContainer: Underlying container - The binomial heap adaptor
 *                        is built on top of this container. This stores the
 *                        elements of the heap.
 */
// FIXME: Remove the random access container parameter
// FIXME: Parameterize over a map function/internal map designator.
// FIXME: Write long functions outside the body of the class.
template <class T,
          class Compare = std::less<T>,
          class ItemToIndexMap = boost::identity_property_map,
          class RandomAccessContainer = std::vector<T> >
class binomial_heap
{
private:
    typedef typename boost::property_traits<ItemToIndexMap>::value_type size_type;

    // FIXME: Value type should be "T const" for mutable priority queues.
    typedef T value_type;
    typedef std::vector<size_type> IndexArray;

    // FIXME: I think there may be some typedefs missing. See priority_queue.

    // FIXME: Move outside this class and rename binomial_heap_node. Consider
    // writing as two classes: binomial_heap_node_base (non-template) and
    // binomial_heap_node (that contains the value type).
	/*
	 * Structure which holds the pointers representing each element in the heap.
	 */
    struct data {
        #define NIL -1
		/* Index of the element in the heap */
        size_type item_index;

		/* Index of parent of the element in the heap */
        size_type parent;

		/* Index of the child element in the heap */
        size_type child;

		/* Index of the right neighbour of the element in the heap */
        size_type right_sibling;

        /* Degree of the element */
        size_type degree;
        data ():parent(NIL), child(NIL), right_sibling(NIL), degree(NIL)
        {};
    } ;

    // FIXME: Combine as a single vector of binomial_heap_node<T>
    /* Random access container which holds the heap elements */
    std::vector<data<T>> _elements;
    std::vector<struct data> _data;

    /* _n  - number of elements in the heap
     * _head - index of the root element
     * _top - index of the top element
     * max_size - maximum number of elements in the heap
     */
    // FIXME: Lose _n and max_size.
    // FIXME: Prefer a trailing underscore for member variable names.
    size_type _n, _head, _top, max_size;

    // FIXME: Consider using the Empty Base Optimization to store the
    // Compare function (I'll implement compressed pair at some point to
    // help with this).
	/* Internal map for mapping the values stored in the external property map
	 * to actual index of the element in the heap
	 */
    IndexArray index_array;
    ItemToIndexMap _id;
    Compare _compare;

    // FIXME: Remove me
    size_type nil() const { return -1;}

    /*
     * print_recur: Helper function for displaying the binomial heap
     * Input:
     * size_type x : Index of the element
     * ostresm &os : Reference to the ouput stream
     * Output:
     * Prints the nodes for a particular binomial tree identified by x
     * Return Value:
     * None
     */
    void print_recur(size_type x, std::ostream& os)
    {
        if (x != nil()) {
            os << _elements[_data[x].item_index];
			if (_data[x].degree > 0) {
			    os << "(";
                size_type i = _data[x].child;
                do {
                    print_recur (i, os);
                    os << " ";
                    i = _data[i].right_sibling;
                } while (i != nil());
                os << ")";
            }
        }
    }

    /*
     * binomial_heap_merge: Function to merge two binomial heaps
     * Input:
     * size_type x : Index of the root element of the heap to be merged
     * Output:
     * Merges the heap pointed to by x in the main heap pointed to by _head
     * Return Value:
     * None
     */
    void merge (size_type index)
	{
        size_type p = _head;
        size_type q = index;

        size_type new_head;
        if (std::min<size_type>(_data[p].degree, _data[q].degree)) {
            new_head = p;
		} else {
            new_head = q;
		}

        size_type current = _data.size();
        size_type initial_size = -1;
        _data.push_back(data());

        while (p != initial_size || q != initial_size) {
            if (q == initial_size ||
			     (p != initial_size && _data[p].degree < _data[q].degree)) {
                _data[current].right_sibling = p;
                current = p;
                p = _data[p].right_sibling;
            } else {
                _data[current].right_sibling = q ;
                current = q;
                q = _data[q].right_sibling;
            }
        }

        _head = _data.back().right_sibling;
        _data.pop_back();
    }

    // FIXME: Should be private?
    /*
     * get_new_top: Function to find the index of new root element
     * Input:
     * None
     * Output:
     * Index of the new root
     * Return Value:
     * size_type: Index of the new root
     */
    size_type get_new_top()
	{
        size_type top_index;
		size_type tmp;
        if (_head == nil()) {
            return nil();
        }

        top_index = _head;
		tmp = _data[top_index].right_sibling;

		while (tmp != nil()) {
            if (_compare(_elements[_data[tmp].item_index], _elements[_data[top_index].item_index])) {
                top_index = tmp;
		    }

            tmp = _data[tmp].right_sibling;
        }

        return top_index;
    }

    // FIXME: This operation may be written in such a way that it does not
    // depend on template parameters. Consider moving it outside the body
    // of this class.
     /*
     * binomial_link: Links the specified nodes in the heap
     * Input:
	 * size_type y: element in the heap
	 * size_type z: element in the heap
     * Output:
     * links element y and z in the heap
     * Return Value:
	 * None
     */
    void binomial_link (size_type y, size_type z)
    {
        _data[y].parent = z;
        _data[y].right_sibling = _data[z].child;
        _data[z].child = y;
        _data[z].degree = _data[z].degree + 1;
    }



public:
    // FIXME Implement two more constructors: a range constructor (taking two
    // iterators) and an initializer list constructor.


    /*
     * binomial_heap: Default constructor
     * Input:
     * None
     * Output:
	 * Instantiates an empty heap with Compare() as comparison function and
	 * ItemToIndexMap() as the map
     * Return Value:
     * None
     */
    binomial_heap (): _compare(Compare()), _id(ItemToIndexMap())
    {}

   /*
     * binomial_heap: 3 argument constructor
     * Input:
     * size_type n: number of elements in the heap initially
	 * Compare &cmp: comparison function predicate
	 * ItemToIndexMap: Property map
     * Output:
	 * Instantiates a heap of n elements with given comparison function
	 * and property map
     * Return Value:
     * None
     */
    binomial_heap (size_type n,
                   const Compare &cmp = std::less<T>(),
                   const ItemToIndexMap& id = boost::identity_property_map()):
                   _n(0), _compare(cmp), _id(id)
      : max_size(n), _top(-1),
    {
        max_size = n;
        index_array.resize(n);
        _top = nil();
        _head = nil();
        _elements.reserve(n);
        _data.reserve(n);
    }

    /*
     * print: Function for displaying the binomial heap
     * Input:
     * ostresm &os : Reference to the ouput stream
     * Output:
     * Outputs the binomial heap to the specified output stream
     * Return Value:
     * None
	 * Note: This a helper function developed for unit testing
     */

    template<typename Char, typename Traits>
    void print(std::basic_ostream<Char, Traits>& os)
	{
        if (_head != nil()) {
            size_type i = _head;
            do {
                print_recur(i, os);
                os << std::endl;
                i = _data[i].right_sibling;
            } while (i != nil());
        }
    }

    // FIXME: Remove this function.
    /*
     * remove: Remove an element from the heap
     * Input:
	 * value_type &d: Reference to element which has to be deleted
     * Output:
	 * Removes the given element from the heap.
     * Return Value:
     * None
     */
    void remove(const value_type& d)
    {
        /*
		 * This function works by decreasing key and then popping the top
		 * element. Since the value type is not known in advance, the element
		 * to be deleted is assumed to be the smallest element in the heap
		 * and made the new top element. After that pop operation is done to
		 * remove the top element, and thus delete the element referenced by d
		 */
        size_type index = index_array[get(_id, d)];
        size_type parent = _data[index].parent;
        size_type temp;
        _elements[_data[index].item_index] = d;

        while(parent != nil()) {
            _elements[_data[index].item_index] =
		                     _elements[_data[parent].item_index];
            _elements[_data[parent].item_index] = d;
            temp = get(_id, _elements[_data[index].item_index]);
            index_array[temp] = index;


            index = parent;
            parent = _data[parent].parent;
        }

        temp = get(_id,d);

        index_array[temp] = index;

        _top = index;
        pop();

    }

    /*
     * Update: Updates the given element in the heap
     * Input:
	 * value_type &d: Reference to element which has to be updated
     * Output:
	 * Updated heap
     * Return Value:
     * None
	 * Precondition: Element d must already be updated in the map
     */
    void update(const value_type& d)
	{
        size_type index = index_array[get(_id, d)];
        size_type parent = _data[index].parent;
        size_type temp;
        _elements[_data[index].item_index] = d;

        while (parent != nil() &&
		           _compare(d, _elements[_data[parent].item_index])) {
            _elements[_data[index].item_index] =
			                  _elements[_data[parent].item_index];
            _elements[_data[parent].item_index] = d;
            temp = get(_id, _elements[_data[index].item_index]);
            index_array[temp] = index;

            index = parent;
            parent = _data[parent].parent;
        }

        temp = get(_id,d);

        index_array[temp] = index;

        if (_compare(d, _elements[_data[_top].item_index])) {
            _top = index;
        }
    }

    /*
     * push: Insets the given element in the heap
     * Input:
	 * value_type &d: Reference to element which has to be inserted
     * Output:
	 * Heap with the new element inserted
     * Return Value:
     * None
	 * Precondition: Element d must already be present in the map
     */
    void push(const value_type& d)
	{
        data obj;
        size_type index;
        _n ++;

		/*
		 * If the heap has already been filed up to the intialized size,
		 * then double the size of the heap, and reset the max size to the
		 * doubled size
		 */
        if (_n > max_size) {
            max_size = max_size * 2;
            index_array.resize(max_size);
            _data.reserve(max_size);
            _elements.reserve(max_size);
        }

        _elements.push_back(d);
        obj.item_index = _elements.size() - 1;
        obj.parent = nil();
        obj.right_sibling = nil();
        obj.child = nil();
        obj.degree = 0;

        _data.push_back(obj);

        index = _elements.size() - 1;

        index_array[get(_id, d)] = index;
        if (_head == nil()) {
		/* New heap */
            _head = index;
            _top = _head;
        } else {
		/* Unite the 1 element heap with the existing heap */
           binomial_heap_union(index);
              if (_compare(_elements[_data[index].item_index], _elements[_data[_top].item_index]))
                  _top = index;
        }

    }


    /*
     * binomial_heap_union: Unites the heap pointed to by _head with the heap
	 *                      specified
     * Input:
	 * size_type index: Index of the root element of the heap to be united
     * Output:
	 * United binomial heap
     * Return Value:
     * None
     */
    void binomial_heap_union (size_type index)
    {
        /* Merge the root lists */
	    binomial_heap_merge(index);
        if (_data[_head].right_sibling == nil()) {
            return;
        }

        size_type x = _head;
        size_type next_x = _data[_head].right_sibling;
        size_type prev_x = nil();

        while (next_x != nil()) {
            if ((_data[x].degree != _data[next_x].degree)
                    || (_data[next_x].right_sibling != nil()
                    && _data[_data[next_x].right_sibling].degree
                                             == _data[x].degree)) {

                 prev_x = x;
                 x = next_x;
            } else {
                if (_compare (_elements[_data[x].item_index], _elements[_data[next_x].item_index])) {
                    _data[x].right_sibling = _data[next_x].right_sibling;
                    binomial_link(next_x, x);
                } else {
                    if (prev_x == nil()) {
                        _head = next_x;
                    } else {
                        _data[prev_x].right_sibling = next_x;
                    }

                    binomial_link(x, next_x);
                    x = next_x;
                }
            }


            next_x = _data[x].right_sibling;

        }
    }

    // FIXME: Add a const version of this function that returns a constant
    // reference.
    /*
     * top: Function to return the top element of the heap
     * Input:
     * None
     * Output:
     * top element
     * Return Value:
     * value_type &: Reference to the top element is retured
     */
	value_type& top()
	{
        return _elements[ _data[_top].item_index];
    }

    /*
     * empty: Function to check for empty heap
     * Input:
     * None
     * Output:
     * State of the heap (empty/notempty)
     * Return Value:
     * bool : True if heap is empty, False otherwise
     */
    bool empty() const
	{
        return _n==0;
    }

    /*
     * size: Function to find the size of the heap
     * Input:
     * None
     * Output:
     * Number of elements in the heap
     * Return Value:
     * size_type : Number of elements
     */
    size_type size() const
	{
        return _n;
    }

    /*
     * pop: Removes the top element from the heap
     * Input:
	 * None
     * Output:
	 * binomial heap with a new top element
     * Return Value:
     * None
     */
    void pop()
	{
        size_type new_head = nil();

        if (_head == nil()) {
            return;
        }

        /* Reverse the list */
        if (_data[_top].child != nil()){
            size_type tmp_head = _data[_top].child;
            size_type tmp_sibling;

            while (tmp_head != nil()) {
                tmp_sibling = _data[tmp_head].right_sibling;
                _data[tmp_head].parent = nil();
                _data[tmp_head].right_sibling = new_head;
                new_head = tmp_head;
                tmp_head = tmp_sibling;
            }
        }

        if (_data[_head].right_sibling == nil()) {
            _head = new_head;
        } else {
            if (_head == _top) {
                _head = _data[_head].right_sibling;
            } else {
                size_type tmp = _head;
                while (_data[tmp].right_sibling != _top) {
                tmp = _data[tmp].right_sibling;
                }

            _data[tmp].right_sibling = _data[_top].right_sibling;
            }

            binomial_heap_union(new_head);
        }

        _top = get_new_top();
        --_n;
    }

};

#endif // BINOMIAL_HEAP_HPP
