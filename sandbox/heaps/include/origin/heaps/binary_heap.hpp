// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINARY_HEAP_HPP
#define ORIGIN_HEAPS_BINARY_HEAP_HPP

#include <vector>
#include <iostream>
#include <unordered_map>

#include <origin/utility/meta.hpp>

namespace origin
{
    /* Class: Binary Heap 
     * Template parameters
     * T : Value type - Type of data to be stored in the heap
     * Compare : Binary function predicate - This gives the ordering between the
     *           element (max/min heap)
     * Item_Map : Property Map - External property map used to find the 
     *                  index of element in the heap
     */
    template <class T, 
              class Compare,
              class Item_Map>
    class mutable_binary_heap_impl
    {
        private:
            typedef T value_type;
            typedef size_t size_type;

            //Random access container which holds the heap elements
            std::vector<T> elements_;

            Compare compare_;
            Item_Map id_;

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


        public:

            /*
             * mutable_binary_heap_impl: Default constructor
             * Input: 
             * None
             * Output:
             * Return Value:
             * None       
             */
            mutable_binary_heap_impl () {};

            /*
             * mutable_binary_heap_impl: 3 argument constructor
             * Input: 
             * size_type n: number of elements in the heap initially
             * Compare &cmp: comparison function predicate
             * Item_Map: Lambda function for map
             * Output:
             * Instantiates a heap of n elements with given comparison function
             * and property map
             * Return Value:
             * None
             */
            mutable_binary_heap_impl (size_type n,
                  const Compare &cmp, const Item_Map& id) :
                  compare_{cmp}, id_{id}
            { 
            }

            /*
             * mutable_binary_heap_impl: range based constructor
             * Input: 
             * ForwardIterator first: Iterator to the first element of a container 
             * ForwardIterator last: Iterator to the last element of a container
             * Compare &cmp: comparison function predicate
             * Item_Map: Lambda function for map
             * Output:
             * Instantiates a heap of n elements with given comparison function
             * and property map
             * Return Value:
             * None       
             */
            template<typename ForwardIterator>
            mutable_binary_heap_impl (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp, const Item_Map& id) :
                   compare_{cmp}, id_{id}
            {
                while(first != last) {
                    elements_.push_back(*first);
                    id_(*first) = elements_.size() - 1;
                    ++first;
                }
                size_type index = elements_.size() / 2;
                while (index > 0){
                    index--;
                    heapify(index); 
                }
            }
  
