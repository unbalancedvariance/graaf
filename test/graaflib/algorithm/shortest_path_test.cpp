#include <fmt/core.h>
#include <graaflib/algorithm/shortest_path.h>
#include <graaflib/graph.h>
#include <graaflib/types.h>
#include <gtest/gtest.h>

namespace graaf::algorithm {

namespace {
template <typename T>
struct TypedShortestPathTest : public testing::Test {
  using graph_t = T;
};

using graph_types =
    testing::Types<directed_graph<int, int>, undirected_graph<int, int>>;
TYPED_TEST_SUITE(TypedShortestPathTest, graph_types);
}  // namespace

TYPED_TEST(TypedShortestPathTest, BfsMinimalShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  graph_t graph{};

  const auto vertex_1{graph.add_vertex(10)};

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_1);

  // THEN
  const graph_path<int> expected_path{{vertex_1}, 0};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(TypedShortestPathTest, BfsNoAvailablePath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  graph_t graph{};

  const auto vertex_1{graph.add_vertex(10)};
  const auto vertex_2{graph.add_vertex(20)};

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_2);

  // THEN
  ASSERT_FALSE(path.has_value());
}

TYPED_TEST(TypedShortestPathTest, BfsSimpleShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  graph_t graph{};

  const auto vertex_1{graph.add_vertex(10)};
  const auto vertex_2{graph.add_vertex(20)};

  // We add an edge from the vertex where we start the traversal
  // so it does not matter whether this is a directed or undirected graph
  graph.add_edge(vertex_1, vertex_2, 100);

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_2);

  // THEN
  const graph_path<int> expected_path{{vertex_1, vertex_2}, 1};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(TypedShortestPathTest, BfsMoreComplexShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  graph_t graph{};

  const auto vertex_1{graph.add_vertex(10)};
  const auto vertex_2{graph.add_vertex(20)};
  const auto vertex_3{graph.add_vertex(30)};
  const auto vertex_4{graph.add_vertex(40)};
  const auto vertex_5{graph.add_vertex(50)};

  // All edges are in the search direction, so the graph specialization does not
  // matter
  graph.add_edge(vertex_1, vertex_2, 100);
  graph.add_edge(vertex_2, vertex_3, 200);
  graph.add_edge(vertex_1, vertex_3, 300);
  graph.add_edge(vertex_3, vertex_4, 400);
  graph.add_edge(vertex_4, vertex_5, 500);
  graph.add_edge(vertex_3, vertex_5, 600);

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_5);

  // THEN
  const graph_path<int> expected_path{{vertex_1, vertex_3, vertex_5}, 2};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(TypedShortestPathTest, BfsCyclicShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  graph_t graph{};

  const auto vertex_1{graph.add_vertex(10)};
  const auto vertex_2{graph.add_vertex(20)};
  const auto vertex_3{graph.add_vertex(30)};
  const auto vertex_4{graph.add_vertex(40)};
  const auto vertex_5{graph.add_vertex(50)};

  // All edges are in the search direction, so the graph specialization does not
  // matter
  graph.add_edge(vertex_1, vertex_2, 100);
  graph.add_edge(vertex_2, vertex_3, 200);
  graph.add_edge(vertex_3, vertex_4, 300);
  graph.add_edge(vertex_4, vertex_2, 300);
  graph.add_edge(vertex_3, vertex_5, 400);

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_5);

  // THEN
  const graph_path<int> expected_path{{vertex_1, vertex_2, vertex_3, vertex_5},
                                      3};
  ASSERT_EQ(path, expected_path);
}

