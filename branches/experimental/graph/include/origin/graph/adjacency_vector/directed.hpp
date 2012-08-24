// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_GRAPH_ADJACECNY_VECTOR_DIRECTED_HPP
#define ORIGIN_GRAPH_ADJACECNY_VECTOR_DIRECTED_HPP

// FIXMEs
//  - directed and undirected can share the iterator types

#include <tuple> 

namespace origin {

/*============================================================================*/
  // Helpers
  namespace {
    // This optimizes void out of the undirected adjacency vector
    // It is essentially a heterogeneous tuple with 2 or 3 fields
    template <typename Size_t, typename T>
      struct tuple_trim_void_impl { using type = std::tuple<Size_t,Size_t,T>; };

    template <typename Size_t>
      struct tuple_trim_void_impl<Size_t,void>
      { using type = std::tuple<Size_t,Size_t>; };

    template <typename Size_t, typename Edge_t>
      using tuple_trim_void =
        typename tuple_trim_void_impl<Size_t,Edge_t>::type;


    // Predicate to find a vertex with an edge
    template <typename Size_t, typename Edge_t, typename Vertex_t>
    struct target_of_edge
    {
    private:
      using internal_vertex = tuple_trim_void<Size_t,Vertex_t>;
      using internal_edge   = tuple_trim_void<Size_t,Edge_t>;
    public:
      target_of_edge(std::size_t n) : edge_index(n) { }

      bool operator() (internal_vertex v)
      { return std::get<1>(v) == edge_index; }

      std::size_t edge_index;
    };
  } // namespace



/*============================================================================*/
  // Edge Iterator
namespace dav {

    // Directed adjacency vector edge iterator
    class edge_iterator
    {
      using size_type     = std::size_t;
      using internal_edge = std::tuple<size_type,size_type>;
      using edge_vec_iter = std::vector<internal_edge>::const_iterator;
      using edge          = undirected_edge_handle<size_type>;
    public:
      using value_type        = edge;
      using reference         = edge;
      using pointer           = edge;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      // Initializers
      edge_iterator(edge_vec_iter first, size_type n = 0)
        : first_(first), i_(n)
      { }

      edge_iterator(edge_iterator const& ei)
        : first_(ei.first_), i_(ei.i_)
      { }

      edge_iterator(edge_iterator && ei)
        : first_(ei.first_), i_(ei.i_)
      { }

      edge_iterator& operator= (edge_iterator ei)
      { first_ = ei.first_; i_ = ei.i_; return *this; }

      // Readable
      reference operator* () const { return make_edge(); }
      //pointer operator-> () const;

      // Equality comparable
      bool operator== (edge_iterator ei) const { return i_ == ei.i_; }
      bool operator!= (edge_iterator ei) const { return i_ != ei.i_; }

      // Totally ordered
      bool operator<(edge_iterator ei) const { return i_ < ei.i_; }
      bool operator>(edge_iterator ei) const { return i_ > ei.i_; }
      bool operator<=(edge_iterator ei) const { return i_ <= ei.i_; }
      bool operator>=(edge_iterator ei) const { return i_ >= ei.i_; }

      // Incrementable
      edge_iterator& operator++ () { ++i_; return *this; }
      edge_iterator operator++ (int) {
        edge_iterator t(*this);
        ++i_;
        return t;
      }

      // Decrementable
      edge_iterator& operator-- () { --i_; return *this; }
      edge_iterator operator-- (int) {
        edge_iterator t(*this);
        --i_;
        return t;
      }

    private:
      edge_vec_iter first_;
      size_type i_;

      // Helpers
      // wraps the iterator position into an edge
      edge make_edge() const
      { 
        return edge(
          i_,
          std::get<0>(*(first_ + i_)),
          std::get<1>(*(first_ + i_))
        );
      }
    };



/*============================================================================*/
  // Incident Edge Iterator
    class out_edge_iterator
    {
      using size_type     = std::size_t;
      using internal_edge = std::tuple<size_type,size_type>;
      using edge_vec_iter = std::vector<internal_edge>::const_iterator;
      using edge          = undirected_edge_handle<size_type>;
    public:
      using value_type        = edge;
      using reference         = edge;
      using pointer           = edge;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      // Initializers
      out_edge_iterator(edge_vec_iter neighbors, edge_vec_iter edges)
        : adj_(neighbors), edges_(edges)
      { }

      out_edge_iterator(out_edge_iterator const& ei)
        : adj_(ei.adj_), edges_(ei.edges_)
      { }

      out_edge_iterator(out_edge_iterator && ei)
        : adj_(ei.adj_), edges_(ei.edges_)
      { }

      out_edge_iterator& operator= (out_edge_iterator ei)
      { adj_ = ei.adj_; edges_ = ei.edges_; return *this; }

      // Readable
      reference operator* () const { return make_edge(); }
      //pointer operator-> () const;

      // Equality comparable
      bool operator== (out_edge_iterator ei) const
      { return adj_ == ei.adj_; }
      bool operator!= (out_edge_iterator ei) const
      { return adj_ != ei.adj_; }

