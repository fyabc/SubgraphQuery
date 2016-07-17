//
// Created by fanyang on 16-6-24.
//

#ifndef SUBGRAPHQUERY_GRAPH_H
#define SUBGRAPHQUERY_GRAPH_H

#include <cstddef>
#include <gmpxx.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <unordered_map>

/**
 *  This is a class of an undirected graph.
 *  Nodes in this graph are 0 ~ n-1, n is the number of nodes.
 */
class Graph {

    /* ====================================================================== */
    /* ============================= Subclasses ============================= */
    /* ====================================================================== */

    struct Vertex {
        std::size_t degree = 0;
        std::unordered_set<std::size_t> adj = std::unordered_set<std::size_t>();
        mutable std::size_t color;

        Vertex() = default;

        void addEdge(std::size_t dst) {
            if (adj.insert(dst).second)
                ++degree;
        }

        void removeEdge(std::size_t dst) {
            if (adj.erase(dst) != 0)
                --degree;
        }
    };

    /// A decompose tree node, is a subtree of the origin graph.
    struct DecomposeTreeNode {
        std::unordered_set<std::size_t> vertices;
        int parent, activeChild, passiveChild;
        std::size_t root;
    };

public:
    /// A 2-treewidth decompose tree node, is a leaf or cycle of the origin graph.
    struct DecomposeTree2Node {
        // The vertices are in order (if it is a cycle)
        std::vector<std::size_t> vertices;
        // children of the node.
        std::vector<int> children;

        // anVer[i] == j means vertices[i] is annotated by child of index j (j == -1 means not annotated)
        std::vector<int> annotatedVertices;
        // anEd[i] == j means edge (vertices[i], vertices[i+1]) is annotated by child of index j (j == -1 means not annotated)
        std::vector<int> annotatedEdges;

        // Indexes of Boundary Nodes in vertices.
        // 1 or 2 nodes.
        std::vector<std::size_t> bNodeIndexes;

        struct Vec_S_Hasher {
            inline size_t operator() (const std::vector<size_t>& v) const {
                if (v.size() == 1)
                    return std::hash<size_t>()(v[0]);
                return std::hash<size_t>()(v[0]) ^ (std::hash<size_t>()(v[1]) << 1);
            }
        };

        // Counts of this node for all vertices in G and all color sets.
        // Used to calculate counts of parent nodes.
        std::unordered_map<std::vector<std::size_t>, std::unordered_map<std::vector<bool>, mpz_class>, Vec_S_Hasher> count;
    };

    /* ====================================================================== */
    /* ========================== Member Variables ========================== */
    /* ====================================================================== */

    std::size_t N;
    std::vector<Vertex> vertices;

public:
    explicit Graph(std::size_t N) :
            N(N), vertices(std::vector<Vertex>(N)) {}

    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph(Graph&&) = default;

    Graph& operator=(const Graph&) = default;
    Graph& operator=(Graph&&) = default;

    /* ====================================================================== */
    /* ======================== Some Factory Methods ======================== */
    /* ====================================================================== */

    static std::unique_ptr<Graph> fromFile(const std::string& fileName);
    static std::unique_ptr<Graph> fromTypeString(const std::string& typeString);
    static std::unique_ptr<Graph> createStar(std::size_t n);
    static std::unique_ptr<Graph> createCycle(std::size_t n);
    static std::unique_ptr<Graph> createComplete(std::size_t n);
    static std::unique_ptr<Graph> createTree(std::size_t depth, std::size_t width);
    static std::unique_ptr<Graph> createTreeH(const std::vector<std::size_t>& widths);
    static std::unique_ptr<Graph> createER(std::size_t n, std::size_t m);
    void toFile(const std::string& fileName) const;

    /* ====================================================================== */
    /* ============================ Basic Methods =========================== */
    /* ====================================================================== */

    std::size_t size() const { return N; }
    std::size_t degree(std::size_t i) const { return vertices[i].degree; }
    std::size_t edgeNum() const;
    std::size_t maxDegree() const;

    const std::unordered_set<std::size_t>& getAdj(std::size_t i) const { return vertices[i].adj; }
    const std::vector<Vertex>& getVertices() const { return vertices; }
    
    void addEdge(std::size_t src, std::size_t dst);
    void addEdges(const std::initializer_list<std::size_t>& srcs, const std::initializer_list<std::size_t>& dsts);
    void removeEdge(std::size_t src, std::size_t dst);
    bool haveEdge(std::size_t src, std::size_t dst) const;

    /// Test if G is a star.
    bool isStar(std::size_t root = 0) const;

    /// Show adjacent matrix.
    void showAdj(std::ostream& out = std::cout) const;
    void showGraphInfo(bool showEdgeDistrib = false, std::ostream& out = std::cout) const;

    /// Get the degree distribution of the graph.
    std::vector<std::unordered_set<std::size_t>> getDegreeDistribution() const;

    /// Get the connect component which contains vertex start.
    std::unordered_set<std::size_t> getConnectComponent(std::size_t start) const;

private:
    std::size_t getColor(std::size_t i) const { return vertices[i].color; }

