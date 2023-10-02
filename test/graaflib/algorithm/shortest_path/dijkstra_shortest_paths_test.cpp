#include <fmt/core.h>
#include <graaflib/algorithm/shortest_path/common.h>
#include <graaflib/algorithm/shortest_path/dijkstra_shortest_paths.h>
#include <gtest/gtest.h>

namespace graaf::algorithm {

namespace {

template <typename T>
class my_weighted_edge : public weighted_edge<T> {
 public:
  explicit my_weighted_edge(T weight) : weight_{weight} {}

  [[nodiscard]] T get_weight() const noexcept override { return weight_; }

 private:
  T weight_{};
};

template <typename T>
struct DijkstraShortestPathsTest : public testing::Test {
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

TYPED_TEST_SUITE(DijkstraShortestPathsTest, weighted_graph_types);

template <typename T>
struct DijkstraShortestPathsSignedTypesTest : public testing::Test {
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

TYPED_TEST_SUITE(DijkstraShortestPathsSignedTypesTest,
                 weighted_graph_signed_types);

}  // namespace

TYPED_TEST(DijkstraShortestPathsTest, DijkstraMinimalShortestPathTree) {
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

TYPED_TEST(DijkstraShortestPathsTest, DijkstraSimpleShortestPathTree) {
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

TYPED_TEST(DijkstraShortestPathsTest, DijkstraMoreComplexShortestPathTree) {
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

TYPED_TEST(DijkstraShortestPathsSignedTypesTest, DijkstraNegativeWeightTree) {
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

}  // namespace graaf::algorithm