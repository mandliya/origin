// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GRAPH_ALGORITHM_SPANNING_TREE_PRIM_HPP
#define ORIGIN_GRAPH_ALGORITHM_SPANNING_TREE_PRIM_HPP

// FIXMEs

#include <origin/graph/traits.hpp>
#include <origin/graph/label.hpp>

namespace origin
{

  namespace
  {
/*============================================================================*/
    class disjoint_int_set {
      struct simple_node
      {
        simple_node() : next (this) { }
        bool operator== (simple_node a) { return next == a.next; }
        simple_node* next;
      };
    public:
      using size_type  = typename std::vector<simple_node>::size_type;

    private:
      std::vector<simple_node> store;
      size_type set_count;

    public:
      disjoint_int_set () : store(), set_count() { }
      disjoint_int_set (size_type n) : store(n), set_count(n) { }

      bool same_set (int a, int b) const
      { return root_of (store[a]) == root_of (store[b]) ? true : false; }

      void merge_sets (int a, int b)
      {
        simple_node a_node = root_of (store[a]);
        --set_count;
        a_node.next->next = &store[b];
      }

      void safe_merge_sets (int a, int b)
      {
        simple_node a_node = root_of (store[a]);

        // Ensure that a and b are in different sets
        if(root_of (store[b]) == a_node)
          return;

        --set_count;
        a_node.next->next = &store[b];
      }

      size_type size() const { return set_count; }

    private:
      // helper functions
      simple_node root_of (simple_node a)
      {
        while (a.next->next != a.next)
          a = *a.next;

        return a;
      }
    };

    template <typename V>
    V nil_vertex() { return V(max_val<V>()); }
/*============================================================================*/
  }

  template <typename G, typename Edge_weight>
    labeling <Vertex<G>,Vertex<G>>
    kruskal (const G& g,Edge_weight & w)
    {
      auto pred = label_vertices (g, nil_vertex<Vertex<G>>());
      
      // Push all edges into a queue
      auto q = make_weight_queue(w);
      for (auto e : edges(g))
        q.push(e);

      // make distinct sets
      disjoint_int_set sets (g.order());

      // Second short circuit only good for 
      while (!q.empty() && (sets.size() != 1))
      {
        // pop q
        Edge<G> e = q.top();
        q.pop();

        // if the vertices aren't in the same set, merge them
        Vertex<G> u = source(e);
        Vertex<G> v = target(e);
        if (!sets.same_set (ord(u),ord(v)) {
          sets.merge_sets (ord(u),ord(v);
          pred(u) = v;
        }
      }
    }

}

#endif // ORIGIN_GRAPH_ALGORITHM_SPANNING_TREE_PRIM_HPP