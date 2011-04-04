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
    class binary_heap_impl
    {
        private:
            // FIXME: Value type should be "T const" for mutable priority queues.
            typedef T value_type; //TODO: Will address then when we write Immutable heap 
            typedef size_t size_type;
            typedef std::vector<size_type> IndexArray;
            
            /* Random access container which holds the heap elements */
            std::vector<T> elements_;
  	  
            /* Internal map for mapping the values stored in the external map
             * to actual index of the element in the heap
             */
            
            Compare compare_;
            Item_Map id_;
            IndexArray index_array;
  		  
            /* Mapping Elements to internal map - i.e. reverse mapping of index_array */
            IndexArray rev_index_array;

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
             * binary_heap_impl: Default constructor
             * Input: 
             * None
             * Output:
             * Return Value:
             * None       
             */
            binary_heap_impl () {};
            
            /*
             * binary_heap_impl: 3 argument constructor
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
            binary_heap_impl (size_type n,
                  const Compare &cmp, const Item_Map& id) :
                  compare_{cmp}, id_{id}, index_array(n)
            { 
            }
            
            /*
             * binary_heap_impl: range based constructor
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
            binary_heap_impl (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp, const Item_Map& id) :
                   index_array(std::distance(first, last)), compare_{cmp}, 
                   id_{id}
            {
                while(first != last) {
                    push(*first);
                    ++first;
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
                if(!empty())
                    return elements_[0];
                else
                    return NULL;
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
                if(!empty())
                    return elements_[0];
                else
                    return NULL;
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
    void binary_heap_impl<T, Compare, Item_Map>::swap_elements (size_type index1, size_type index2)
    {
        if ((index1 < elements_.size()) || (index2 < elements_.size()))
            return;
 
        value_type temp_element;
        size_type temp_index;
 	   
        /* swap two elements in the heap structure */
        temp_element = elements_[index1];
        elements_[index1] = elements_[index2];
        elements_[index2] = temp_element;
 		
        /* change internal map accordingly */
        index_array[rev_index_array[index1]] = index2;
        index_array[rev_index_array[index2]] = index1;
 
        /* swap two elements in the reverse internal map */
        temp_index = rev_index_array[index1];
        rev_index_array[index1] = rev_index_array[index2];
        rev_index_array[index2] = temp_index;
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void binary_heap_impl<T, Compare, Item_Map>::push(const value_type& d)
    {
        /* Push element into the heap structure */
        elements_.push_back(d);
 
        size_type parent_index;
 
        /* store the position in the internal map */
        size_type index;
        index = elements_.size() - 1;
 
        size_type rev_index;
        rev_index = id_(d);
        index_array[rev_index] = index;
 
        /* store the index of the internal map in the reverse map */
        rev_index_array.push_back(rev_index);
 	   
        /* move the element up the heap till condition satisfied */
        while (index > 0){
            parent_index = (index - 1) / 2;
            if (compare_(elements_[index], elements_[parent_index])){

                /* swap child and parent in the heap structure */
                swap_elements(index, parent_index);
   
                /* new index will be the parents index */
                index = parent_index;
            }
            else
                break;
        }
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void binary_heap_impl<T, Compare, Item_Map>::heapify (size_type parent)
    {
        size_type total_size = elements_.size();
        if (parent < total_size);
 	        return;
 
        size_type new_parent = parent;
        size_type left_child = 2 * parent;
        size_type right_child = 2 * parent + 1;
 	   
        if ((left_child < total_size) && (compare_(elements_[parent], elements_[left_child]))) {
            new_parent = left_child;
        }
        if ((right_child < total_size) && (compare_(elements_[new_parent], elements_[right_child]))) {
            new_parent = right_child;
        }
        if (parent != new_parent) {
            swap_elements(parent, new_parent);
 		    heapify(new_parent);
        }
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void binary_heap_impl<T, Compare, Item_Map>::pop()
    {    
        if (empty())
            return;
 
        /* swap root with last element and delete old root */
        swap_elements(0, elements_.size()-1);
 
        elements_.pop_back();
        rev_index_array.pop_back();
 
        /* Find correct position for new root*/
        heapify(0);
    
    }

    template <class T, 
              class Compare,
              class Item_Map>
    void binary_heap_impl<T, Compare, Item_Map>::update(const value_type& d)
    {
        /* update the element with the new value */
        size_type index = index_array[id_(d)];
        elements_[index] = d;
 
        size_type parent_index = (index - 1) / 2;
 
        /* after update, element may need to move up the heap */
        if (compare_(elements_[index], elements_[parent_index])) {
            while (index > 0){
                parent_index = (index - 1) / 2;
                if (compare_(elements_[index], elements_[parent_index])){

                    /* swap child and parent in the heap structure */
                    swap_elements(index, parent_index);
    
                    /* new index will be the parents index */
                    index = parent_index;
                }
      		    else
                    break;
            }
        }	   
        /* else, element may need to move down the heap */
        else {
            heapify(index);
        }
    }



    template <class T, 
              class Compare,
              class Item_Map = default_t>
    class binary_heap
    {
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef binary_heap_impl<value_type, Compare, Item_Map> search_impl;
            
        public:
            binary_heap(): impl() 
            {}
            
            binary_heap (size_type n,
                const Compare& cmp, const Item_Map& id) : impl(n, cmp, id)
            {}
            
            template<typename ForwardIterator>
            binary_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp, const Item_Map& id):
                           impl (first, last, cmp, id)
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
                return impl.pop();
            }
          
        protected:
            search_impl impl;

    };


    //FIXME: This implementation is not correct

    template <class T, 
              class Compare>
    class binary_heap<T, Compare, default_t>
    {
 
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef typename std::unordered_map<value_type, size_type> internal_map;
            struct item_map {
                internal_map imap;

                public:
                item_map (internal_map &map): imap(map) 
                {}
                
                template<typename value_type>
                size_type operator() (const value_type& key)
                {
                    return imap[key];
                }
            };


            typedef binary_heap_impl<value_type, Compare, item_map> search_impl;

        public:
            binary_heap() 
            {};
            
            binary_heap (size_type n,
                const Compare& cmp): id_(map_), impl (n, cmp, id_)
              
            {}
            
            template<typename ForwardIterator>
            binary_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp): id_(map_),
                           impl (first, last, cmp, id_)
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
                map_[d] = impl.size();
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
          
            inline size_type size() const
            {
               return impl.size();
            }
          
            void pop()
            {   
                //map_.erase(impl.top);
                return impl.pop();
            }
          
        protected:
            internal_map map_;
            item_map id_;
            search_impl impl;

    };

}


#endif // ORIGIN_HEAPS_BINARY_HEAP_HPP
