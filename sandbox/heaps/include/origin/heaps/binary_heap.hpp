// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINARY_HEAP_HPP
#define ORIGIN_HEAPS_BINARY_HEAP_HPP

#include <iosfwd>
#include <vector>
#include <unordered_map>

#include <origin/utility/meta.hpp>

namespace origin
{
  /**
   * @internal
   * Impelements a mutable binary heap.

   * @tparam T      The type of data to be stored in the heap; the value type
   * @tparam Comp   A Strict_weak_order over T
   * @tparam Index  A Mapping between values and an index.
   */
  template<typename T, typename Comp, typename Index>
    class mutable_binary_heap_impl
    {
      typedef std::vector<T> container_type;
    private:
      typedef T value_type;
      typedef Comp value_compare;   // FIXME: Should be key compare?
      typedef Index index_mapping;
      typedef typename container_type::size_type size_type;

    private:
      /*
        * heapify: Function to heapify after root is swapped with last element
        * Input:
        * size_type x : Index of the root element of the heap
        * Output:
        * Finds the correct place for element x in the heap
        * Return Value:
        * None
        */
      void heapify (size_type index);

      /*
        * swap_elements: Function to swap two elements in the heap
        * Input:
        * size_type index1: Index of the one element
        * size_type index2: Index of the second element
        * Output:
        * Swaps the two elements
        * Return Value:
        * None
        */
      void swap_elements (size_type index1, size_type index2);

      /*
        * print_recur: Helper function for displaying the binary heap
        * Input:
        * size_type x : Index of the element
        * ostresm &os : Reference to the ouput stream
        * Output:
        * Prints the nodes for a particular binary tree identified by x
        * Return Value: None
        */
      template<typename Char, typename Traits>
      void print_recur(size_type x, std::basic_ostream<Char, Traits>& os)
      {
          size_type sz = elements_.size();

          os << elements_[x];
          size_type i = 2 * x + 1;

          if (i < sz) {
              os << "(";
              print_recur (i, os);

              os << " ";
              i = i + 1;
              if (i < sz) {
                  print_recur (i, os);
              }
              os << ")";
          }
      }


  public:

      /*
        * mutable_binary_heap_impl: Default constructor
        * Input:
        * None
        * Output:
        * Return Value:
        * None
        */
      mutable_binary_heap_impl () = default;

      /*
        * mutable_binary_heap_impl: 2 argument constructor
        * Input:
        * Comp &cmp: comparison function predicate
        * Index: Lambda function for map
        * Output:
        * Instantiates a heap with given comparison function
        * and property map
        * Return Value:
        * None
        */
      mutable_binary_heap_impl (const Comp &cmp, const Index& id) :
                                compare_{cmp}, index_{id}
      {}

