// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_PAIRING_HEAP_HPP
#define ORIGIN_HEAPS_PAIRING_HEAP_HPP

#include <vector>
#include <iostream>
#include <unordered_map>
#include <origin/utility/meta.hpp>

namespace origin
{
    /* Forward Declaration */
    struct pairing_heap_node
    {
       public:
          size_t item_index;
    
          /* Index of parent of the element in the heap */
          size_t parent;
          
          /* Index of the child element in the heap */
          size_t child;
          
          /* Index of the right neighbour of the element in the heap */
          size_t right_sibling;
          
          /* Index of the left neighbour of the element in the heap */
          size_t left_sibling;
          
          pairing_heap_node ():parent{-1}, child{-1}, right_sibling{-1}, left_sibling{0} {};
    };
    
    /* Class: Mutable Pairing Heap 
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
    class mutable_pairing_heap_impl
    {
       private:
          typedef T value_type;
          typedef size_t size_type;
          
          /* Random access container which holds the heap elements */
          std::vector<T> elements_;
          std::vector<pairing_heap_node> data_;
           
          Compare compare_;
          Item_Map id_;
          
          /*
           * top_ - index of the top element
           */ 
          
          size_type top_;
         
          /*
           * print_recur: Helper function for displaying the pairing heap
           * Input:
           * size_type x : Index of the element
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Prints the nodes for a particular pairing tree identified by x
           * Return Value: None       
           */
          void print_recur(size_type x, std::ostream& os)
          {
             if (x != size_type (-1)) {
                os << elements_[data_[x].item_index];
                size_type i = data_[x].child;
                if (i != size_type(-1)) {
                   os << "(";
                   do {
                      print_recur (i, os);
                      os << " ";
                      i = data_[i].right_sibling;
                   } while (i != size_type(-1));
                   os << ")";
                }
             }
          }
          
          /*
           * merge: Function to merge two pairing heaps
           * Input: 
           * size_type x : Index of the root element of the heap that will become left child 
           * size_type y : Index of the root element of the heap that takes a new left child 
           * Output:
           * Merges the heap pointed to by x to the heap pointed to by y
           * Return Value:
           * None       
           */
          void merge (size_type index1, size_type index2)
          {
              //make index1 heap a left child of index2 heap
              data_[index1].parent = index2;
              data_[index1].right_sibling = data_[index2].child;

              if (data_[index2].child != size_type(-1))
                  data_[data_[index2].child].left_sibling = index1;

              data_[index2].child = index1;
          }

       public:
          /*
           * mutable_pairing_heap_impl: Default constructor
           * Input: 
           * None
           * Output:
           * Return Value:
           * None       
           */
          mutable_pairing_heap_impl () {};
          