TEST(ShortestPathTest, BfsDirectedrWrongDirectionShortestPath) {
  // GIVEN
  directed_graph<int, int> graph{};

  const auto vertex_1{graph.add_vertex(10)};
  const auto vertex_2{graph.add_vertex(20)};
  const auto vertex_3{graph.add_vertex(30)};
  const auto vertex_4{graph.add_vertex(40)};
  const auto vertex_5{graph.add_vertex(50)};

  // Edge between 2 and 3 is inverted, so path needs to take detour via 4
  graph.add_edge(vertex_1, vertex_2, 100);
  graph.add_edge(vertex_3, vertex_2, 200);
  graph.add_edge(vertex_3, vertex_5, 300);
  graph.add_edge(vertex_2, vertex_4, 400);
  graph.add_edge(vertex_4, vertex_3, 500);

  // WHEN
  const auto path = bfs_shortest_path(graph, vertex_1, vertex_5);

  // THEN
  const graph_path<int> expected_path{
      {vertex_1, vertex_2, vertex_4, vertex_3, vertex_5}, 4};
  ASSERT_EQ(path, expected_path);
}

template <typename T>
class my_weighted_edge : public weighted_edge<T> {
 public:
  explicit my_weighted_edge(T weight) : weight_{weight} {}

  [[nodiscard]] T get_weight() const noexcept override { return weight_; }

 private:
  T weight_{};
};

template <typename T>
struct DijkstraShortestPathTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};

using weighted_graph_types = testing::Types<

    /**
     * Primitive edge type directed graph
     */
    std::pair<directed_graph<int, int>, int>,
    std::pair<directed_graph<int, unsigned long>, unsigned long>,
    std::pair<directed_graph<int, float>, float>,
    std::pair<directed_graph<int, long double>, long double>,

    /**
     * Non primitive weighted edge type directed graph
     */

    std::pair<directed_graph<int, my_weighted_edge<int>>,
              my_weighted_edge<int>>,
    std::pair<directed_graph<int, my_weighted_edge<unsigned long>>,
              my_weighted_edge<unsigned long>>,
    std::pair<directed_graph<int, my_weighted_edge<float>>,
              my_weighted_edge<float>>,
    std::pair<directed_graph<int, my_weighted_edge<long double>>,
              my_weighted_edge<long double>>,

    /**
     * Primitive edge type undirected graph
     */
    std::pair<undirected_graph<int, int>, int>,
    std::pair<undirected_graph<int, unsigned long>, unsigned long>,
    std::pair<undirected_graph<int, float>, float>,
    std::pair<undirected_graph<int, long double>, long double>,

    /**
     * Non primitive weighted edge type undirected graph
     */
    std::pair<undirected_graph<int, my_weighted_edge<int>>,
              my_weighted_edge<int>>,
    std::pair<undirected_graph<int, my_weighted_edge<unsigned long>>,
              my_weighted_edge<unsigned long>>,
    std::pair<undirected_graph<int, my_weighted_edge<float>>,
              my_weighted_edge<float>>,
    std::pair<undirected_graph<int, my_weighted_edge<long double>>,
              my_weighted_edge<long double>>>;

TYPED_TEST_SUITE(DijkstraShortestPathTest, weighted_graph_types);

