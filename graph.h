//
// Created by fanyang on 16-6-24.
//

#ifndef SUBGRAPHQUERY_GRAPH_H
#define SUBGRAPHQUERY_GRAPH_H

#include <gmpxx.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>

/**
 *  This is a class of an undirected graph.
 *  Nodes in this graph are 0 ~ n-1, n is the number of nodes.
 */
class Graph {
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

    struct DecomposeTreeNode {
        std::unordered_set<std::size_t> vertices;
        int parent, activeChild, passiveChild;
        std::size_t root;
    };

public:
    explicit Graph(std::size_t N) :
            N(N), vertices(std::vector<Vertex>(N)) {}

    ~Graph() = default;
    Graph(const Graph&) = default;
    Graph(Graph&&) = default;

    Graph& operator=(const Graph&) = default;
    Graph& operator=(Graph&&) = default;

    // some factory functions.
    static std::unique_ptr<Graph> fromFile(const std::string& fileName);
    static std::unique_ptr<Graph> fromTypeString(const std::string& typeString);
    static std::unique_ptr<Graph> createStar(std::size_t n);
    static std::unique_ptr<Graph> createCycle(std::size_t n);
    static std::unique_ptr<Graph> createTree(std::size_t depth, std::size_t width);
    static std::unique_ptr<Graph> createTreeH(std::initializer_list<std::size_t> widths);

    std::size_t size() const { return N; }
    std::size_t degree(std::size_t i) const { return vertices[i].degree; }
    std::size_t edgeNum() const;
    const std::unordered_set<std::size_t>& getAdj(std::size_t i) const { return vertices[i].adj; }
    const std::vector<Vertex>& getVertices() const { return vertices; }
    
    void addEdge(std::size_t src, std::size_t dst);
    void removeEdge(std::size_t src, std::size_t dst);

    /// Show adjacent matrix.
    void showAdj(std::ostream& out = std::cout) const;

    /// Get the degree distribution of the graph.
    std::vector<std::unordered_set<std::size_t>> getDegreeDistribution() const;

    /// Get the connect component which contains vertex start.
    std::unordered_set<std::size_t> getConnectComponent(std::size_t start) const;

    /// Do subgraph test for sampleTimes.
    int testSubgraph(const Graph& Q, int sampleTimes=100000) const;

    /// Brute Force subgraph counting using color coding.
    /// (This method is used for checking results of other methods)
    mpz_class getSubgraphNumber_BF(const Graph& Q, int sampleTimes = 10000) const;

    /// Tree query using paper [30].
    /// Q must be a tree. The root of Q will be set to 0 by default.
    mpz_class getSubgraphNumber_Tree(const Graph& Q, int sampleTimes = 1000) const;

    /// Get the tree decompose of graph.
    /// [NOTE]: the graph must be a tree, the default root is 0.
    std::vector<DecomposeTreeNode> treeDecompose(std::size_t root = 0) const;

    /// (Large) Tree query with hard or soft degree constraints.
    /// constraints[i] == -1 means vertex i doesn't have constraint.
    mpz_class getSubgraphNumber_Tree_DegreesHard(const Graph& Q, const std::vector<int>& constraints) const;
    mpz_class getSubgraphNumber_Tree_DegreesSoft(const Graph& Q, const std::vector<int>& constraints) const;

private:
    std::size_t N;
    std::vector<Vertex> vertices;

    bool contain(const std::size_t* ver, const Graph& Q) const;

    void randomColor(std::size_t qSize) const;

    void testPermutations(const Graph& Q, std::size_t *ver,
                          const std::vector<std::unordered_set<std::size_t>>& colorVertices,
                          std::size_t depth, mpz_class& result) const;
};

#endif //SUBGRAPHQUERY_QUERY_H
