// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GRAPH_ALGORITHM_SHORTEST_PATH_DIJKSTRA
#define ORIGIN_GRAPH_ALGORITHM_SHORTEST_PATH_DIJKSTRA

#include <cassert>
#include <iostream>

#include <origin/graph/label.hpp>
#include <origin/graph/traits.hpp>


namespace origin {


  // Helper functions
  namespace {
  template <typename T>
    constexpr T max_val() { return std::numeric_limits<T>::max(); }

  // The Nil Vertex
    // FIXME move this into vertex.hpp
  template <typename V>
    V nil_vertex() { return V(max_val<V>()); }
  }

  // Note that this version queues all vertices right away.
  template <typename G, typename Edge_weight>
    labeling<Vertex<G>, Vertex<G>>
    dijkstra(const G& g, Vertex<G> s, Edge_weight& w)
    {
      // make labels
      auto max = max_val<Value_type<Edge_weight>>();
      auto w_v = label_vertices(g,max);
      auto pred = label_vertices(g,nil_vertex<Vertex<G>>());

      // Queue all vertices and initialize
      auto q = make_weight_queue(w_v);
      for (Vertex<G> v : vertices(g))
        q.push(v);
      w_v(s) = 0;

      // Main algorithm
      while (!q.empty()) {
        // get new vertex
        Vertex<G> u = q.top();

        // If this elements weight is max or more, exit. We're done.
        // There was the notion of clamping, but this relieves that
        // Overflow is a USER problem. 
        if (!(w_v(u) < max))
          return pred;

        // relax all neighbors
        for (Edge<G> e : out_edges(g,u)) {

          // weight must increase
          assert (( !(w_v(u) > w_v(u) + w(e)) ));

          // relax the edge
          Vertex<G> v = opposite(e,u);
          if (w_v(u) + w(e) < w_v(v)){
            w_v(v) = w_v(u) + w(e);
            pred(v) = u;
            q.decrease(v);
          }
        }
        q.pop();
      }

      return pred;
    }

  // The case where the graph edge values are the edge weights
  template <typename G, typename Edge_weight>
    inline
    labeling<Vertex<G>, Vertex<G>>
    dijkstra(const G& g, Vertex<G> s)
    { return dijkstra (g,s,edge_weight_graph_wrapper (g)); }

} // namespace origin

#endif // ORIGIN_GRAPH_ALGORITHM_SHORTEST_PATH_DIJKSTRA