TYPED_TEST(DijkstraShortestPathTest, DijkstraMinimalShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};

  // WHEN;
  const auto path = dijkstra_shortest_path(graph, vertex_id_1, vertex_id_1);

  // THEN
  const graph_path<weight_t> expected_path{{vertex_id_1}, 0};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraNoAvailablePath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};

  // WHEN;
  const auto path = dijkstra_shortest_path(graph, vertex_id_1, vertex_id_2);

  // THEN
  ASSERT_FALSE(path.has_value());
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraSimpleShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(3)});

  // WHEN
  const auto path = dijkstra_shortest_path(graph, vertex_id_1, vertex_id_2);

  // THEN
  const graph_path<weight_t> expected_path{{vertex_id_1, vertex_id_2}, 3};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraMoreComplexShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};
  const auto vertex_id_5{graph.add_vertex(50)};

  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(3)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(4)});
  graph.add_edge(vertex_id_4, vertex_id_5, edge_t{static_cast<weight_t>(5)});
  graph.add_edge(vertex_id_3, vertex_id_5, edge_t{static_cast<weight_t>(6)});

  // WHEN
  const auto path = dijkstra_shortest_path(graph, vertex_id_1, vertex_id_5);

  // THEN
  const graph_path<weight_t> expected_path{
      {vertex_id_1, vertex_id_3, vertex_id_5}, 9};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraCyclicShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};
  const auto vertex_id_5{graph.add_vertex(50)};

  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(3)});
  graph.add_edge(vertex_id_4, vertex_id_2, edge_t{static_cast<weight_t>(4)});
  graph.add_edge(vertex_id_3, vertex_id_5, edge_t{static_cast<weight_t>(5)});

  // WHEN
  const auto path = dijkstra_shortest_path(graph, vertex_id_1, vertex_id_5);

  // THEN
  const graph_path<weight_t> expected_path{
      {vertex_id_1, vertex_id_2, vertex_id_3, vertex_id_5}, 8};
  ASSERT_EQ(path, expected_path);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraMinimalShortestPathTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};

  // WHEN;
  const auto path_map = dijkstra_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  ASSERT_EQ(path_map, expected_path_map);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraSimpleShortestPathTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(3)});

  // WHEN
  const auto path_map = dijkstra_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  const graph_path<weight_t> path2{{vertex_id_1, vertex_id_2}, 3};

  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  expected_path_map[vertex_id_2] = path2;
  ASSERT_EQ(path_map, expected_path_map);
}

TYPED_TEST(DijkstraShortestPathTest, DijkstraMoreComplexShortestPathTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};
  const auto vertex_id_5{graph.add_vertex(50)};

  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(3)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(4)});
  graph.add_edge(vertex_id_4, vertex_id_5, edge_t{static_cast<weight_t>(5)});
  graph.add_edge(vertex_id_3, vertex_id_5, edge_t{static_cast<weight_t>(6)});

  // WHEN
  const auto path_map = dijkstra_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  const graph_path<weight_t> path2{{vertex_id_1, vertex_id_2}, 1};
  const graph_path<weight_t> path3{{vertex_id_1, vertex_id_2, vertex_id_3}, 2};
  const graph_path<weight_t> path4{
      {vertex_id_1, vertex_id_2, vertex_id_3, vertex_id_4}, 6};
  const graph_path<weight_t> path5{
      {vertex_id_1, vertex_id_2, vertex_id_3, vertex_id_5}, 8};

  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  expected_path_map[vertex_id_2] = path2;
  expected_path_map[vertex_id_3] = path3;
  expected_path_map[vertex_id_4] = path4;
  expected_path_map[vertex_id_5] = path5;
  ASSERT_EQ(path_map, expected_path_map);
}

template <typename T>
struct DijkstraShortestPathSignedTypesTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};

using weighted_graph_signed_types = testing::Types<

    /**
     * Primitive edge type directed graph
     */
    std::pair<directed_graph<int, int>, int>,
    std::pair<directed_graph<int, float>, float>,
    std::pair<directed_graph<int, long double>, long double>,

    /**
     * Non primitive weighted edge type directed graph
     */

    std::pair<directed_graph<int, my_weighted_edge<int>>,
              my_weighted_edge<int>>,
    std::pair<directed_graph<int, my_weighted_edge<float>>,
              my_weighted_edge<float>>,
    std::pair<directed_graph<int, my_weighted_edge<long double>>,
              my_weighted_edge<long double>>,

    /**
     * Primitive edge type undirected graph
     */
    std::pair<undirected_graph<int, int>, int>,
    std::pair<undirected_graph<int, float>, float>,
    std::pair<undirected_graph<int, long double>, long double>,

    /**
     * Non primitive weighted edge type undirected graph
     */
    std::pair<undirected_graph<int, my_weighted_edge<int>>,
              my_weighted_edge<int>>,
    std::pair<undirected_graph<int, my_weighted_edge<float>>,
              my_weighted_edge<float>>,
    std::pair<undirected_graph<int, my_weighted_edge<long double>>,
              my_weighted_edge<long double>>>;

TYPED_TEST_SUITE(DijkstraShortestPathSignedTypesTest,
                 weighted_graph_signed_types);

