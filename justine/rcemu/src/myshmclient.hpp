#ifndef ROBOCAR_MYSHMCLIENT_HPP
#define ROBOCAR_MYSHMCLIENT_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient.hpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <smartcity.hpp>
#include <car.hpp>

#include <cstdlib>
#include <iterator>

#include <boost/asio.hpp>

#include <limits>

#include <memory>

#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

#include <shmclient.hpp>

#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <chrono>
#endif

#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include <boost/graph/graphviz.hpp>
#include <fstream>

namespace justine
{
namespace sampleclient
{

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
        boost::property<boost::vertex_name_t, osmium::unsigned_object_id_type>,
        boost::property<boost::edge_weight_t, int>> NodeRefGraph;

typedef boost::graph_traits<NodeRefGraph>::vertex_descriptor NRGVertex;
typedef boost::graph_traits<NodeRefGraph>::vertex_iterator NRGVertexIter;
typedef boost::graph_traits<NodeRefGraph>::edge_descriptor NRGEdge;
typedef boost::graph_traits<NodeRefGraph>::edge_iterator NRGEdgeIter;

typedef boost::property_map<NodeRefGraph, boost::vertex_name_t>::type VertexNameMap;
typedef boost::property_map<NodeRefGraph, boost::vertex_index_t>::type VertexIndexMap;

typedef boost::iterator_property_map <NRGVertex*, VertexIndexMap, NRGVertex, NRGVertex&> PredecessorMap;
typedef boost::iterator_property_map <int*, VertexIndexMap, int, int&> DistanceMap;

typedef boost::property_map<NodeRefGraph, boost::edge_weight_t>::type EdgeWeightMap;

/**
 * @brief A sample class used for testing the routing algorithms.
 *
 * This sample class shows how client agents can create BGL graph from data can be found in the shared memory.
 *
 * @author Norbert Bátfai
 * @date Dec. 7, 2014
 */
class MyShmClient : public ShmClient
{
public:

  /**
   * @brief This constructor creates the BGL graph from the map graph.
   * @param shm_segment the shared memory object name
   *
   * This constructor creates the BGL graph from the map graph that
   * is placed in the shared memory segment.
   */
  MyShmClient ( const char * shm_segment, std::string teamname ) : ShmClient ( shm_segment ), m_teamname ( teamname )
  {

    nr_graph = bgl_graph();

#ifdef DEBUG
    print_vertices ( 10 );
    print_edges ( 10 );
    std::fstream graph_log( teamname+".dot" , std::ios_base::out );
    boost::write_graphviz(graph_log, *nr_graph);		   
#endif

  }

  /**
   * @brief Dtor
   *
   */
  ~MyShmClient()
  {

    delete nr_graph;

  }

  /**
   * @brief This function starts the client.
   * @param io_service
   * @param port the TCP port of the traffic server
   *
   * This method does the following: retrieves a value from shared memory,
   * then establishes a connection with the traffic server, finally
   * sends some client commands.
   */
  void start ( boost::asio::io_service& io_service, const char * port );

  void start10 ( boost::asio::io_service& io_service, const char * port );

  /**
   * @brief This function counts the number of vertices and number of edges in the map graph.
   * @param [out] sum_edges the number of edges
   * @return the number of vertices
   *
   * This function counts the number of vertices and number of edges in the map graph that
   * is placed in the shared memory segment.
   */
  int num_vertices ( int &sum_edges )
  {

    std::set<osmium::unsigned_object_id_type> sum_vertices;

    for ( justine::robocar::shm_map_Type::iterator iter=shm_map->begin();
          iter!=shm_map->end(); ++iter )
      {

        sum_vertices.insert ( iter->first );
        sum_edges+=iter->second.m_alist.size();

        for ( auto noderef : iter->second.m_alist )
          {
            sum_vertices.insert ( noderef );
          }

      }

    return sum_vertices.size();
  }

  /**
   * @brief This function prints the edges of the map graph.
   * @param more the maximum number of printed items
   *
   */
  void print_edges ( unsigned more )
  {

    VertexNameMap vertexNameMap = boost::get ( boost::vertex_name, *nr_graph );

    std::pair<NRGVertexIter, NRGVertexIter> vi;

    unsigned count {0};

    for ( vi = boost::vertices ( *nr_graph ); vi.first != vi.second; ++vi.first )
      {
        if ( more )
          if ( ++count > more ) break;

        std::cout << vertexNameMap[*vi.first] <<  " ";
      }
    std::cout << std::endl;

  }