      /*
        * mutable_binary_heap_impl: range based constructor
        * Input:
        * ForwardIterator first: Iterator to the first element of a container
        * ForwardIterator last: Iterator to the last element of a container
        * Comp &cmp: comparison function predicate
        * Index: Lambda function for map
        * Output:
        * Instantiates a heap of elements with given comparison function
        * and property map
        * Return Value:
        * None
        */
      template<typename ForwardIterator>
      mutable_binary_heap_impl (ForwardIterator first, ForwardIterator last,
                                const Comp &cmp, const Index& id) :
                                compare_{cmp}, index_{id}
      {
          reserve(std::distance(first, last));

          while(first != last) {
              elements_.push_back(*first);
              index_(*first) = elements_.size() - 1;
              ++first;
          }
          size_type index = elements_.size() / 2;
          while (index > 0){
              index--;
              heapify(index);
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
      void push(const value_type& x);

      /*
        * top: Constant Function to return the const top element of the heap
        * Input:
        * None
        * Output:
        * top element
        * Return Value:
        * value_type &: Reference to the top element is retured
        */
      value_type const& top() const
      {
          return elements_[0];
      }

      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.empty();
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison function.
       */
      value_compare value_comp() const
      {
        return compare_;
      }

      /**
       * Return the index mapping function.
       */
      index_mapping index_map() const
      {
        return index_;
      }


      /**
       * Return the capacity allocated to the heap.
       */
      size_type capacity() const
      {
          return elements_.capacity();
      }

      /**
       * Reserve additional capacity for the heap.
       */
      void reserve(size_type n)
      {
          elements_.reserve(n);
      }


      /*
        * pop: Removes the top element from the heap
        * Input:
        * None
        * Output:
        * binary heap with a new top element
        * Return Value:
        * None
        */
      void pop();

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
      void update(const value_type& d);

      /*
        * print: Function for displaying the binary heap
        * Input:
        * ostresm &os : Reference to the ouput stream
        * Output:
        * Outputs the binary heap to the specified output stream
        * Return Value:
        * None
        * Note: This a helper function developed for unit testing
        */
      template<typename Char, typename Traits>
      void print(std::basic_ostream<Char, Traits>& os);

    private:
      // Return the results of comparing the mth element with the nth element.
      // Note that the comparison is inverted in order to define the correct
      // heap order.
      bool compare_elems(size_type m, size_type n)
      {
        return compare_(elements_[n], elements_[m]);
      }

    private:
      container_type elements_;
      value_compare compare_;
      index_mapping index_;
    };

  template<typename T, typename Comp, typename Index>
    void
    mutable_binary_heap_impl<T, Comp, Index>::swap_elements(size_type m,
                                                            size_type n)
    {
      // Swap two elements in the heap structure
      std::swap(elements_[m], elements_[n]);

      // And update their external map values accordingly
      index_(elements_[m]) = m;
      index_(elements_[n]) = n;
    }

  template<typename T, typename Comp, typename Index>
    void mutable_binary_heap_impl<T, Comp, Index>::push(value_type const& x)
    {
      // Push element into the heap structure
      elements_.push_back(x);
      size_type index = elements_.size() - 1;
      index_(x) = index;

      // Move the element up the heap until condition satisfied
      while(index > 0) {
        size_type parent = (index - 1) / 2;
        if(compare_elems(index, parent)) {
          swap_elements(index, parent);
          index = parent;
        } else {
          break;
        }
      }
    }

  template<typename T, class Comp, class Index>
    void mutable_binary_heap_impl<T, Comp, Index>::heapify(size_type parent)
    {
      size_type total_size = elements_.size();
      size_type new_parent = parent;

      do {
        swap_elements(parent, new_parent);
        parent = new_parent;

        size_type left = 2 * parent + 1;
        size_type right = 2 * parent + 2;

        if((left < total_size) && compare_elems(left, parent))
          new_parent = left;
        if((right < total_size) && compare_elems(right, new_parent))
          new_parent = right;
      } while(parent != new_parent);
    }

  template<typename T, typename Comp, typename Index>
    void mutable_binary_heap_impl<T, Comp, Index>::pop()
    {
      //swap root with last element and delete old root
      swap_elements(0, elements_.size() - 1);
      elements_.pop_back();

      // Preserve the heap invariant.
      if(!empty()) {
        heapify(0);
      }
    }

  template<typename T, typename Comp, typename Index>
    void mutable_binary_heap_impl<T, Comp, Index>::update(value_type const& x)
    {
      //update the element with the new value
      size_type index = index_(x);

      // This is an invariant, not an insertion.
      assert(( elements_[index] == x ));
      // elements_[index] = x;

      // After update, element may need to move up the heap
      while(index > 0){
        size_type parent = (index - 1) / 2;
        if(compare_elems(index, parent)){
          swap_elements(index, parent);
          index = parent;
        } else {
          break;
        }
      }
    }

  template<typename T, typename Comp, typename Index>
    template<typename Char, typename Traits>
      void
      mutable_binary_heap_impl<T, Comp, Index>::
        print(std::basic_ostream<Char, Traits>& os)
        {
          if(elements_.size() > 0)
              print_recur(0, os);
        }

 /**
  * A mutable binary heap is a binary heap that allows the values in the to
  * be modified by an external program. The heap is updated to adjust the
  * location of the element within the data structure and preserve the heap
  * invariant.
  *
  * @tparam T     The value type
  * @tparam Comp  A Strict_weak_order on T
  * @tparam Index A Mapping between T and an index, or default_t.
  */
  template<typename T,
           typename Comp = std::less<T>,
           typename Index = default_t>
    class mutable_binary_heap
    {
    public:
      typedef T value_type;
      typedef Comp value_compare;
      typedef Index index_mapping;
      typedef std::size_t size_type;
    private:
      typedef mutable_binary_heap_impl<value_type, Comp, Index> impl_type;
    public:
      /**
       * @brief Default constructor
       * Initialize an empty heap. The value compare function and index mapping
       * may be optionally given.
       *
       * @param comp
       * @param ix
       */
      mutable_binary_heap(value_compare const& comp = value_compare{},
                          index_mapping const& ix = index_mapping{})
        : impl_(comp, ix)
      { }

      /**
       * @brief Range constructor
       */
      template<typename ForwardIterator>
        mutable_binary_heap(ForwardIterator first,
                            ForwardIterator last,
                            value_compare const& comp = value_compare{},
                            index_mapping const& ix = index_mapping{})
          : impl_(first, last, comp, ix)
        { }

      /**
       * @brief Initializer list constructor
       */
      mutable_binary_heap(std::initializer_list<T> list,
                          value_compare const& comp = value_compare{},
                          index_mapping const& ix = index_mapping{})
        : impl_(list.begin(), list.end(), comp, ix)
      { }

      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return impl_.empty();
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return impl_.size();
      }

      value_compare value_comp() const
      {
        return impl_.value_comp();
      }
      //@}

      /**
       * Return the number of objects allocated to the heap.
       */
      size_type capacity() const
      {
          return impl_.capacity();
      }

      /**
       * Reserve additional capacity for the heap.
       */
      void reserve(size_type n)
      {
          impl_.reserve(n);
      }

      /** @name Heap operations */
      //@{
      /**
       * Return the object at the top of the heap.
       */
      value_type const& top() const
      {
          return impl_.top();
      }

      /**
       * Insert the given object into the heap. This operation completes in
       * O(lg n) time.
       */
      void push(value_type const& x)
      {
          impl_.push(x);
      }

      /**
       * Update the heap after a change to the value of the given object. This
       * operation completes in O(lg n) time.
       */
      void update(value_type const& x)
      {
          impl_.update(x);
      }

      /**
       * Remove the top object from the heap. This operation completes in
       * O(lg n) time.
       */
      void pop()
      {
          impl_.pop();
      }
      //@}

      template<typename Char, typename Traits>
      void print(std::basic_ostream<Char, Traits>& os)
      {
          impl_.print(os);
      }

    protected:
      impl_type impl_;
    };