TYPED_TEST(DijkstraShortestPathSignedTypesTest, DijkstraNegativeWeight) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(-1)});

  //  THEN
  ASSERT_THROW(
      {
        try {
          // Call the get_edge function for non-existing vertices
          [[maybe_unused]] const auto path{
              dijkstra_shortest_path(graph, vertex_id_1, vertex_id_2)};
          // If the above line doesn't throw an exception, fail the test
          FAIL()
              << "Expected std::invalid_argument exception, but no exception "
                 "was thrown.";
        } catch (const std::invalid_argument &ex) {
          // Verify that the exception message contains the expected error
          // message
          EXPECT_EQ(
              ex.what(),
              fmt::format(
                  "Negative edge weight [{}] between vertices [{}] -> [{}].",
                  -1, vertex_id_1, vertex_id_2));
          throw;
        }
      },
      std::invalid_argument);
}

TYPED_TEST(DijkstraShortestPathSignedTypesTest, DijkstraNegativeWeightTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(-1)});

  //  THEN
  ASSERT_THROW(
      {
        try {
          // Call the get_edge function for non-existing vertices
          [[maybe_unused]] const auto path{
              dijkstra_shortest_paths(graph, vertex_id_1)};
          // If the above line doesn't throw an exception, fail the test
          FAIL()
              << "Expected std::invalid_argument exception, but no exception "
                 "was thrown.";
        } catch (const std::invalid_argument &ex) {
          // Verify that the exception message contains the expected error
          // message
          EXPECT_EQ(
              ex.what(),
              fmt::format(
                  "Negative edge weight [{}] between vertices [{}] -> [{}].",
                  -1, vertex_id_1, vertex_id_2));
          throw;
        }
      },
      std::invalid_argument);
}

template <typename T>
struct BellmanFordShortestPathsTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};

TYPED_TEST_SUITE(BellmanFordShortestPathsTest, weighted_graph_types);

TYPED_TEST(BellmanFordShortestPathsTest, BellmanFordMinimalShortestPathsTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};

  // WHEN;
  const auto path_map = bellman_ford_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  ASSERT_EQ(path_map, expected_path_map);
}

TYPED_TEST(BellmanFordShortestPathsTest, BellmanFordShortestPathsTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(3)});

  // WHEN
  const auto path_map = bellman_ford_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  const graph_path<weight_t> path2{{vertex_id_1, vertex_id_2}, 3};

  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  expected_path_map[vertex_id_2] = path2;
  ASSERT_EQ(path_map, expected_path_map);
}

TYPED_TEST(BellmanFordShortestPathsTest,
           BellmanFordMoreComplexShortestPathTree) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};
  const auto vertex_id_5{graph.add_vertex(50)};

  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(3)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(4)});
  graph.add_edge(vertex_id_4, vertex_id_5, edge_t{static_cast<weight_t>(5)});
  graph.add_edge(vertex_id_3, vertex_id_5, edge_t{static_cast<weight_t>(6)});

  // WHEN
  const auto path_map = bellman_ford_shortest_paths(graph, vertex_id_1);

  // THEN
  const graph_path<weight_t> path1{{vertex_id_1}, 0};
  const graph_path<weight_t> path2{{vertex_id_1, vertex_id_2}, 1};
  const graph_path<weight_t> path3{{vertex_id_1, vertex_id_2, vertex_id_3}, 2};
  const graph_path<weight_t> path4{
      {vertex_id_1, vertex_id_2, vertex_id_3, vertex_id_4}, 6};
  const graph_path<weight_t> path5{
      {vertex_id_1, vertex_id_2, vertex_id_3, vertex_id_5}, 8};

  std::unordered_map<vertex_id_t, graph_path<weight_t>> expected_path_map;
  expected_path_map[vertex_id_1] = path1;
  expected_path_map[vertex_id_2] = path2;
  expected_path_map[vertex_id_3] = path3;
  expected_path_map[vertex_id_4] = path4;
  expected_path_map[vertex_id_5] = path5;
  ASSERT_EQ(path_map, expected_path_map);
}
template <typename T>
struct BellmanFordShortestPathsSignedTypesTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};
using undirected_weighted_graph_signed_types = testing::Types<
    /**
     * Primitive edge type directed graph
     */
    std::pair<directed_graph<int, int>, int>,
    std::pair<directed_graph<int, float>, float>,
    std::pair<directed_graph<int, long double>, long double>,

    /**
     * Non primitive weighted edge type directed graph
     */
    std::pair<directed_graph<int, my_weighted_edge<int>>,
              my_weighted_edge<int>>,
    std::pair<directed_graph<int, my_weighted_edge<float>>,
              my_weighted_edge<float>>,
    std::pair<directed_graph<int, my_weighted_edge<long double>>,
              my_weighted_edge<long double>>>;

