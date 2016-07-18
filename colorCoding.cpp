//
// Created by fanyang on 7/1/2016.
//

#include "graph.h"
#include "utils.h"

#include <fstream>
#include <sstream>
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

#define higher(u, v) ((degree(u) > degree(v)) || ((degree(u) == degree(v)) && ((u) > (v))))
#define lPlus(x, y) (((x) + (y)) % L)
#define lMinus(x, y) (((x) + L - (y)) % L)

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
//            calculateNode_PS_raw(Q, node, decompose);
            calculateNode_PS(Q, node, decompose);
        }

        for (const auto& countV: decompose[0].count) {
            for (const auto& countVC: countV.second) {
//                assert(nnz(countVC.first) == k);
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

pair<unique_ptr<Graph>, vector<Graph::DecomposeTree2Node>> Graph::readQueryWithDecompose(const string& inFileName) {
    string selfFileName(__FILE__);
    {
        size_t i = selfFileName.size() - 1;
        for (; i != 0; --i) {
            if (selfFileName[i] == '/' || selfFileName[i] == '\\')
                break;
        }
        selfFileName.erase(i + 1);
    }


    ifstream inFile((selfFileName + "decompositions/" + inFileName).c_str());

    size_t k;
    inFile >> k;

    pair<unique_ptr<Graph>, vector<DecomposeTree2Node>> result(unique_ptr<Graph>(new Graph(k)), vector<DecomposeTree2Node>());

    int src, dst;
    while (inFile >> src >> dst, src != -1) {
        result.first->addEdge((size_t)src, (size_t)dst);
    }

    string line;
    stringstream sLine;
    size_t decomposeSize;
    inFile >> decomposeSize; getline(inFile, line); // eat the end of this line

    for (size_t i = 0; i < decomposeSize; ++i) {
        int temp;
        size_t tempU;

        result.second.push_back(DecomposeTree2Node());
        DecomposeTree2Node& node = result.second.back();

        // vertices
        getline(inFile, line);
        sLine.clear(); sLine.str(line);     // clear() is necessary to clear all bad bits.
        while (sLine >> tempU)
            node.vertices.push_back(tempU);

        // children
        getline(inFile, line);
        sLine.clear(); sLine.str(line);
        while (sLine >> tempU)
            node.children.push_back(tempU);

        // annotatedVertices
        getline(inFile, line);
        sLine.clear(); sLine.str(line);
        while (sLine >> temp)
            node.annotatedVertices.push_back(temp);

        // annotatedEdges
        getline(inFile, line);
        sLine.clear(); sLine.str(line);
        while (sLine >> temp)
            node.annotatedEdges.push_back(temp);

        // bNodeIndexes
        getline(inFile, line);
        sLine.clear(); sLine.str(line);
        while (sLine >> tempU)
            node.bNodeIndexes.push_back(tempU);
    }

    inFile.close();

    return result;
}

void Graph::contractLeaf(std::size_t bNode, std::vector<Graph::DecomposeTree2Node>& decompose) {

}

void Graph::contractCycle1(std::size_t bNode) {

}

void Graph::contractCycle2(std::size_t bNode1, std::size_t bNode2) {

}

void Graph::nodeJoin(const DecomposeTree2Node& node, const vector<DecomposeTree2Node>& decompose, bool direction,
                     size_t p, size_t k, size_t L, size_t j, vector<decltype(node.count)>& pathCounts) const {
    auto aj = direction ? lPlus(p, j) : lMinus(p, j);

    if (node.annotatedVertices[aj] != -1) {
        // for each (u, v, c1) with Count[u, v, c1| P(p, p+j)+] != 0
        auto& countB = decompose[node.annotatedVertices[aj]].count;
        for (const auto& countUV: pathCounts[j - 1]) {
            auto u = countUV.first[0], v = countUV.first[1];
            auto c_v = makeSet(k, {getColor(v)});
            auto uv = {u, v};

            for (const auto& countUVC1: countUV.second) {
                const auto& c1 = countUVC1.first;

                // for each (v, c2) with Count[v, c2| B] != 0
                for (const auto& countVC2: const_cast<decltype(node.count)&>(countB)[{v}]) {
                    const auto& c2 = countVC2.first;
                    if ((c1 & c2) == c_v) {
                        pathCounts[j - 1][uv][c1 | c2] += countUVC1.second * countVC2.second;
                    }
                }
            }
        }
    }
}

void Graph::edgeJoin(const DecomposeTree2Node& node, const vector<DecomposeTree2Node>& decompose, bool direction,
                     size_t p, size_t k, size_t L, size_t j, vector<decltype(node.count)>& pathCounts) const {
    auto aj = direction ? lPlus(p, j) : lMinus(p, j + 1);

    if (node.annotatedEdges[aj] != -1) {
        auto& countB = decompose[node.annotatedEdges[aj]].count;
        // for each (u, v, c1) with Count[u, v, c1| P(p, p+j)+] != 0
        for (const auto& countUV: pathCounts[j - 1]) {
            auto u = countUV.first[0], v = countUV.first[1];
            auto c_v = makeSet(k, {getColor(v)});

            for (const auto& countUVC1: countUV.second) {
                const auto& c1 = countUVC1.first;

                // for each (v, w, c2) with Count[v, w, c2| B] != 0
                for (const auto& countVW: countB) {
                    if (countVW.first[0] != v)
                        continue;
                    auto w = countVW.first[1];
                    auto uw = {u, w};

                    for (const auto& countVWC2: countVW.second) {
                        const auto& c2 = countVWC2.first;

                        if ((c1 & c2) == c_v) {
                            pathCounts[j][uw][c1 | c2] += countUVC1.second * countVWC2.second;
                        }
                    }
                }
            }
        }
    }
    else {
        for (const auto& countUV: pathCounts[j - 1]) {
            auto u = countUV.first[0], v = countUV.first[1];
            for (const auto &countUVC1: countUV.second) {
                const auto &c1 = countUVC1.first;

                for (auto w: getAdj(v)) {
                    if (!c1[getColor(w)]) {
                        pathCounts[j][{u, w}][addColor(c1, getColor(w))] += countUVC1.second;
                    }
                }
            }
        }
    }
}

void Graph::calculateNode_PS(const Graph &Q, DecomposeTree2Node &node,
                             const std::vector<DecomposeTree2Node> &decompose) const {
    auto k = Q.size();

    if (node.vertices.size() == 2) {
        // leaf node
        // leaf node cannot have annotated edge.
        // TODO: to be implemented
    }
    else if (node.bNodeIndexes.size() == 1) {
        // 1 boundary cycle node
        // TODO: to be implemented
    }
    else {
        // 2 boundary cycle node
        auto L = node.vertices.size();
        auto p = node.bNodeIndexes[0], q = node.bNodeIndexes[1];

        // positiveCounts[i] = Count[*,*| P(p, p+i+1 % L)+]
        // negativeCounts[i] = Count[*,*| P(p, p-i-1 % L)-]
        vector<decltype(node.count)> positiveCounts(q - p), negativeCounts(p + L - q);

#pragma omp parallel sections default(shared)
        {
#pragma omp section
            // Initialize the counts
            if (node.annotatedEdges[p] != -1) {
                // edge (p, p+1) is annotated
                positiveCounts[0] = decompose[node.annotatedEdges[p]].count;
            }
            else {
                // not annotated, as common
                for (size_t u = 0; u < N; ++u) {
                    for (const auto& v: getAdj(u)) {
                        if (getColor(u) == getColor(v))
                            continue;
                        positiveCounts[0][{u, v}][makeSet(k, {getColor(u), getColor(v)})] = 1;
                    }
                }
            }

#pragma omp section
            // Initialize the counts
            if (node.annotatedEdges[lMinus(p, 1)] != -1) {
                // edge (p, p-1) is annotated
                negativeCounts[0] = decompose[node.annotatedEdges[lMinus(p, 1)]].count;
            }
            else {
                // not annotated, as common
                for (size_t u = 0; u < N; ++u) {
                    for (const auto& v: getAdj(u)) {
                        if (getColor(u) == getColor(v))
                            continue;
                        negativeCounts[0][{u, v}][makeSet(k, {getColor(u), getColor(v)})] = 1;
                    }
                }
            }
        }

#pragma omp parallel sections default(shared)
        {
#pragma omp section
            {
                for (size_t j = 1; j <= positiveCounts.size(); ++j) {
                    nodeJoin(node, decompose, true, p, k, L, j, positiveCounts);
                    if (j != positiveCounts.size())
                        edgeJoin(node, decompose, true, p, k, L, j, positiveCounts);
                }
            }

#pragma omp section
            {
                for (size_t j = 1; j <= negativeCounts.size(); ++j) {
                    nodeJoin(node, decompose, false, p, k, L, j, negativeCounts);
                    if (j != negativeCounts.size())
                        edgeJoin(node, decompose, false, p, k, L, j, negativeCounts);
                }
            }
        }

        // computing projection table for the cycle.
        const auto& positiveCount = positiveCounts.back();
        auto& negativeCount = negativeCounts.back();

        // for each (u, v, c1) with Count[u, v, c1|P+] != 0
        // for each (u, v, c2) with Count[u, v, c2|P-] != 0
        for (const auto& countPUV: positiveCount) {
            auto u = countPUV.first[0], v = countPUV.first[1];
            auto c_uv = makeSet(k, {getColor(u), getColor(v)});
            for (const auto& countPUVC1: countPUV.second) {
                const auto& c1 = countPUVC1.first;
                for (const auto& countNUVC2: negativeCount[countPUV.first]) {
                    const auto& c2 = countNUVC2.first;
                    if ((c1 & c2) == c_uv) {
                        node.count[countPUV.first][c1 | c2] += countPUVC1.second * countNUVC2.second;
                    }
                }
            }
        }
    }
}

void Graph::calculateNode_DB(const Graph &Q, DecomposeTree2Node &node, const std::vector<DecomposeTree2Node> &decompose) const {
    auto k = Q.size();

    if (node.vertices.size() == 2) {

    }
    else if (node.bNodeIndexes.size() == 1) {

    }
    else {
        auto L = node.vertices.size();
        auto p = node.bNodeIndexes[0], q = node.bNodeIndexes[1];

        // positiveCounts[{u, v, x, y}, c| P]
        // [NOTE]: if x or y is nil, its value is N.
        vector<decltype(node.count)> positiveCounts((L + 1) / 2);

        // for each h in 0..L - 1
        // Calculate Count*[x, y, c| C, hi = h]
#pragma omp parallel for private(L, p, q, positiveCounts)
        for (size_t h = 0; h < L; ++h) {
            // (operator ".>" means degree increment order, and id increment order when degree equals.)
            // (operator "%+" means plus modulo L. "%-" as it.)

            // Procedure 1: Compute Count*[u, v, c| P(h, d)+]
            // for each edge (u, v) in G with u .> v
            for (size_t u = 0; u < N; ++u) {
                for (auto v: getAdj(u)) {
                    if (higher(u, v)) {
                        vector<size_t> verEntry{u, v, N, N};
                        if ((h + 1) % L == p)
                            verEntry[2] = v;
                        if ((h + 1) % L == q)
                            verEntry[3] = v;

                        positiveCounts[0][verEntry][makeSet(k, {getColor(u), getColor(v)})] = 1;
                    }
                }
            }

            // for j = h %+ 2 ~ d
            for (size_t j = 1; j < positiveCounts.size(); ++j) {
                // for each (u, v, c) with Count*[u, v, c| P(h, j %- 1)+] != 0
                for (const auto& countUV: positiveCounts[j - 1]) {
                    auto u = countUV.first[0], v = countUV.first[1];
                    for (const auto& countUVC: countUV.second) {
                        const auto& colorSet = countUVC.first;
                        // for each edge (v, w) in G with u .> w && color(w) not in colorSet
                        for (auto w: getAdj(v)) {
                            if (higher(u, w) && !colorSet[getColor(w)]) {
                                auto verEntry = countUV.first;
                                verEntry[1] = w;
                                if ((h + j + 1) % L == p)
                                    verEntry[2] = w;
                                if ((h + j + 1) % L == q)
                                    verEntry[3] = w;

                                positiveCounts[j][verEntry][addColor(colorSet, getColor(w))] += countUVC.second;
                            }
                        }
                    }
                }
            }

            // Procedure 2: Compute Count*[x, y, c| C, hi = h] for config (A)
            // for each (u, v, x, c1) with Count*[u, v, x, c1| P(h, d)+] != 0

            // TODO: to be implemented
        }
    }
}
