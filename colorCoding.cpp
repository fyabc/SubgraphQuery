//
// Created by fanyang on 7/1/2016.
//

#include "graph.h"
#include "utils.h"

#include <fstream>
#include <queue>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <cstdlib>

using namespace std;

namespace {

size_t *all = nullptr;

struct Pair_IS_Hasher {
    inline size_t operator() (const pair<int, size_t>& p) const {
        return hash<int>()(p.first) ^ (hash<size_t>()(p.second) << 1);
    }
};

inline vector<bool> makeSet(size_t k, const initializer_list<size_t>& colors) {
    vector<bool> result(k, false);
    for (const auto& elem: colors)
        result[elem] = true;

    return result;
}

inline bool haveIntersect(const vector<bool>& lhs, const vector<bool>& rhs) {
    for (auto il = lhs.cbegin(), ir = rhs.cbegin(); il != lhs.cend(); ++il, ++ir)
        if (*il && *ir)
            return true;
    return false;
}

inline vector<bool> operator| (const vector<bool>& lhs, const vector<bool>& rhs) {
    vector<bool> result(lhs.size());
    auto il = lhs.cbegin(), ir = rhs.cbegin();
    for (auto ire = result.begin(); ire != result.end(); ++ire, ++il, ++ir)
        *ire = *il || *ir;
    return result;
}

inline vector<bool> operator& (const vector<bool>& lhs, const vector<bool>& rhs) {
    vector<bool> result(lhs.size());
    auto il = lhs.cbegin(), ir = rhs.cbegin();
    for (auto ire = result.begin(); ire != result.end(); ++ire, ++il, ++ir)
        *ire = *il && *ir;
    return result;
}

inline bool operator== (const vector<bool>& lhs, const vector<bool>& rhs) {
    for (auto il = lhs.cbegin(), ir = rhs.cbegin(); il != lhs.end(); ++il, ++ir)
        if (*il != *ir)
            return false;
    return true;
}

inline vector<bool> addColor(const vector<bool> &lhs, size_t i) {
    auto result(lhs);
    result[i] = true;

    return result;
}

// functions below are for debug.

inline size_t nnz(const vector<bool>& op) {
    return (size_t)count(op.cbegin(), op.cend(), true);
}

inline void disp(const vector<bool>& op) {
    for (auto i = 0; i < op.size(); ++i)
        if (op[i])
            cout << i << " ";
    cout << endl;
}

inline ostream& operator<< (ostream& out, const unordered_set<size_t>& s) {
    out << "{";
    for (const auto& elem: s)
        out << elem << " ";
    out << "}";
    return out;
}

} // anonymous namespace

void Graph::randomColor(std::size_t qSize) const {
    for (auto& elem : vertices) {
        elem.color = (size_t)rand() % qSize;
    }
}

mpz_class Graph::getSubgraphNumber_BF(const Graph& Q, int sampleTimes) const {
    mpz_class result(0);
    auto ver = new size_t[Q.size()];
    vector<unordered_set<size_t>> colorVertices(N, unordered_set<size_t>());

    for (auto i = 0; i < sampleTimes; ++i) {
        randomColor(Q.size());
        for (auto& elem: colorVertices)
            elem.clear();

        for (size_t j = 0; j < N; ++j)
            colorVertices[vertices[j].color].insert(j);

        testPermutations(Q, ver, colorVertices, 0, result);
    }

    delete[] ver;

    return result * power((int) Q.size(), Q.size()) / fac(Q.size()) / sampleTimes;
}

void Graph::testPermutations(const Graph& Q, std::size_t *ver,
                                  const std::vector<std::unordered_set<std::size_t>>& colorVertices,
                                  size_t depth, mpz_class& result) const {
    auto k = Q.size();

    if (depth == k) {
        auto tempVer = new size_t[k];
        copy(ver, ver + k, tempVer);
        sort(tempVer, tempVer + k);
        bool flag;
        do {
            if (contain(tempVer, Q))
                ++result;
            flag = next_permutation(tempVer, tempVer + k);
        } while (flag);
        delete[] tempVer;
        return;
    }

    for (const auto& elem: colorVertices[depth]) {
        ver[depth] = elem;
        testPermutations(Q, ver, colorVertices, depth + 1, result);
    }
}