  /**
   * @brief This function prints the vertices of the map graph.
   * @param more the maximum number of printed items
   *
   */
  void print_vertices ( unsigned more )
  {
    VertexNameMap vertexNameMap = boost::get ( boost::vertex_name, *nr_graph );

    unsigned count {0};

    osmium::unsigned_object_id_type prev = 0;
    NRGEdgeIter ei, ei_end;

    for ( boost::tie ( ei, ei_end ) = boost::edges ( *nr_graph ); ei != ei_end; ++ei )
      {
        auto ii = vertexNameMap[boost::source ( *ei, *nr_graph )];

        if ( ii != prev )
          std::cout << std::endl;

        std::cout << "(" << ii
                  << " -> " << vertexNameMap[boost::target ( *ei, *nr_graph )] << ") ";

        prev = ii;

        if ( more )
          if ( ++count > more ) break;

      }

    std::cout << std::endl;

  }

  /**
   * @brief This function create the BGL graph.
   * @return he pointer of the created BGL graph.
   *
   */
  NodeRefGraph* bgl_graph ( void )
  {

    NodeRefGraph* nr_graph = new NodeRefGraph();

    int count {0};

    for ( justine::robocar::shm_map_Type::iterator iter=shm_map->begin();
          iter!=shm_map->end(); ++iter )
      {

        osmium::unsigned_object_id_type u = iter->first;

        for ( justine::robocar::uint_vector::iterator noderefi = iter->second.m_alist.begin();
              noderefi!=iter->second.m_alist.end();
              ++noderefi )
          {

            NodeRefGraph::vertex_descriptor f;
            std::map<osmium::unsigned_object_id_type, NRGVertex>::iterator it = nr2v.find ( u );

            if ( it == nr2v.end() )
              {

                f = boost::add_vertex ( u, *nr_graph );
                nr2v[u] = f;

                ++count;

              }
            else
              {

                f = it->second;

              }

            NodeRefGraph::vertex_descriptor t;
            it = nr2v.find ( *noderefi );
            if ( it == nr2v.end() )
              {

                t = boost::add_vertex ( *noderefi, *nr_graph );
                nr2v[*noderefi] = t;

                ++count;

              }
            else
              {

                t = it->second;

              }

            int to = std::distance ( iter->second.m_alist.begin(), noderefi );

            boost::add_edge ( f, t, palist ( iter->first, to ), *nr_graph );

          }

      }

#ifdef DEBUG
    std::cout << "# vertices count: " << count << std::endl;;
    std::cout << "# BGF edges: " << boost::num_edges ( *nr_graph ) << std::endl;;
    std::cout << "# BGF vertices: " << boost::num_vertices ( *nr_graph ) << std::endl;;
#endif

    return nr_graph;
  }

  /**
   * @brief This function solves the shortest path problem using Dijkstra algorithm.
   * @param source the source node
   * @param target the target node
   * @return the shortest path between nodes source and target
   *
   * This function determines the shortest path from the source node to the target node.
   */
  std::vector<osmium::unsigned_object_id_type> hasDijkstraPath ( osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to )
  {

#ifdef DEBUG
    auto start = std::chrono::high_resolution_clock::now();
#endif

    std::vector<NRGVertex> parents ( boost::num_vertices ( *nr_graph ) );
    std::vector<int> distances ( boost::num_vertices ( *nr_graph ) );

    VertexIndexMap vertexIndexMap = boost::get ( boost::vertex_index, *nr_graph );

    PredecessorMap predecessorMap ( &parents[0], vertexIndexMap );
    DistanceMap distanceMap ( &distances[0], vertexIndexMap );

    boost::dijkstra_shortest_paths ( *nr_graph, nr2v[from],
                                     boost::distance_map ( distanceMap ).predecessor_map ( predecessorMap ) );

    VertexNameMap vertexNameMap = boost::get ( boost::vertex_name, *nr_graph );

    std::vector<osmium::unsigned_object_id_type> path;

    NRGVertex tov = nr2v[to];
    NRGVertex fromv = predecessorMap[tov];

#ifdef DEBUG
    int dist {0};
#endif

    while ( fromv != tov )
      {

        NRGEdge edge = boost::edge ( fromv, tov, *nr_graph ).first;

#ifdef DEBUG
        std::cout << vertexNameMap[boost::source ( edge, *nr_graph )]
                  << " -> "
                  << vertexNameMap[boost::target ( edge, *nr_graph )] << std::endl;
        dist += distanceMap[fromv];
#endif

        path.push_back ( vertexNameMap[boost::target ( edge, *nr_graph )] );

        tov = fromv;
        fromv = predecessorMap[tov];
      }
    path.push_back ( from );

    std::reverse ( path.begin(), path.end() );

#ifdef DEBUG
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds> (
                std::chrono::high_resolution_clock::now() - start ).count()
              << " ms " << dist << " meters" << std::endl;

    std::copy ( path.begin(), path.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " " ) );
#endif

    return path;
  }