          /*
           * mutable_pairing_heap_impl: 3 argument constructor
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
          mutable_pairing_heap_impl (size_type n,
                const Compare &cmp, const Item_Map& id) :
                compare_{cmp}, id_{id}, top_{-1} 
          { 
          }
          
          /*
           * mutable_pairing_heap_impl: range based constructor
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
             mutable_pairing_heap_impl (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp, const Item_Map& id) :
                   compare_{cmp}, 
                   id_{id}, top_{-1}
          {
             while(first != last) {
                push(*first);
                ++first;
             }
          }
          
          /*
           * print: Function for displaying the pairing heap
           * Input:
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Outputs the pairing heap to the specified output stream
           * Return Value:
           * None       
           * Note: This a helper function developed for unit testing
           */
          template<typename Char, typename Traits>
             void print(std::basic_ostream<Char, Traits>& os);
          
          
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
             return elements_[data_[top_].item_index];
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
             return elements_[data_[top_].item_index];
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
           * pairing heap with a new top element
           * Return Value:
           * None
           */
          void pop();
    };
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_pairing_heap_impl<T, Compare, Item_Map>::push(const value_type& d)
    {
        //make a single node heap of the new element
        pairing_heap_node obj;
        size_type index;

        elements_.push_back(d);
    
        obj.item_index = elements_.size() - 1;
        obj.parent = size_type(-1);
        obj.right_sibling = size_type(-1);
        obj.left_sibling = size_type(-1);
        obj.child = size_type(-1);
       
        data_.push_back(obj);
       
        index = data_.size() - 1;
       
        id_(d) = index;

        if (top_ == size_type(-1)) {
            //set top_ pointer and exit if this was the fist element
            top_ = index;
        }
        else {
            //else, unite the 1 element heap with the existing heap
            if (compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index])){
                //make existing root a left child of updated node
                merge(top_, index);
                top_ = index;
            }
            else{
                //make new node a left child of the existing root
                merge(index, top_);
            }
        }
    }
    
    
    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_pairing_heap_impl<T, Compare, Item_Map>::update(const value_type& d)
    {
        //update the value in internal structure
        size_type index = id_(d);
        elements_[data_[index].item_index] = d;

        //if root was modifued, no action required
        if (index == top_)
            return;

        size_type parent = data_[index].parent;
        size_type left_sib = data_[index].left_sibling;
        size_type right_sib = data_[index].right_sibling;

        //remove modified node from the heap
        if (left_sib == size_type(-1)){
            data_[parent].child = right_sib;
        }
        else{
            data_[left_sib].right_sibling = right_sib; 
        }

        if (right_sib != size_type(-1))
            data_[right_sib].left_sibling = left_sib;

        //make modified node a new heap 
        data_[index].parent = size_type(-1);
        data_[index].left_sibling = size_type(-1);
        data_[index].right_sibling = size_type(-1);

        //unite the new heap with the existing heap
        if (compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index])){
            //make existing root a left child of updated node
            merge(top_, index);
            top_ = index;
        }
        else{
            //make new node a left child of the existing root
            merge(index, top_);
        }
    } 

    template <class T, 
              class Compare,
              class Item_Map>
    void mutable_pairing_heap_impl<T, Compare, Item_Map>::pop()
    {
        if (top_ == size_type(-1))
            return;

        //iterate from left to right of the children nodes
        //merge a pair of children, creating paired children
        //and store the indices
        std::vector<size_type> paired_child_heaps;
        size_type new_top_ = size_type(-1);

        size_type index1 = data_[top_].child;
        size_type index2 = size_type(-1);
        size_type next_index = size_type(-1);

        
        while ((index1 != size_type(-1)) && (data_[index1].right_sibling != size_type(-1))){

            index2 = data_[index1].right_sibling;

            next_index = data_[index2].right_sibling;

            if (compare_(elements_[data_[index1].item_index], elements_[data_[index2].item_index])){
                merge(index2, index1);
                paired_child_heaps.push_back(index1);
                new_top_ = index1;
            }
            else{
                merge(index1, index2);
                paired_child_heaps.push_back(index2);
                new_top_ = index2;
            }

            index1 = next_index;
        }

        if ((index1 != size_type(-1)) && (data_[index1].right_sibling == size_type(-1))){
            paired_child_heaps.push_back(index1);
            new_top_ = index1;
        }

        //merge each paired children with the right most paired children going from right to left

        size_type remaining_children = paired_child_heaps.size();
        size_type next_child;

        while (remaining_children > 1){

            next_child = paired_child_heaps[remaining_children - 2];

            if (compare_(elements_[data_[next_child].item_index], elements_[data_[new_top_].item_index])){
                merge(new_top_, next_child);
                new_top_ = next_child;
            }
            else{
                merge(next_child, new_top_);
            }

            remaining_children--;
        }

        paired_child_heaps.clear();
        //where in data_ old last element is stored
        size_type index = id_(elements_[elements_.size()-1]);

        //copy the last element to location of old top element
        elements_[data_[top_].item_index] = elements_[elements_.size()-1];

        //point the item_index of the old element to correct location
        data_[index].item_index = data_[top_].item_index;

        //Invalidating the entries of node
        data_[top_].parent = -1;
        data_[top_].child = -1;
        data_[top_].right_sibling = -1;
        data_[top_].left_sibling = -1;
        data_[top_].item_index = -1;

        elements_.pop_back();

        top_ = new_top_;
    }

    template <class T, 
              class Compare,
              class Item_Map>
        template<typename Char, typename Traits>
    void mutable_pairing_heap_impl<T, Compare, Item_Map>::print(std::basic_ostream<Char, Traits>& os)
    {
        if (top_ != size_type(-1)) {
            print_recur(top_, os);
            os << std::endl;
        } 
    }
    

    /*
     * Pairing Heap implementation used when an external map is provided
     * by the user
     */
    template <class T, 
              class Compare,
              class Item_Map = default_t> 
    class mutable_pairing_heap
    {
        public:
            typedef T value_type;
            typedef size_t size_type;
        
        protected:
            typedef mutable_pairing_heap_impl<value_type, Compare, Item_Map> search_impl;
            
        public:
            mutable_pairing_heap(): impl() 
            {}
            
            mutable_pairing_heap (size_type n,
                const Compare& cmp, const Item_Map& id) : impl(n, cmp, id)
            {}
            
            template<typename ForwardIterator>
            mutable_pairing_heap (ForwardIterator first, ForwardIterator last,
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
                impl.pop();
            }

            template<typename Char, typename Traits>
            void print(std::basic_ostream<Char, Traits>& os) 
            {
                impl.print(os);
            } 
        protected:
            search_impl impl;
    };

    /*
     * Pairing Heap Implementation in case of no external map
     */
    template <class T, 
              class Compare>
    class mutable_pairing_heap<T, Compare, default_t>
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
                size_type & operator() (const value_type& key)
                {
                    return (*imap)[key];
                }
            };


            typedef mutable_pairing_heap_impl<value_type, Compare, item_map> search_impl;

        public:
            mutable_pairing_heap() 
            {};
            
            mutable_pairing_heap (size_type n,
                const Compare& cmp): map_{n}, id_{&map_}, impl (n, cmp, id_)
              
            { 
            }
            
            template<typename ForwardIterator>
            mutable_pairing_heap (ForwardIterator first, ForwardIterator last,
                           const Compare& cmp): id_(map_),
                           impl (first, last, cmp, id_)
            {
            }
 
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
            
            template<typename Char, typename Traits>
            void print(std::basic_ostream<Char, Traits>& os) 
            {
                impl.print(os);
            }
                
        protected:
            internal_map map_;
            item_map id_;
            search_impl impl;

    };