  // The default specialization of mutable binary heaps defines its own
  // index map (a hash table).
  template<typename T, typename Comp>
    class mutable_binary_heap<T, Comp, default_t>
    {
    public:
      typedef T value_type;
      typedef Comp value_compare;
      typedef std::size_t size_type;
    private:
      // Indices are stored in an unordered map.
      // FIXME: What if the value type can't be hashed?
      typedef std::unordered_map<value_type, size_type> index_map_type;

      // The index_map_func implements abstracts the access to the stored
      // hash table.
      struct index_map_func
      {
        index_map_func(index_map_type& m)
          : map(m)
        { }

        template<typename value_type>
        size_type& operator()(value_type const& key)
        {
            return map[key];
        }

        index_map_type& map;
      };

      typedef mutable_binary_heap_impl<T, Comp, index_map_func> impl_type;
    public:
      /**
       * @brief Default constructor
       */
      mutable_binary_heap (value_compare const& comp = value_compare{})
        : map_{}, impl_(comp, index_map_func{map_})
      { }

      /**
       * @brief Range constructor
       */
      template<typename ForwardIterator>
        mutable_binary_heap(ForwardIterator first,
                            ForwardIterator last,
                            value_compare const& comp = value_compare{})
          : map_{}, impl_{first, last, comp, index_map_func{map_}}
        { }

      /**
       * @brief Initializer list constructor
       */
      mutable_binary_heap(std::initializer_list<T> list,
                          value_compare const& comp = value_compare{})
        : map_{}, impl_{list, comp, index_map_func{map_}}
      { }


      bool empty() const
      {
          return impl_.empty();
      }

      size_type size() const
      {
          return impl_.size();
      }

      void reserve(size_type n)
      {
          impl_.reserve(n);
      }

      size_type capacity() const
      {
          return impl_.capacity();
      }

      const value_type& top() const
      {
          return impl_.top();
      }

      void push(const value_type& d)
      {
          impl_.push(d);
      }

      void update(const value_type& d)
      {
          impl_.update(d);
      }

      void pop()
      {
        // FIXME: This leaves entries in the index map after popping. It
        // shouldn't cause any problems (keys are reassigned new indexes when
        // pushed), but it does waste some space. Fixing this would require
        // reimplementing impl_type::pop so that the associated key was erased
        // before the object is popped from the vector.
        impl_.pop();
      }

      template<typename Char, typename Traits>
      void print(std::basic_ostream<Char, Traits>& os)
      {
          impl_.print(os);
      }


