// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GRAPH_ADJACECNY_VECTOR_UNDIRECTED_HPP
#define ORIGIN_GRAPH_ADJACECNY_VECTOR_UNDIRECTED_HPP

#include <origin/graph/adjacency_vector/impl.hpp>

namespace origin {


  // Undirected adjacecny vector
  // This is a graph type whose representation is a vector of vectors. It uses
  // the adjacency_vector_base type. Undirected behavior is implemented
  // redundantly. Esentially, an edge is stored twice.

  // FIXME How is this going to store user data?
  // NOTE 
  class undirected_adjacency_vector
    : private adjacency_vector_detail_::adjacency_vector_base
  {
    using base_type = adjacency_vector_detail_::adjacency_vector_base;
    using this_type = undirected_adjacency_vector;
    using size_type = base_type::size_type;
  public:

    // Types
    using vertex = base_type::vertex;
    using edge = base_type::edge;
    using vertex_range = base_type::vertex_range;
    using out_edge_range = base_type::out_edge_range;


    // Initializers
    undirected_adjacency_vector(size_type n = 0) : base_type(n) { }
    undirected_adjacency_vector(this_type const& g);


    // Graph Metrics
    using base_type::empty;
    using base_type::size;
    using base_type::null;
    using base_type::order;

    using base_type::out_degree;
    size_type in_degree(vertex v) const;
    size_type degree(vertex v) const;


    // Structural Mutators
    using base_type::add_vertex;
    void remove_vertex(vertex v);
    edge add_edge(vertex u, vertex v);
    void remove_edge(vertex u, vertex v);


    // Structural Queries
    bool is_edge(edge e) const;
    bool is_edge(vertex u, vertex v) const;


    // Data accessors
    using base_type::vertices;
    void edges();
    using base_type::out_edges;
    void in_edges(vertex v);
    void incident_edges(vertex v);
  };

/*============================================================================*/
  auto undirected_adjacency_vector::in_degree(vertex v) const -> size_type
  { return out_degree(v); }

  auto undirected_adjacency_vector::degree(vertex v) const -> size_type
  { return out_degree(v); }
}

#endif // ORIGIN_GRAPH_ADJACECNY_VECTOR_UNDIRECTED_HPP