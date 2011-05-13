// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// The purpose of this file is to simply ensure that all header files in the
// heaps library compile without warnings or errors.

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
//#include <boost/pending/indirect_cmp.hpp>
#include <origin/heaps/binomial_heap.hpp>
#include <origin/heaps/binary_heap.hpp>
#include <origin/heaps/fibonacci_heap.hpp>
#include <origin/heaps/pairing_heap.hpp>
#include <iterator>

using namespace origin;
using namespace std;

template<class Map, class Compare>
class indirect_compare {
  public:
    indirect_compare(const Map& df, const Compare& c): d(df), cmp(c)
    {}

    template <class A, class B>
    bool operator()(A u, B v) const
    {
        return cmp(d[u], d[v]);
    }
  protected:
    Map d;
    Compare cmp;
};

template <class Heap, class T>
int test_heap_external_map(T &item_label, int node) 
{
  
    mt19937 gen;
    int i;
   
    typedef indirect_compare<float*, std::greater<float>> ICmp;
    for (int N = node; N < node+1; ++N) {
        uniform_int_distribution<> distrib(0, N-1);
        auto rand_gen = std::bind(distrib, gen);

        for (int t = 0; t < 1; ++t) {
            std::vector<float> v, w(N);
            ICmp cmp(&w[0], std::greater<float>());

            Heap Q(cmp);
   
            for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;

            std::random_shuffle(w.begin(), w.end());
            
            for (i = 0; i < N; ++i)
                Q.push(i);
           
            for (i = 0; i < N; ++i) {
                int u = rand_gen();
                float r = rand_gen();
                r /= 2.0;
                w[u] = w[u] - r;
                Q.update(u);
            }
           
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }

            //Perform the above process again in the empty heap
            //This tests the correctness of the heap in the scenario
            //push () --> pop () and then again push()
            for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;
            std::random_shuffle(w.begin(), w.end());
            v.clear();

            for (i = 0; i < N; ++i)
                Q.push(i);
        
            for (i = 0; i < N; ++i) {
                int u = rand_gen();
                float r = rand_gen(); r /= 2.0;
                w[u] = w[u] - r;
                Q.update(u);
            }
           
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                  std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }
        }
    }

    return 1;
}

template <class Heap>
int test_heap_internal_map(int node) 
{
  
    mt19937 gen;
    int i;
   
    typedef indirect_compare<float*,std::greater<float>> ICmp;
    for (int N = node; N < node+1; ++N) {
        uniform_int_distribution<> distrib(0, N-1);
        auto rand_gen = std::bind(distrib, gen);
        
        for (int t = 0; t < 1; ++t) {
            std::vector<float> v, w(N);
            ICmp cmp(&w[0], std::greater<float>());

            Heap Q(cmp);
    

           for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;
            
            std::random_shuffle(w.begin(), w.end());
            for (i = 0; i < N; ++i)
                Q.push(i);
            
            for (i = 0; i < N; ++i) {
                int u = rand_gen();
                float r = rand_gen(); r /= 2.0;
                w[u] = w[u] - r;
                Q.update(u);
            }
            
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }
            
            for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;
            std::random_shuffle(w.begin(), w.end());
            v.clear();

            for (i = 0; i < N; ++i)
                Q.push(i);
            for (i = 0; i < N; ++i) {
                int u = rand_gen();
                float r = rand_gen(); r /= 2.0;
                w[u] = w[u] - r;
                Q.update(u);
            }
            
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                  std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }
        }
    }

    return 1;
}

template <class Heap>
int test_non_mutable_heap(int node) 
{
    int i;
   
    typedef indirect_compare<float*,std::greater<float>> ICmp;
    for (int N = node; N < node+1; ++N) {
        
        for (int t = 0; t < 1; ++t) {
            std::vector<float> v, w(N);
            ICmp cmp(&w[0], std::greater<float>());

            Heap Q(cmp);
    

           for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;
            
            std::random_shuffle(w.begin(), w.end());
            for (i = 0; i < N; ++i)
                Q.push(i);
            
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }
            
            for (int c = 0; c < int(w.size()); ++c)
                w[c] = c;
            std::random_shuffle(w.begin(), w.end());
            v.clear();

            for (i = 0; i < N; ++i)
                Q.push(i);
            
            for (i = 0; i < N; ++i) {
                v.push_back(w[Q.top()]);
                Q.pop();
            }
            
            std::sort(w.begin(), w.end());

            if (! std::equal(v.begin(), v.end(), w.begin())) {
                std::cout << std::endl << "heap sorted: ";
                std::copy(v.begin(), v.end(), 
                        std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl << "correct: ";
                std::copy(w.begin(), w.end(), 
                  std::ostream_iterator<float>(std::cout," "));
                std::cout << std::endl;
                return -1;
            }
        }
    }

    return 1;
}