      // Totally ordered
      bool operator<(out_edge_iterator ei) const
      { return adj_ < ei.adj_; }
      bool operator>(out_edge_iterator ei) const
      { return adj_ > ei.adj_; }
      bool operator<=(out_edge_iterator ei) const
      { return adj_ <= ei.adj_; }
      bool operator>=(out_edge_iterator ei) const
      { return adj_ >= ei.adj_; }

      // Incrementable
      out_edge_iterator& operator++ () { ++adj_; return *this; }
      out_edge_iterator operator++ (int) {
        out_edge_iterator t(*this);
        ++adj_;
        return t;
      }

      // Decrementable
      out_edge_iterator& operator-- () { --adj_; return *this; }
      out_edge_iterator operator-- (int) {
        out_edge_iterator t(*this);
        --adj_;
        return t;
      }

    private:
      edge_vec_iter adj_;
      edge_vec_iter edges_;

      // Helpers
      // wraps the iterator position into an edge
      edge make_edge() const
      {
        size_type n = std::get<1>(*adj_);
        return edge(n, std::get<0>(*(edges_ + n)), std::get<1>(*(edges_ + n)));
      }
    };

  } // namespace dav

/*============================================================================*/
  // directed adjacency vector
  class directed_adjacency_vector
  {
    using size_type       = std::size_t;
    using internal_vertex = tuple_trim_void<size_type,void>; // (target,edge)
    using internal_edge   = tuple_trim_void<size_type,void>; // (src,target)
    using adj_vec         = std::vector<internal_vertex>;
    using vertex_vec      = std::vector<adj_vec>;
    using edge_vec        = std::vector<internal_edge>;
  public:
    // Types
    using vertex = vertex_handle<size_type>;
    using edge   = undirected_edge_handle<size_type>;

    using vertex_range   = bounded_range<vertex_iterator<size_type>>;
    using edge_range     = bounded_range<dav::edge_iterator>;
    using out_edge_range = bounded_range<dav::out_edge_iterator>;

    using vertex_value_type = void;
    using edge_value_type   = void;


    // Initializers
    directed_adjacency_vector(size_type n) : neighbors_(n), edges_() {}
    directed_adjacency_vector(const directed_adjacency_vector& g)
      : neighbors_(g.neighbors_), edges_(g.edges_)
    { }
    //directed_adjacency_vector(directed_adjacency_vector && g)


    // Graph Metrics
    bool empty()      const { return edges_.empty(); }
    size_type size()  const { return edges_.size(); }
    bool null()       const { return neighbors_.empty(); }
    size_type order() const { return neighbors_.size(); }

    size_type out_degree (vertex v) const { return neighbors_[ord(v)].size(); }


    // Structural Mutators
    vertex add_vertex();
    edge add_edge(vertex u, vertex v);
    void remove_edge(edge e);
    //void remove_edges(vertex u, vertex u);


    // Ranges
    vertex_range vertices() const;
    edge_range edges() const;
    out_edge_range out_edges(vertex v) const;


  private:
    vertex_vec neighbors_;
    edge_vec edges_;

    // Helper functions
    // Removes a vertex adjacent to vertex v incedent to edge e
    void remove_adjacent_vertex_by_edge(vertex v, edge e);
  };



  auto directed_adjacency_vector::add_vertex() -> vertex
  {
    neighbors_.push_back(adj_vec());
    return vertex(order() - 1);
  }

  auto directed_adjacency_vector::add_edge(vertex u, vertex v) -> edge
  {
    // Add the edge to the edge list
    edges_.push_back(internal_edge(ord(u),ord(v)));
    // Add vertex
    neighbors_[ord(u)].push_back(internal_vertex(ord(v),size() - 1));

    return edge(size() - 1, ord(u), ord(v));
  }

  void directed_adjacency_vector::remove_edge(edge e)
  {
    // remove from neighbors list
    edges_.erase(edges_.begin() + ord(e));
    // remove from edge list
    remove_adjacent_vertex_by_edge(target(e), e);
  }

  void
  directed_adjacency_vector::remove_adjacent_vertex_by_edge(vertex v, edge e)
  {
    // find vertex
    size_type e_index = ord(e);
    auto pos = std::find_if(        // Needs to be replaced.
      neighbors_[ord(v)].begin(),
      neighbors_[ord(v)].end(),
      target_of_edge<size_type,void,void>(e_index));
    // remove edge
    neighbors_[ord(v)].erase(pos);
  }

  auto directed_adjacency_vector::vertices() const -> vertex_range
  {
    return vertex_range(
      vertex_iterator<size_type>(0),
      vertex_iterator<size_type>(order())
    );
  }

  auto directed_adjacency_vector::edges() const -> edge_range
  {
    return edge_range(
      dav::edge_iterator(edges_.begin(), 0),
      dav::edge_iterator(edges_.begin(), size())
    );
  }

  auto directed_adjacency_vector::out_edges(vertex v) const
    -> out_edge_range
  {
    return out_edge_range(
      dav::out_edge_iterator(
        neighbors_[ord(v)].begin(), edges_.begin()),
      dav::out_edge_iterator(
        neighbors_[ord(v)].end(), edges_.begin())
    );
  }

} // namespace origin

#endif // ORIGIN_GRAPH_ADJACECNY_VECTOR_DIRECTED_HPP