TYPED_TEST_SUITE(BellmanFordShortestPathsSignedTypesTest,
                 undirected_weighted_graph_signed_types);

TYPED_TEST(BellmanFordShortestPathsSignedTypesTest,
           BellmanFordNegativecycleTest) {
  // Bellman Ford can handle negative edge weights only in directed graphs.
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};
  // Adding vertices
  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};

  // Adding Edges

  // Negative cycle exists between the vertices 2,3 and 4.
  graph.add_edge(vertex_id_2, vertex_id_1, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(-6)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(-1)});
  graph.add_edge(vertex_id_4, vertex_id_2, edge_t{static_cast<weight_t>(-2)});

  ASSERT_THROW(
      {
        try {
          [[maybe_unused]] const auto path{
              bellman_ford_shortest_paths(graph, vertex_id_3)};
          // If the above line doesn't throw an exception, fail the test
          FAIL() << "Expected std::invalid_argument exception, but no "
                    "exception was thrown";
        } catch (const std::invalid_argument &ex) {
          // Verify that the exception message contains the expected err  or
          // message.
          EXPECT_STREQ("Negative cycle detected in the graph.", ex.what());
          throw;
        }
      },
      std::invalid_argument);
}
template <typename T>
struct AStarShortestPathTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};

TYPED_TEST_SUITE(AStarShortestPathTest, weighted_graph_types);

// Graph with only one vertex.
TYPED_TEST(AStarShortestPathTest, AStarMinimalShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};

  // WHEN
  // The weight_t type annotation and static_cast are needed to avoid compiler
  // warnings.
  const auto heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };
  const auto path = a_star_search(graph, vertex_id_1, vertex_id_1, heuristic);

  // THEN
  const graph_path<weight_t> expected_path{{vertex_id_1}, 0};
  ASSERT_EQ(path, expected_path);
}

// Find the shortest path between the only two vertices in a graph.
TYPED_TEST(AStarShortestPathTest, AStarSimpleShortestPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});

  const auto heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };

  // WHEN
  const auto path = a_star_search(graph, vertex_id_1, vertex_id_2, heuristic);

  // THEN
  const graph_path<weight_t> expected_path{{vertex_id_1, vertex_id_2}, 1};
  ASSERT_EQ(path, expected_path);
}

// Graph where there's no path between the start and target vertices.
TYPED_TEST(AStarShortestPathTest, NoPathExistence) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  // Define start and target vertices
  vertex_id_t start_vertex = 0;
  vertex_id_t target_vertex = 5;

  // Define a heuristic function that always returns 0
  auto zero_heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };

  // WHEN
  auto result =
      a_star_search(graph, start_vertex, target_vertex, zero_heuristic);

  // THEN
  // Check that the result is an empty optional
  ASSERT_FALSE(result.has_value());
}

// Find the shortest path between multiple possible paths in a graph.
TYPED_TEST(AStarShortestPathTest, MultiplePathsTest) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(2)});

  // WHEN
  const auto heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };
  const auto path = a_star_search(graph, vertex_id_1, vertex_id_3, heuristic);

  // THEN
  const graph_path<weight_t> expected_path{{vertex_id_1, vertex_id_3}, 2};
  ASSERT_EQ(path, expected_path);
}