    protected:
        index_map_type map_;
        impl_type impl_;
    };


  // FIXME: Find a nice data structure reference for the documentation (CLR?).
  // FIXME: This is basically std::priority_queue. Why not reuse that
  // implementation?
  /**
   * The binary heap data structure implements a classic, generic binary heap.
   * The order (min or max) of the heap is determined by a compare operation on
   * the value type. This defaults to std::less, resulting in max-heap.
   *
   * @tparam T      Type of data to be stored in the heap
   * @tparam Comp   A Strict Weak Ordering over T
   */
  template<typename T, typename Comp = std::less<T>>
    class binary_heap
    {
      typedef std::vector<T> container_type;
    public:
      typedef T value_type;
      typedef Comp value_compare;
      typedef typename container_type::size_type size_type;

      /**
       * @brief Default constructor
       * Initialize an empty heap. A comparison function may optionally be given.
       *
       * @param comp  A value comparison Operation
       */
      binary_heap(value_compare const& comp = value_compare{})
        : elements_{}, compare_{comp}
      { }

      // FIXME: This needs to be specialized for input and forwrad iterators.
      /**
       * @brief Range constructor
       * Initialize the heap over the sequence of elements in the range
       * [first, last). A comparison function may be optional given.
       *
       * @tparam Iter A Forward_Iterator having value type T
       *
       * @param first   The first iterator in a range of objects
       * @param last    An iterator past the end of a range of objects
       * @param comp    A Comp function object.
       */
      template<typename ForwardIterator>
        binary_heap(ForwardIterator first,
                    ForwardIterator last,
                    value_compare const& comp = value_compare{})
          : elements_{first, last}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /**
       * @brief Initializer list constructor
       * Initialize the heap from a seuqence of objects. A comparison function
       * may be optionally given.
       *
       * @param list  An initializer list of objects of type T
       * @param comp  A Comp function object.
       */
      binary_heap(std::initializer_list<T> list, value_compare const& comp)
        : elements_{list.begin(), list.end()}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.size() == 0;
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison operation.
       */
      value_compare value_comp() const
      {
        return compare_;
      }
      //@}

      /** @name Capacity */
      //@{
      /**
       * Return the current capacity of the heap. This is the number of objects
       * for which memory has been allocated, but not initialized.
       */
      size_type capacity() const
      {
        return elements_.capacity();
      }

      // FIXME: What if n < size()? Do you truncate the heap? Is the heap still
      // valid after truncation? Should you never give up capacity once required?
      /**
       * Reserve additional memory for the heap.
       */
      void reserve(size_type n)
      {
        elements_.reserve(n);
      }
      //@}


      /** @name Operations */
      //@{
      /**
       * Return the object on the top of the heap.
       */
      const value_type& top() const
      {
          return elements_[0];
      }

      /**
       * Push an object onto the heap. This operation completes in O(lg n) time.
       */
      void push(value_type const& x)
      {
        elements_.push_back(x);
        std::push_heap(elements_.begin(), elements_.end(), compare_);
      }

      /**
       * Pop the top object off the heap. This operation completes in O(lg n)
       * time.
       */
      void pop()
      {
        std::pop_heap(elements_.begin(), elements_.end(), compare_);
        elements_.pop_back();
      }
      //@}

      // FIXME: Go away?
      /**
       * Write the contents of the heap to the given output stream.
       */
      template<typename Char, typename Traits>
        void print(std::basic_ostream<Char, Traits>& os);

    private:

      // FIXME: Go away?
      // Helper function for displaying the binary heap
      template<typename Char, typename Traits>
        void print_recur(size_type n, std::basic_ostream<Char, Traits>& os);

    private:
      // FIXME: Use EBO to remove the overhead of the value compare function.
      container_type elements_;
      value_compare compare_;
    };


  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print(std::basic_ostream<Char, Traits>& os)
      {
        if(!empty()) {
          print_recur(0, os);
        }
      }

  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print_recur(size_type n,
                                             std::basic_ostream<Char, Traits>& os)
      {
        os << elements_[n];
        size_type i = 2 * n + 1;

        size_type sz = elements_.size();
        if(i < sz) {
          os << "(";
          print_recur(i, os);

          os << " ";
          i = i + 1;
          if(i < sz) {
            print_recur(i, os);
          }
          os << ")";
        }
      }

} // namespace origin


#endif // ORIGIN_HEAPS_BINARY_HEAP_HPP