    /* ====================================================================== */
    /* =========================== Simple Sampling ========================== */
    /* ====================================================================== */

private:
    bool contain(const std::size_t* ver, const Graph& Q) const;

public:
    /// Do subgraph test for sampleTimes.
    int testSubgraph(const Graph& Q, int sampleTimes = 1) const;

    /* ====================================================================== */
    /* ============================= Star Query ============================= */
    /* ====================================================================== */

    /// Star query.
    mpz_class getSubgraphNumber_Star(const Graph& Q) const;

    /// Star query with degree constraints.
    /// If constraintRoot == 0, it means no constraint of root.
    mpz_class getSubgraphNumber_Star_DegreesHard(const Graph& Q, std::size_t constraint, std::size_t constraintRoot = 0) const;
    mpz_class getSubgraphNumber_Star_DegreesSoft(const Graph& Q, std::size_t constraint, std::size_t constraintRoot = 0) const;
    mpz_class getSubgraphNumber_Star_All(std::size_t k, std::size_t constraintRoot, bool rootHard,
                                         std::size_t constraintLeaf, bool leafHard) const;

    /* ====================================================================== */
    /* ============================ Color Coding ============================ */
    /* ====================================================================== */

private:
    void randomColor(std::size_t qSize) const;

public:
    /// Brute Force subgraph counting using color coding.
    /// (This method is used for checking results of other methods)
    mpz_class getSubgraphNumber_BF(const Graph& Q, int sampleTimes = 1) const;

private:
    void testPermutations(const Graph& Q, std::size_t *ver,
                          const std::vector<std::unordered_set<std::size_t>>& colorVertices,
                          std::size_t depth, mpz_class& result) const;

    /* ====================================================================== */

public:
    /// Tree query using color coding and dynamic programming in paper [30].
    /// Q must be a tree. The root of Q will be set to 0 by default.
    mpz_class getSubgraphNumber_Tree(const Graph& Q, int sampleTimes = 1) const;

    /// Get the tree decompose of graph.
    /// [NOTE]: the graph must be a tree, the default root is 0.
    std::vector<DecomposeTreeNode> treeDecompose(std::size_t root = 0) const;

    /* ====================================================================== */

    /// Graph query using color coding in paper [Subgraph Counting].
    /// Q must be a graph which treewidth <= 2.
    mpz_class getSubgraphNumber_2Treewidth(const Graph& Q, int sampleTimes = 1) const;

    /// 2 Treewidth query with decompose given.
    mpz_class getSubgraphNumber_2Treewidth_Decompose(const Graph &Q, std::vector<DecomposeTree2Node> &decompose,
                                                     int sampleTimes = 1) const;

    /// Get the 2-treewidth decompose of graph.
    /// [NOTE]: the graph must be have treewidth <= 2.
    std::vector<DecomposeTree2Node> tree2Decompose() const;

private:
    /// Contract a leaf or cycle from the graph.
    void contractLeaf(std::size_t bNode, std::vector<Graph::DecomposeTree2Node>& decompose);
    void contractCycle1(std::size_t bNode);
    void contractCycle2(std::size_t bNode1, std::size_t bNode2);

    void calculateNode_PS_raw(const Graph &Q, DecomposeTree2Node &node,
                              const std::vector<DecomposeTree2Node> &decompose) const;
    void calculateNode_PS(const Graph &Q, DecomposeTree2Node &node,
                              const std::vector<DecomposeTree2Node> &decompose) const;
    void calculateNode_DB(const Graph& Q, DecomposeTree2Node& node, const std::vector<DecomposeTree2Node>& decompose) const;

    void nodeJoin(const DecomposeTree2Node& node, const std::vector<DecomposeTree2Node>& decompose, bool direction,
                  std::size_t p, std::size_t k, std::size_t L, std::size_t j,
                  std::vector<decltype(node.count)>& pathCounts) const;

    void edgeJoin(const DecomposeTree2Node& node, const std::vector<DecomposeTree2Node>& decompose, bool direction,
                  std::size_t p, std::size_t k, std::size_t L, std::size_t j,
                  std::vector<decltype(node.count)>& pathCounts) const;

    /* ====================================================================== */

public:
    /// Full tree query.
    /// Using color coding.
    mpz_class getSubgraphNumber_FullTree(const std::vector<std::size_t>& branches,
                                         int sampleTimes = 1) const;

    /* ====================================================================== */
    /* ======================= 2-Depth Full Tree Query ====================== */
    /* ====================================================================== */

    /// 2-depth full tree query.
    mpz_class getSubgraphNumber_2dFullTree(std::size_t b1, std::size_t b2, int sampleTimes = 1) const;

    /// 2-depth full tree query (ignore conflicts).
    mpz_class getSubgraphNumber_2dFullTree_ignore(std::size_t b1, std::size_t b2) const;

    /// 2-depth full tree query sampling.
    double testSubgraph_2dTree(std::size_t b1, std::size_t b2, int sampleTimes = 1) const;

    /* ====================================================================== */
    /* ======================== Other Special Queries ======================= */
    /* ====================================================================== */
    mpz_class getSubgraphNumber_Triangle() const;
};

#endif //SUBGRAPHQUERY_QUERY_H
