// Copyright (c) 2008-2011 Kent State University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <iostream>
#include <functional>
#include <limits>
#include <cstddef>
#include <tuple>

#include <origin/graph/algorithm/shortest_path/dijkstra.hpp>
#include <origin/graph/algorithm/shortest_path/bellman_ford.hpp>
#include <origin/graph/adjacency_vector/directed.hpp>
#include <origin/graph/traits.hpp>

using namespace origin;

using G = directed_adjacency_vector;
using V = Vertex<G>;
using E = Edge<G>;

int main() {

  // Make a graph.
  G g(5);

  Edge<G> e01 = g.add_edge(V(0),V(1));
  Edge<G> e02 = g.add_edge(V(0),V(2));
  Edge<G> e12 = g.add_edge(V(1),V(2));
  Edge<G> e13 = g.add_edge(V(1),V(3));
  Edge<G> e21 = g.add_edge(V(2),V(1));
  Edge<G> e23 = g.add_edge(V(2),V(3));
  Edge<G> e24 = g.add_edge(V(2),V(4));
  Edge<G> e34 = g.add_edge(V(3),V(4));
  Edge<G> e40 = g.add_edge(V(4),V(0));
  Edge<G> e43 = g.add_edge(V(4),V(3));

  // Make it a weighted graph.
  auto w = label_edges(g,0u);

  w(e01) = 10;
  w(e02) = 5;
  w(e12) = 2;
  w(e13) = 1;
  w(e21) = 3;
  w(e23) = 9;
  w(e24) = 2;
  w(e34) = 4;
  w(e40) = 7;
  w(e43) = 6;

  // run dijkstra
  auto pred_d = dijkstra(g,V(0),w);
  auto pred_b = bellman__ford(g,V(0),w);

  std::cout << "Dijkstra\n";
  for (auto v : vertices(g))
    std::cout << ord(pred_d(v)) << " : " << ord(v) << '\n';

  std::cout << "\nBellman-ford\n";
  for (auto v : vertices(g))
    std::cout << ord(pred_b(v)) << " : " << ord(v) << '\n';

  return 0;
}