int main()
{
    int result;
    typedef indirect_compare<float*,std::greater<float>> ICmp;

//     std::unordered_map<int, size_t> id_;
//     auto item_label = [&](int x) -> size_t &{return id_[x];};

    //number of elements in the heap
    int node = 10;
  /*   
    typedef mutable_binomial_heap<int, ICmp, decltype(item_label)> mbinomial;
    
    result = test_heap_external_map<mbinomial>(item_label, node);
    
    if (result > 0) {
        std::cout << "mutable binomial heap passed test" << std::endl; 
    } else {
        std::cout<<"mutable binomial heap test failed \n";
    }
 
    typedef mutable_fibonacci_heap<int, ICmp, decltype(item_label)> mfibonacci;
    
    result = test_heap_external_map<mfibonacci>(item_label, node);
    
    if (result > 0) {
        std::cout << "mutable fibonacci heap passed test" << std::endl; 
    } else {
        std::cout<<"mutable fibonacci heap test failed \n";
    }
*/
  
  // FIXME: This test needs to be updated.
    typedef mutable_binary_heap<int, ICmp> mbinary;
    
//     result = test_heap_external_map<mbinary>(item_label, node);
    
//     if (result > 0) {
//         std::cout << "mutable binary heap passed test" << std::endl; 
//     } else {
//         std::cout<<"mutable binary heap test failed \n";
//     }
    
/* 
    typedef mutable_pairing_heap<int, ICmp, decltype(item_label)> mpairing;
    
    result = test_heap_external_map<mpairing>(item_label, node);
    
    if (result > 0) {
        std::cout << "mutable pairing heap passed test" << std::endl; 
    } else {
        std::cout<<"mutable pairing heap test failed \n";
    }
   
    typedef mutable_binomial_heap<int, ICmp> mbinomial_int;
    
    result = test_heap_internal_map<mbinomial_int>(node);
    
    if (result > 0) {
        std::cout << "mutable binomial heap (internal map) passed test" << std::endl; 
    } else {
        std::cout<<"mutable binomial heap (internal map) test failed \n";
    }

    typedef mutable_fibonacci_heap<int, ICmp> mfibonacci_int;
    
    result = test_heap_internal_map<mfibonacci_int>(node);
    
    if (result > 0) {
        std::cout << "mutable fibonacci heap (internal map) passed test" << std::endl; 
    } else {
        std::cout<<"mutable fibonacci heap (internal map) test failed \n";
    }

    typedef mutable_binary_heap<int, ICmp> mbinary_int;
    
    result = test_heap_internal_map<mbinary_int>(node);
    
    if (result > 0) {
        std::cout << "mutable binary heap(internal_map) passed test" << std::endl; 
    } else {
        std::cout<<"mutable binary heap (internal map) test failed \n";
    }
*/
  /*
    typedef mutable_pairing_heap<int, ICmp> mpairing_int;
    
    result = test_heap_internal_map<mpairing_int>(node);
    
    if (result > 0) {
        std::cout << "mutable pairing heap(internal map) passed test" << std::endl; 
    } else {
        std::cout<<"mutable pairing heap(internal map) test failed \n";
    }

    typedef binomial_heap<int, ICmp> binomial;
 
    result = test_non_mutable_heap<binomial>(node);
    
    if (result > 0) {
        std::cout << "non-mutable binomial heap passed test" << std::endl; 
    } else {
        std::cout<<"non-mutable binomial heap test failed \n";
    }

    typedef fibonacci_heap<int, ICmp> fibonacci;
 
    result = test_non_mutable_heap<fibonacci>(node);
    
    if (result > 0) {
        std::cout << "non-mutable fibonacci heap passed test" << std::endl; 
    } else {
        std::cout<<"non-mutable fibonnacci heap test failed \n";
    }

    typedef binary_heap<int, ICmp> binary;
 
    result = test_non_mutable_heap<binary>(node);
    
    if (result > 0) {
        std::cout << "non-mutable binary heap passed test" << std::endl; 
    } else {
        std::cout<<"non-mutable binary heap test failed \n";
    }
 
    typedef pairing_heap<int, ICmp> pairing;
    
    result = test_non_mutable_heap<pairing>(node);
    
    if (result > 0) {
        std::cout << "non-mutable pairing heap passed test" << std::endl; 
    } else {
        std::cout<<"non-mutable pairing heap test failed \n";
    }
   std::cout<<"\n";
*/   
   return 0;
}