#if 0 
    /*
     * Non-Mutable Binomial Heap Implementation 
     */
    template <class T,
             class Compare>
    class binomial_heap
    {
       private:
          typedef T const value_type;
          typedef size_t size_type;
          
          /* Random access container which holds the heap elements */
          std::vector<T> elements_;
          std::vector<binomial_heap_node> data_;
          std::vector<size_type> reversemap_;
           
          /* Internal map for mapping the values stored in the external map
           * to actual index of the element in the heap
           */
          
          Compare compare_;
          
          /*
           * head_ - index of the root element
           * top_ - index of the top element
           */ 
          
          size_type top_, head_;
         
          /*
           * print_recur: Helper function for displaying the binomial heap
           * Input:
           * size_type x : Index of the element
           * ostresm &os : Reference to the ouput stream
           * Output:
           * Prints the nodes for a particular binomial tree identified by x
           * Return Value: None       
           */
          void print_recur(size_type x, std::ostream& os)
          {
             if (x != size_type (-1)) {
                os << elements_[data_[x].item_index];
                if (data_[x].degree > 0) {
                   os << "(";
                   size_type i = data_[x].child;
                   do {
                      print_recur (i, os);
                      os << " ";
                      i = data_[i].right_sibling;
                   } while (i != size_type(-1));
                   os << ")";
                }
             }
          }
          
          /*
           * merge: Function to merge two binomial heaps
           * Input: 
           * size_type x : Index of the root element of the heap to be merged
           * Output:
           * Merges the heap pointed to by x in the main heap pointed to by head_
           * Return Value:
           * None       
           */
          void merge (size_type index);

          /*
           * mutable_binomial_heap_union: Unites the heap pointed to by head_ with 
           * the heap specified
           * Input: 
           * size_type index: Index of the root element of the heap to be united
           * Output:
           * United binomial heap
           * Return Value:
           * None       
           */
          void binomial_heap_union (size_type index);
           
          /*
           * get_new_top: Function to find the index of new root element
           * Input: 
           * None
           * Output:
           * Index of the new root
           * Return Value:
           * size_type: Index of the new root
           */
          size_type get_new_top(); 
          
       public:
          /*
           * mutable_binomial_heap_impl: Default constructor
           * Input: 
           * None
           * Output:
           * Return Value:
           * None       
           */
          binomial_heap () {};
          
          /*
           * mutable_binomial_heap_impl: 3 argument constructor
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
          binomial_heap (size_type n,
                const Compare &cmp) :
                compare_{cmp}, top_{-1}, head_{-1} 
          { 
          }
          
          /*
           * mutable_binomial_heap_impl: range based constructor
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
             binomial_heap (ForwardIterator first, ForwardIterator last,
                   const Compare &cmp) :
                   compare_{cmp}, top_{-1}, head_{-1}
          {
             while(first != last) {
                push(*first);
                ++first;
             }
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
             void print(std::basic_ostream<Char, Traits>& os);
          
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
             return elements_[data_[top_].item_index];
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
             return elements_[data_[top_].item_index];
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
          inline size_type size() const
          {
             return elements_.size();
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
          void pop();
    };
    
    template <class T, 
              class Compare>
    typename binomial_heap<T, Compare>::size_type 
    binomial_heap<T, Compare>::get_new_top()
    {
       size_type top_index;
       size_type tmp;
       if (head_ == size_type(-1)) {
          return -1;
       }
       
       top_index = head_;
       tmp = data_[top_index].right_sibling;
       
       while (tmp != size_type(-1)) {
          if (compare_(elements_[data_[tmp].item_index], elements_[data_[top_index].item_index])) {
             top_index = tmp;
          }
          
          tmp = data_[tmp].right_sibling;
       }
       
       return top_index;
    }
    
    
    template <class T, 
              class Compare>
    void binomial_heap<T, Compare>::push(const value_type& d)
    {
       binomial_heap_node obj;
       size_type index;
      
       elements_.push_back(d);
    
       obj.item_index = elements_.size() - 1;
       obj.parent = size_type(-1);
       obj.right_sibling = size_type(-1);
       obj.child = size_type(-1);
       obj.degree = 0;
       
       data_.push_back(obj);
       
       index = data_.size() - 1;

       reversemap_.push_back(index);

       if (head_ == size_type(-1)) {
          /* New heap */
          head_ = index;
          top_ = head_;
       } else {
          /* Unite the 1 element heap with the existing heap */
          binomial_heap_union(index);
          if (compare_(elements_[data_[index].item_index], elements_[data_[top_].item_index]))
             top_ = index;
       }
    }
    
    
    template <class T, 
              class Compare>
    void binomial_heap<T, Compare>::merge (size_type index)
    {
       size_type p = head_;
       size_type q = index;
       
       size_type current = data_.size();
       size_type initial_size = -1;
       data_.push_back(binomial_heap_node());
       
       while (p != initial_size || q != initial_size) {
          if (q == initial_size || 
                (p != initial_size && data_[p].degree < data_[q].degree)) {
             data_[current].right_sibling = p;
             current = p;
             p = data_[p].right_sibling;
          } else {
             data_[current].right_sibling = q ;
             current = q;
             q = data_[q].right_sibling;
          }
       }
       
       head_ = data_.back().right_sibling;
       data_.pop_back();
    }
    
    
    template <class T, 
              class Compare>
    void binomial_heap<T, Compare>::binomial_heap_union (size_type index)
    {
       /* Merge the root lists */
       merge(index);
       if (data_[head_].right_sibling == size_type(-1)) {
          return;
       }
       
       size_type x = head_;
       size_type next_x = data_[head_].right_sibling;
       size_type prev_x = size_type(-1);
       
       while (next_x != size_type(-1)) {
          if ((data_[x].degree != data_[next_x].degree) 
                || (data_[next_x].right_sibling != size_type(-1) 
                   && data_[data_[next_x].right_sibling].degree 
                   == data_[x].degree)) {
             prev_x = x;
             x = next_x;
          } else {
             if (compare_ (elements_[data_[x].item_index], elements_[data_[next_x].item_index])) {
                data_[x].right_sibling = data_[next_x].right_sibling;
                binomial_link(data_[next_x], data_[x], next_x, x);
             } else {
                if (prev_x == size_type (-1)) {
                   head_ = next_x;
                } else {
                   data_[prev_x].right_sibling = next_x;
                }
                
                binomial_link(data_[x], data_[next_x], x, next_x);
                x = next_x;
             }
          }
          
          next_x = data_[x].right_sibling;
       }
    }
    
    
    template <class T, 
              class Compare>
    void binomial_heap<T, Compare>::pop()
    {
       size_type new_head = size_type(-1);
       
       if (head_ == size_type(-1)) {
          return;
       }
      
       size_type temp = top_;
    
       /* Reverse the list */
       if (data_[top_].child != size_type(-1)) {
          size_type tmp_head = data_[top_].child;
          size_type tmp_sibling;
          
          while (tmp_head != size_type(-1)) {
             tmp_sibling = data_[tmp_head].right_sibling;
             data_[tmp_head].parent = size_type(-1);
             data_[tmp_head].right_sibling = new_head;
             new_head = tmp_head;
             tmp_head = tmp_sibling;
          }
       }
       
       if (data_[head_].right_sibling == size_type(-1)) {
          head_ = new_head;
       } else {
          if (head_ == top_) {
             head_ = data_[head_].right_sibling;
          } else {
             size_type tmp = head_;
             while (data_[tmp].right_sibling != top_) {
                tmp = data_[tmp].right_sibling;
             }
             
             data_[tmp].right_sibling = data_[top_].right_sibling;
          }
          
          binomial_heap_union(new_head);
       }
       
    
       size_type index = reversemap_.back();
    
       // copy the last element to location of old top element
       elements_[data_[temp].item_index] = elements_[elements_.size()-1];
   
       reversemap_[data_[temp].item_index] = index;

       //point the item_index of the old element to correct location
       data_[index].item_index = data_[temp].item_index;
    
       // Invalidating the entries of node
       data_[temp].parent = -1;
       data_[temp].child = -1;
       data_[temp].right_sibling = -1;
       data_[temp].degree = 0;
       data_[temp].item_index = -1;
    
       elements_.pop_back();
   
       reversemap_.pop_back();

       top_ = get_new_top();
    }
    
    template <class T, 
              class Compare>
       template<typename Char, typename Traits>
    void binomial_heap<T, Compare>::print(std::basic_ostream<Char, Traits>& os)
    {
       if (head_ != size_type(-1)) {
          size_type i = head_;
          do {
             print_recur(i, os);
             os << std::endl;
             i = data_[i].right_sibling;
          } while (i != size_type(-1));
       }
    }
#endif
}
#endif // ORIGIN_HEAPS_PAIRING_HEAP_HPP