// Suboptimal Path Test
TYPED_TEST(AStarShortestPathTest, AStarSuboptimalPath) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(4)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(3)});

  // WHEN
  const auto heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(10);  // Overestimate the remaining cost
  };
  const auto path = a_star_search(graph, vertex_id_1, vertex_id_3, heuristic);

  // THEN
  ASSERT_TRUE(path.has_value());  // Check if optional has a value
  // Note: The path might not be the shortest, but it should still be valid
}

template <typename T>
struct AStarShortestPathSignedTypesTest : public testing::Test {
  using graph_t = typename T::first_type;
  using edge_t = typename T::second_type;
};

TYPED_TEST_SUITE(AStarShortestPathSignedTypesTest, weighted_graph_signed_types);

// Negative Weight Test
TYPED_TEST(AStarShortestPathSignedTypesTest, AStarNegativeWeight) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  graph.add_edge(vertex_id_1, vertex_id_2,
                 edge_t{static_cast<weight_t>(-1)});  // Negative weight edge

  const auto heuristic = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };

  // THEN
  // Taken from DijkstraNegativeWeight
  ASSERT_THROW(
      {
        try {
          // Call the get_edge function for non-existing vertices
          [[maybe_unused]] const auto path{
              a_star_search(graph, vertex_id_1, vertex_id_2, heuristic)};
          // If the above line doesn't throw an exception, fail the test
          FAIL()
              << "Expected std::invalid_argument exception, but no exception "
                 "was thrown.";
        } catch (const std::invalid_argument &ex) {
          // Verify that the exception message contains the expected error
          // message
          EXPECT_EQ(
              ex.what(),
              fmt::format(
                  "Negative edge weight [{}] between vertices [{}] -> [{}].",
                  -1, vertex_id_1, vertex_id_2));
          throw;
        }
      },
      std::invalid_argument);
}

// Heuristic Impact Test
TYPED_TEST(AStarShortestPathTest, AStarHeuristicImpact) {
  // GIVEN
  using graph_t = typename TestFixture::graph_t;
  using edge_t = typename TestFixture::edge_t;
  using weight_t = decltype(get_weight(std::declval<edge_t>()));

  graph_t graph{};

  const auto vertex_id_1{graph.add_vertex(10)};
  const auto vertex_id_2{graph.add_vertex(20)};
  const auto vertex_id_3{graph.add_vertex(30)};
  const auto vertex_id_4{graph.add_vertex(40)};
  graph.add_edge(vertex_id_1, vertex_id_2, edge_t{static_cast<weight_t>(1)});
  graph.add_edge(vertex_id_2, vertex_id_3, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_1, vertex_id_3, edge_t{static_cast<weight_t>(3)});
  graph.add_edge(vertex_id_1, vertex_id_4, edge_t{static_cast<weight_t>(2)});
  graph.add_edge(vertex_id_3, vertex_id_4, edge_t{static_cast<weight_t>(1)});

  const auto start_vertex = vertex_id_1;
  const auto target_vertex = vertex_id_3;

  // Define two different heuristic functions
  const auto heuristic1 = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(0);
  };  // Underestimating heuristic
  const auto heuristic2 = [](vertex_id_t vertex) -> weight_t {
    return static_cast<weight_t>(10);
  };  // Overestimating heuristic

  // WHEN
  const auto path_with_underestimating_heuristic =
      a_star_search(graph, start_vertex, target_vertex, heuristic1);
  const auto path_with_overestimating_heuristic =
      a_star_search(graph, start_vertex, target_vertex, heuristic2);

  // THEN
  // Verify that the path with the underestimating heuristic is shorter
  ASSERT_TRUE(path_with_underestimating_heuristic.has_value());
  ASSERT_TRUE(path_with_overestimating_heuristic.has_value());
  ASSERT_LT(path_with_underestimating_heuristic->total_weight,
            path_with_overestimating_heuristic->total_weight);
}

}  // namespace graaf::algorithm