  /**
   * @brief This function solves the shortest path problem using Bellman-Ford algorithm.
   * @param source the source node
   * @param target the target node
   * @return the shortest path between nodes source and target
   *
   * This function determines the shortest path from the source node to the target node.
   */
  std::vector<osmium::unsigned_object_id_type> hasBellmanFordPath ( osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to )
  {

#ifdef DEBUG
    auto start = std::chrono::high_resolution_clock::now();
#endif

    std::vector<NRGVertex> parents ( boost::num_vertices ( *nr_graph ) );
    for ( int i = 0; i < boost::num_vertices ( *nr_graph ); ++i )
      parents[i] = i;

    std::vector<int> distances ( boost::num_vertices ( *nr_graph ), ( std::numeric_limits<int>::max ) () );
    distances[ nr2v[from] ] = 0;

    VertexIndexMap vertexIndexMap = boost::get ( boost::vertex_index, *nr_graph );
    EdgeWeightMap weightMap = boost::get ( boost::edge_weight_t(), *nr_graph );

    PredecessorMap predecessorMap ( &parents[0], vertexIndexMap );
    DistanceMap distanceMap ( &distances[0], vertexIndexMap );

    boost::bellman_ford_shortest_paths ( *nr_graph, boost::num_vertices ( *nr_graph ),
                                         boost::weight_map ( weightMap ).
                                         distance_map ( distanceMap ).predecessor_map ( predecessorMap ) );

    VertexNameMap vertexNameMap = boost::get ( boost::vertex_name, *nr_graph );

    std::vector<osmium::unsigned_object_id_type> path;

    NRGVertex tov = nr2v[to];
    NRGVertex fromv = predecessorMap[tov];

#ifdef DEBUG
    int dist {0};
#endif

    while ( fromv != tov )
      {

        NRGEdge edge = boost::edge ( fromv, tov, *nr_graph ).first;

#ifdef DEBUG
        std::cout << vertexNameMap[boost::source ( edge, *nr_graph )]
                  << " -> "
                  << vertexNameMap[boost::target ( edge, *nr_graph )] << std::endl;
        dist += distanceMap[fromv];
#endif

        path.push_back ( vertexNameMap[boost::target ( edge, *nr_graph )] );

        tov = fromv;
        fromv = predecessorMap[tov];
      }
    path.push_back ( from );

    std::reverse ( path.begin(), path.end() );

#ifdef DEBUG
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds> (
                std::chrono::high_resolution_clock::now() - start ).count()
              << " ms " << dist << " meters" << std::endl;

    std::copy ( path.begin(), path.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " " ) );
#endif

    return path;
  }


protected:

  NodeRefGraph* nr_graph;
  std::string m_teamname;

private:

  /**
   * Helper structure to create the BGL graph.
   */
  std::map<osmium::unsigned_object_id_type, NRGVertex> nr2v;

  /**
   * To test the shortest path finding.
   */
  void foo ( void )
  {

    std::cout << std::endl;
    std::vector<osmium::unsigned_object_id_type> pathD = hasDijkstraPath ( 2969934868, 1348670117 );
    std::cout << std::endl;
    std::copy ( pathD.begin(), pathD.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, "  -D-> " ) );

    std::vector<osmium::unsigned_object_id_type> pathBF = hasBellmanFordPath ( 2969934868, 1348670117 );
    std::cout << std::endl;
    std::copy ( pathBF.begin(), pathBF.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " -BF-> " ) );

    std::cout << std::endl;
    pathD = hasDijkstraPath ( 2969934868, 1402222861 );
    std::copy ( pathD.begin(), pathD.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, "  -D-> " ) );

    std::cout << std::endl;
    pathBF = hasBellmanFordPath ( 2969934868, 1402222861 );
    std::cout << std::endl;
    std::copy ( pathBF.begin(), pathBF.end(),
                std::ostream_iterator<osmium::unsigned_object_id_type> ( std::cout, " -BF-> " ) );

  }

  int init ( boost::asio::ip::tcp::socket & socket );

  struct SmartCar
  {
    int id;
    unsigned from;
    unsigned to;
    int step;
  };

  typedef SmartCar Gangster;
  typedef int Cop;

  std::vector<Gangster> gangsters ( boost::asio::ip::tcp::socket & socket, int id, osmium::unsigned_object_id_type cop );
  std::vector<Cop> initcops ( boost::asio::ip::tcp::socket & socket );
  void pos ( boost::asio::ip::tcp::socket & socket, int id );
  void car ( boost::asio::ip::tcp::socket & socket, int id, unsigned *f, unsigned *t, unsigned* s );
  void route ( boost::asio::ip::tcp::socket & socket, int id, std::vector<osmium::unsigned_object_id_type> & );
};

}
} // justine::sampleclient::

#endif // ROBOCAR_SHMCLIENT_HPP