vector<Graph::DecomposeTreeNode> Graph::treeDecompose(size_t root) const {
    auto Q = *this;
    std::vector<DecomposeTreeNode> result(2 * N - 1);

    for (size_t i = 0; i < N; ++i)
        result[0].vertices.insert(i);
    result[0].parent = -1;
    result[0].root = root;

    auto index = 0;
    for (auto i = 0; i < 2 * N - 1; ++i) {
        auto& node = result[i];

        // if node has only one vertex, it is a leaf.
        // continue.
        if (node.vertices.size() <= 1) {
            node.activeChild = node.passiveChild = -1;
            continue;
        }

        // else, choose an edge which contains node.root, remove it to split the tree.
        // active child contains node.root.
        auto src = node.root;
        auto dst = *(Q.vertices[src].adj.begin());
        Q.removeEdge(src, dst);

        node.activeChild = ++index;
        node.passiveChild = ++index;
        result[node.activeChild].parent = result[node.passiveChild].parent = i;
        result[node.activeChild].root = src;
        result[node.passiveChild].root = dst;

        result[node.activeChild].vertices = Q.getConnectComponent(src);
        result[node.passiveChild].vertices = Q.getConnectComponent(dst);
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_Tree(const Graph& Q, int sampleTimes) const {
    mpz_class result(0);
    auto k = Q.size();

    if (Q.isStar())
        return getSubgraphNumber_Star(Q);

    // decompose Q into vertices, create a decompose tree.
    auto decompose = Q.treeDecompose();

    // DP array. [S, v, color_set]
    unordered_map<pair<int, size_t>, unordered_map<vector<bool>, mpz_class>, Pair_IS_Hasher> DP;

    // for all iteration
    for (auto i = 0; i < sampleTimes; ++i) {
        randomColor(Q.size());
        DP.clear();

        // for all sub-templates S
        for (int s = int(2 * k - 2); s >= 0; --s) {
//            cout << s << endl;

            // if s is a leaf node (only one vertex), just set boundary values.
            if (decompose[s].activeChild == -1) {
                for (size_t v = 0; v < N; ++v)
                    DP[make_pair(s, v)][makeSet(k, {vertices[v].color})] = 1;
                continue;
            }

            // for all vertices v in G
            for (size_t v = 0; v < N; ++v) {
                auto a = decompose[s].activeChild, p = decompose[s].passiveChild;

                // for all v's neighbour u
                for (const auto& u: vertices[v].adj) {
                    const auto& av = DP[make_pair(a, v)];
                    const auto& pu = DP[make_pair(p, u)];

                    for (const auto& avi: av) {
                        for (const auto& puj: pu) {
                            if (!haveIntersect(avi.first, puj.first)) {
                                DP[make_pair(s, v)][avi.first | puj.first] += avi.second * puj.second;
                            }
                        }
                    }
                }
            }
        }

        for (size_t v = 0; v < N; ++v) {
            for (const auto& rvi: DP[make_pair(0, v)]) {
                result += rvi.second;
            }
        }
    }

    return result * power(k, k) / fac(k) / sampleTimes;
}

mpz_class Graph::getSubgraphNumber_FullTree(const vector<size_t> &branches, int sampleTimes) const {
    mpz_class result(0);

    for (auto i = 0; i < sampleTimes; ++i) {

    }

    return result;
}


/// Query with tree width 2.

mpz_class Graph::getSubgraphNumber_2Treewidth(const Graph& Q, int sampleTimes) const {
    auto decompose = tree2Decompose();
    return getSubgraphNumber_2Treewidth_Decompose(Q, decompose, sampleTimes);
}

mpz_class Graph::getSubgraphNumber_2Treewidth_Decompose(const Graph& Q, vector<DecomposeTree2Node>& decompose,
                                                        int sampleTimes) const {
    mpz_class result(0);

    auto k = Q.size();

    for (auto i = 0; i < sampleTimes; ++i) {
        randomColor(k);
        for (auto& elem: decompose)
            elem.count.clear();

        // for all sub-templates s
        for (int s = int(decompose.size()) - 1; s >= 0; --s) {
            auto& node = decompose[s];

            // parse the node (leaf, 1-boundary cycle or 2-boundary cycle).
            calculateNode_PS(Q, node, decompose);
        }

        for (const auto& countV: decompose[0].count) {
            for (const auto& countVC: countV.second) {
                result += countVC.second;
//                cout << countVC.second << endl;
            }
        }
    }

    return result * power(k, k) / fac(k) / sampleTimes;
}

std::vector<Graph::DecomposeTree2Node> Graph::tree2Decompose() const {
    auto Q = *this;
    vector<DecomposeTree2Node> result;

    return result;
}

void Graph::contractLeaf(std::size_t bNode, std::vector<Graph::DecomposeTree2Node>& decompose) {

}

void Graph::contractCycle1(std::size_t bNode) {

}

void Graph::contractCycle2(std::size_t bNode1, std::size_t bNode2) {

}

void Graph::calculateNode_PS(const Graph& Q, DecomposeTree2Node& node, const vector<DecomposeTree2Node>& decompose) const {
    auto k = Q.size();

    if (node.vertices.size() == 2) {
        // leaf node
        // leaf node cannot have annotated edge.
    }
    else if (node.bNodeIndexes.size() == 1) {
        // 1 boundary cycle node
    }
    else {
        // 2 boundary cycle node
        auto L = node.vertices.size();
        auto p = node.bNodeIndexes[0], q = node.bNodeIndexes[1];

        // Counts of P+ and P- (p, q are boundary nodes, p < q)
        // P+ is path p, p+1, ..., q
        // P- is path p, p-1, ..., q
        // positiveCounts[i] = Count[*,*| P+(p, p+i+1 % L)]
        // negativeCounts[i] = Count[*,*| P-(p, p-i-1 % L)]
        vector<decltype(node.count)> pathCounts(max(q - p, p + L - q));

        // for each edge (u, v) in G
        for (size_t u = 0; u < N; ++u) {
            for (const auto& v: getAdj(u)) {
//                if (u > v)
//                    continue;
                if (getColor(u) == getColor(v))
                    continue;
                pathCounts[0][{u, v}][makeSet(k, {getColor(u), getColor(v)})] = 1;
            }
        }

        // for all edges in path P+/P-.
        // the calculate is same for P+/P-, so calculate the max one, then get another.

        // Most time cost here.
        for (size_t j = 1; j < pathCounts.size(); ++j) {
            // for each (u, v, color_set) with Count[u, v, c] != 0
            for (const auto& countUV: pathCounts[j - 1]) {
                auto u = countUV.first[0], v = countUV.first[1];
                for (const auto& countUVC: countUV.second) {
                    const auto& colorSet = countUVC.first;

                    // for each edge (v, w) in G with color(w) not in c
                    for (auto w: getAdj(v)) {
                        if (!colorSet[getColor(w)]) {
//                            cout << nnz(colorSet) << "#" << j << endl;
//                            cout << "{" << u << " " << getColor(u) << "|" << v << " " << getColor(v) << "}" << endl;
                            pathCounts[j][{u, w}][addColor(colorSet, getColor(w))] += pathCounts[j - 1][countUV.first][colorSet];
                        }
                    }
                }
            }
        }

        // computing projection table for the cycle.
        const auto& positiveCount = pathCounts[q - p - 1];
        auto& negativeCount = pathCounts[p + L - q - 1];

        // for each (u, v, c1) with Count[u, v, c1|P+] != 0
        // for each (u, v, c2) with Count[u, v, c2|P-] != 0
        for (const auto& countPUV: positiveCount) {
            auto u = countPUV.first[0], v = countPUV.first[1];
            for (const auto& countPUVC1: countPUV.second) {
                const auto& c1 = countPUVC1.first;
                for (const auto& countNUVC2: negativeCount[countPUV.first]) {
                    const auto& c2 = countNUVC2.first;
                    if ((c1 & c2) == makeSet(k, {getColor(u), getColor(v)})) {
//                        cout << "["; disp(c1); disp(c2); cout << "]";
//                        cout << "(" << u << " " << getColor(u) << "|" << v << " " << getColor(v) << ")" << endl;
                        node.count[countPUV.first][c1 | c2] += countPUVC1.second * countNUVC2.second;
                    }
                }
            }
        }
    }
}

void Graph::pathSplittingAlgorithm(const DecomposeTree2Node &node) const {

}

void Graph::degreeBasedAlgorithm(const DecomposeTree2Node &node) const {

}
