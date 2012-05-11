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
  /*class undirected_adjacency_vector
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
  };*/
  template<typename Vertex_data, typename Edge_data>
  class undirected_adjacency_vector
  {
    // This is the vector that holds the vectors oof adjacent vertices
    using sub_vector_type = std::vector<vertex_handle<std::size_t>>;
    using main_vector_type = std::vector<sub_vector_type>;
    using size_type = main_vector_type::size_type;
  public:
    // Types
    using vertex = vertex_handle<std::size_t>;
    using edge = undirected_edge_handle<std::size_t>;

    using vertex_range =
    using edge_range =
    using incident_edge_range = 


    // Initializers
    undirected_adjacency_vector(size_type n = 0)
      : edge_count_{0}, main_vec_(n)
    { }
    undirected_adjacency_vector(this_type const& g)
      : edge_count_{g.edge_count_}, main_vec_(g.main_vec_)
    { }


    // Graph Metrics
    bool empty()      const { return edges_.empty(); }
    size_type size()  const { return edges_.size(); }
    bool null()       const { return main_vec_.empty(); }
    size_type order() const { return main_vec_.size(); }


    // Structural Mutators
    vertex add_vertex();
    edge add_edge(vertex u, vertex u);
    void remove_edge(edge e);
    void remove_edges(vertex u, vertex u);

  private:
    std::vector<edge> edges_;
    main_vector_type main_vec_;
  };

/*============================================================================*/
  auto undirected_adjacency_vector::add_vertex() -> vertex
  {
    main_vec_.push_back();
    return --order();
  }

  auto undirected_adjacency_vector::add_edge(vertex v, vertex u) -> edge
  {
    //make edge
    edge e(--edges_.size(), v.value, u.value);
    
    return e;
  }

  void undirected_adjacency_vector::remove_edge(edge e)
  {

  }

  auto undirected_adjacency_vector::in_degree(vertex v) const -> size_type
  { return out_degree(v); }

  auto undirected_adjacency_vector::degree(vertex v) const -> size_type
  { return out_degree(v); }




/*============================================================================*/
  class undirected_adjacency_vector_other
  {
  public:

    // Graph metrics
S   empty
S   size
S   null
S   order

    // Structural mutators
S   add_vertex
D   remove_vertex
D   add_edge
D   remove_edge

    // Structural queries
    ?

    // Data accessors
S   vertices
D   edges
U   incident_edges
  };

  class directed_adjacency_vector_other
  {
  public:

    // Graph metrics
S   empty
S   size
S   null
S   order

    // Structural mutators
S   add_vertex
D   remove_vertex
D   add_edge
D   remove_edge

    // Structural queries
    ?

    // Data accessors
S   vertices
    edges
U   out_edges
U   in_edges
  };


}

#endif // ORIGIN_GRAPH_ADJACECNY_VECTOR_UNDIRECTED_HPP