            mutable_binary_heap_impl (std::initializer_list<T> lst, 
                   const Compare &cmp, const Item_Map& id):
                   compare_{cmp}, id_{id}
            {
                for (auto &x : lst) {
                    elements_.push_back(x);
                    id_(x) = elements_.size() - 1;
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
            void push(const value_type& d);

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
                return elements_[0];
            }

            /*
             * top: Constant Function to return the const top element of the heap
             * Input: 
             * None
             * Output:
             * top element
             * Return Value:
             * value_type &: Reference to the top element is retured
             */
            const value_type& top() const
            {
                return elements_[0];
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
                return elements_.size()==0;
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
                return elements_.size();
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

    };

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_binary_heap_impl<T, Compare, Item_Map>::swap_elements (size_type index1, size_type index2)
    {
        //swap two elements in the heap structure
        value_type temp_element = elements_[index1];
        elements_[index1] = elements_[index2];
        elements_[index2] = temp_element;

        //change external map values accordingly
        id_(elements_[index1]) = index1;
        id_(elements_[index2]) = index2;
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_binary_heap_impl<T, Compare, Item_Map>::push(const value_type& d)
    {
        //Push element into the heap structure
        elements_.push_back(d);

        size_type parent_index;
        size_type index;
        index = elements_.size() - 1;

        id_(d) = index;

        //move the element up the heap till condition satisfied
        while (index > 0){
            parent_index = (index - 1) / 2;
            if (compare_(elements_[index], elements_[parent_index])){

                //swap child and parent in the heap structure
                swap_elements(index, parent_index);

                //new index will be the parents index
                index = parent_index;
            }
            else
                break;
        }

    }

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_binary_heap_impl<T, Compare, Item_Map>::heapify (size_type parent)
    {
        size_type total_size = elements_.size();

        size_type new_parent = parent;
        size_type left_child;
        size_type right_child;

        do{
            swap_elements(parent, new_parent);
            parent = new_parent;

            left_child = 2 * parent + 1;
            right_child = 2 * parent + 2;
        
            if ((left_child < total_size) && (compare_(elements_[left_child], elements_[parent]))) {
                new_parent = left_child;
            }
            if ((right_child < total_size) && (compare_(elements_[right_child],elements_[new_parent]))) {
                new_parent = right_child;
            }
        }while (parent != new_parent);
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_binary_heap_impl<T, Compare, Item_Map>::pop()
    {
        //swap root with last element and delete old root
        swap_elements(0, elements_.size()-1);

        elements_.pop_back();

        //Find correct position for new root
        heapify(0);

    }

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_binary_heap_impl<T, Compare, Item_Map>::update(const value_type& d)
    {
        //update the element with the new value
        size_type index = id_(d);
        elements_[index] = d;

        //if root was modified, no action required
        if (index == 0)
            return;

        size_type parent_index = (index - 1) / 2;

        //after update, element may need to move up the heap
        if (compare_(elements_[index], elements_[parent_index])) {
            while (index > 0){
                parent_index = (index - 1) / 2;
                if (compare_(elements_[index], elements_[parent_index])){

                    //swap child and parent in the heap structure
                    swap_elements(index, parent_index);

                    //new index will be the parents index
                    index = parent_index;
                }
                else
                    break;
            }
        }
        //else, element may need to move down the heap
        else {
            heapify(index);
        }
    }

    /*
     * Binary Heap implementation used when an external map is provided
     * by the user
     */
    template <class T, 
              class Compare,
              class Item_Map = default_t> 
    class mutable_binary_heap
    {
        public:
            typedef T value_type;
            typedef size_t size_type;

        protected:
            typedef mutable_binary_heap_impl<value_type, Compare, Item_Map> search_impl;

        public:
            mutable_binary_heap(): impl() 
            {}

            mutable_binary_heap (size_type n,
                const Compare& cmp, const Item_Map& id) : impl(n, cmp, id)
            {}

            template<typename ForwardIterator>
            mutable_binary_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp, const Item_Map& id):
                           impl (first, last, cmp, id)
            {}

            mutable_binary_heap (std::initializer_list<T> lst, 
                           const Compare &cmp, const Item_Map& id):
                           impl (lst, cmp, id)
            {}
 
            void update(const value_type& d) 
            {
                impl.update(d); 
            }

            void push(const value_type& d)
            { 
                impl.push(d);
            }

            value_type& top()
            {
               return impl.top(); 
            }

            const value_type& top() const
            {
                return impl.top();
            }

            bool empty() const
            {
               return impl.empty();
            }

            size_type size() const
            {
               return impl.size();
            }

            void pop()
            { 
                impl.pop();
            }

        protected:
            search_impl impl;
    };

    /*
     * Binaryl Heap Implementation in case of no external map
     */
    template <class T, 
              class Compare>
    class mutable_binary_heap<T, Compare, default_t>
    {

        public:
            typedef T value_type;
            typedef size_t size_type;

        protected:
            typedef typename std::unordered_map<value_type, size_type> internal_map;
            struct item_map {
                internal_map *imap;

                public:
                item_map (internal_map *map): imap(map) 
                {}

                template<typename value_type>
                size_type& operator() (const value_type& key)
                {
                    return (*imap)[key];
                }
            };


            typedef mutable_binary_heap_impl<value_type, Compare, item_map> search_impl;

        public:
            mutable_binary_heap() 
            {};

            mutable_binary_heap (size_type n,
                const Compare& cmp): map_{n}, id_{&map_}, impl (n, cmp, id_)
            {}

            template<typename ForwardIterator>
            mutable_binary_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp): id_{&map_},
                           impl (first, last, cmp, id_)
            {}

            mutable_binary_heap (std::initializer_list<T> lst, 
                           const Compare &cmp): id_{&map_},
                           impl (lst, cmp, id_)
            {}

            /*
             * The assumption is that heap never stores the actual value. It 
             * contains reference to the value, and the reference remains 
             * unchanged during update
             */
            void update(const value_type& d)
            {
                impl.update(d); 
            }

            void push(const value_type& d)
            {
                impl.push(d); 
            }

            value_type& top()
            {
               return impl.top(); 
            }

            const value_type& top() const
            {
                return impl.top();
            }

            bool empty() const
            {
               return impl.empty(); 
            }

            size_type size() const
            {
               return impl.size();
            }

            void pop()
            {
                const value_type top_element = impl.top();
                impl.pop();
                map_.erase(top_element);
            }

        protected:
            internal_map map_;
            item_map id_;
            search_impl impl;

    };


    /* Class: Binary Heap - non mutable 
     * Template parameters
     * T : Value type - Type of data to be stored in the heap
     * Compare : Binary function predicate - This gives the ordering between the
     *           element (max/min heap)
     */
    template <class T, 
              class Compare>
    class binary_heap
    {
        private:
            typedef T const value_type; 
            typedef size_t size_type;

            //Random access container which holds the heap elements
            std::vector<T> elements_;

            Compare compare_;

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


        public:

            /*
             * binary_heap: Default constructor
             * Input: 
             * None
             * Output:
             * Return Value:
             * None       
             */
            binary_heap () {};

            /*
             * binary_heap: 2 argument constructor
             * Input: 
             * size_type n: number of elements in the heap initially
             * Compare &cmp: comparison function predicate
             * Output:
             * Instantiates a heap of n elements with given comparison function
             * Return Value:
             * None
             */
            binary_heap (size_type n,
                  const Compare &cmp) :
                  compare_{cmp}
            {}

            /*
             * binary_heap: range based constructor
             * Input: 
             * ForwardIterator first: Iterator to the first element of a container 
             * ForwardIterator last: Iterator to the last element of a container
             * Compare &cmp: comparison function predicate
             * Output:
             * Instantiates a heap of n elements with given comparison function
             * Return Value:
             * None       
             */
            template<typename ForwardIterator>
            binary_heap (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp) :
                   compare_{cmp}
            {
                while(first != last) {
                    elements_.push_back(*first);
                    id_(*first) = elements_.size() - 1;
                    ++first;
                }
                size_type index = elements_.size() / 2;
                while (index > 0){
                    index--;
                    heapify(index); 
                }
            }

            binary_heap (std::initializer_list<T> lst, 
                   const Compare &cmp):
                   compare_{cmp}
            {
                for (auto &x : lst) {
                    elements_.push_back(x);
                    id_(x) = elements_.size() - 1;
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
            void push(const value_type& d); 

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
                return elements_[0];
            }

            /*
             * top: Constant Function to return the const top element of the heap
             * Input: 
             * None
             * Output:
             * top element
             * Return Value:
             * value_type &: Reference to the top element is retured
             */
            const value_type& top() const
            {
                return elements_[0];
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
                return elements_.size()==0;
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
                return elements_.size();
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

    };

    template <class T, 
              class Compare>
    void binary_heap<T, Compare>::swap_elements (size_type index1, size_type index2)
    {
        //swap two elements in the heap structure
        value_type temp_element = elements_[index1];
        elements_[index1] = elements_[index2];
        elements_[index2] = temp_element;
    }

    template <class T, 
              class Compare>
    void binary_heap<T, Compare>::push(const value_type& d)
    {
        //Push element into the heap structure
        elements_.push_back(d);

        size_type parent_index;
        size_type index;
        index = elements_.size() - 1;

        //move the element up the heap till condition satisfied
        while (index > 0){
            parent_index = (index - 1) / 2;
            if (compare_(elements_[index], elements_[parent_index])){

                //swap child and parent in the heap structure
                swap_elements(index, parent_index);

                //new index will be the parents index
                index = parent_index;
            }
            else
                break;
        }

    }

    template <class T, 
              class Compare>
    void binary_heap<T, Compare>::heapify (size_type parent)
    {
        size_type total_size = elements_.size();
        
        size_type new_parent = parent;
        size_type left_child;
        size_type right_child;

        do{
            swap_elements(parent, new_parent);
            parent = new_parent;

            left_child = 2 * parent + 1;
            right_child = 2 * parent + 2;
        
            if ((left_child < total_size) && (compare_(elements_[left_child], elements_[parent]))) {
                new_parent = left_child;
            }
            if ((right_child < total_size) && (compare_(elements_[right_child],elements_[new_parent]))) {
                new_parent = right_child;
            }
        }while (parent != new_parent);
    }

    template <class T, 
              class Compare>
    void binary_heap<T, Compare>::pop()
    {
        //swap root with last element and delete old root
        swap_elements(0, elements_.size()-1);

        elements_.pop_back();

        //Find correct position for new root
        heapify(0);

    }

}


#endif // ORIGIN_HEAPS_BINARY_HEAP_HPP
