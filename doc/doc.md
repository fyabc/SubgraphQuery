# 代码说明

## 代码结构：（比较乱，见谅...）

    /SubgraphQuery
        graph.h             --- Graph类定义
        utils.h             --- 一些工具函数
        graphBase.cpp       --- graph的一些基本的方法
        easyQueries.cpp     --- 一些简单的查询
        subgraphSample.cpp
        2dFullTreeQuery.cpp
        colorCoding.cpp     --- 上面三个文件分别实现了几种不同类型的子图查询
        query.cpp           --- 之前写的一个使用抽样方法统计子图个数的程序，现在用处不大了
        test.cpp            --- 一些小的测试
        (config.txt)        --- 包含指向数据文件夹的路径

        /tests              --- 一些查询的测试文件
            subgraphGen.cpp     --- 生成一些特殊查询图的程序(depth=2 tree, ER)
            test2dFullTree.cpp  --- 深度为2的满的树的查询
            testColorCoding.cpp --- 使用用Color Coding算法的查询
            testStar.cpp        --- 对星形子图的测试
            testEgonet.cpp      --- 对Egonet的测试

        /decompositions     --- 一些查询图的分解
            *.qd 格式:
            3               // Q顶点数
            0 1
            0 2
            1 2
            -1 -1           // Q的若干条边，以-1 -1结尾

            // 每个node是Graph::DecomposeTree2Node的对象
            // 各行对应各个数据成员
            // 中间无空行
            1               // 分解后的node个数
            0 2 1           // vertices
                            // children
            -1 -1 -1        // annotatedVertices
            -1 -1 -1        // annotatedEdges
            1 2             // bNodeIndexes

    (tests里面的代码可能之后改成更加方便脚本调用的比较好)

## Graph类一些方法说明

    Graph                       --- 表示一个无向图的类
    Graph::Vertex               --- 顶点，包含degree, adjacent, color
    Graph::DecomposeTreeNode    --- 之前写的另一个限制子图为树的Color Coding算法[FASCIA 2013]使用的Decomposition Tree
    Graph::DecomposeTree2Node   --- 目前正在写的限制子图为treewidth <= 2的Color Coding算法[Subgraph Counting 2016]
                                    使用的Decomposition Tree, 其中各个变量含义按paper里面的含义

    Graph::fromXxxx, Graph::createXxxx
                                --- some factory methods, fromFile方法从文件（必须是graphgen生成的格式）读入一个图
                                    createTree生成深度为depth, 各层子节点数均为width的树
                                    createTreeH各层子节点数分别为输入各项的树

    Some basic methods:         --- degree(i), adj(i), etc.

    Simple Sampling:            --- 一开始做的,通过抽样的方式估算子图匹配概率
        Graph::sampleSubgraph   --- 测试sampleTime次，返回Q的匹配次数

    Star Query:                 --- 星形子图查询，对root和leaf可以有度数的宽松或严格限制
        Graph::getSubgraphNumber_Star_*
                                --- 对应soft或hard各种限制的查询，主要使用_All方法

    Color Coding:               --- 使用Color Coding算法的子图查询；sampleTimes为测试次数
        Graph::getSubgraphNumber_BF
                                --- 暴力枚举colorful match个数，然后计算match个数(用于检查其他函数正确性)
        Graph::getSubgraphNumber_Tree
                                --- 使用[FASCIA 2013]的算法估算match个数(子图必须为树)
        Graph::getSubgraphNumber_2Treewidth_Decompose
                                --- 输入Q和Q的分解decompose， 使用[Subgraph Counting 2016]中的PS算法估算match个数
                                --- (生成decompose的算法和使用DB的算法待实现)

    2-depth Full Tree Query:    --- 深度为2的满树的查询（不是很重要）
        Graph::getSubgraphNumber_2dFullTree
        Graph::getSubgraphNumber_2dFullTree_ignore
        Graph::testSubgraph_2dTree
                                --- 先用_ignore计算出不考虑重复顶点时的match数目，然后乘以testSubgraph_2dTree抽样得到的概率，
                                    得到实际的match个数估计值

    Other Special Queries:      --- 一些其他查询，包括子图中三角形个数查询
