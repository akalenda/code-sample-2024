#include <iostream>
#include <string>
#include <unordered_set>
#include <queue>
#include <thread>
#include <vector>
#include "../lib/json.hpp"

auto parseAdjacencyListFromStdIn () {
  std::stringstream stdinBuffer;
  stdinBuffer << std::cin.rdbuf();
  return nlohmann::json::parse(stdinBuffer.str());
}

/*!
  @brief 
    Constructs a minimum spanning tree. 
    Can be called concurrently. 
    The centerpiece of this source file.
    Time complexity O(e), where e is number of edges in the original graph
  @param[in] adjacencyList
    JSON object describing a graph.
    Keys are vertices, and values are lists of vertices connected to that key, i.e.:
    i.e. this graph:
    . hi---hit---hot---hoh---oh
    .           /  |
    .          /   |
    .         /    |
    .       lot--dot--dog
    .          \      / |
    .           log--<  |
    .                 \ |
    .                 cog
    would be represented as such:
    {
      "cog":["dog","log"],
      "dog":["cog","log","dot"],
      "dot":["hot","lot","dog"],
      "hi":["hit"],
      "hit":["hot","hi"],
      "hoh":["oh","hot"],
      "hot":["hoh","dot","lot","hit"],
      "log":["dog","cog","lot"],
      "lot":["hot","dot","log"],
      "oh":["hoh"]
    }
  @param[in,out] root
    The vertex that will serve as the root (aka source) of the tree.
  @param[out] minSpanTree
    json of the minimum spanning tree.
    Its format is mostly the same as that for the input adjacency list.
    E.g. for root "dot", the minimum spanning tree can be:
      . hi---hit---hot---hoh---oh
      .              |
      .       lot--dot--dog
      .                 / \
      .              log  cog
    would be represented as such:
    {
      "dog":["cog","log"],
      "dot":["hot","lot","dog"],
      "hit":["hi"],
      "hoh":["oh"],
      "hot":["hoh","hit"],
    }
    json is chosen here instead of more native C constructs, because
    it will expedite writing the data to stdout. But it does cost us
    some runtime performance, especially with this library.
  @return Nothing
*/
void constructMinSpanTree (nlohmann::json& adjacencyList, std::string root, nlohmann::json& minSpanTree) {
  std::unordered_set<std::string> nodesVisited = {root};
  std::queue<std::string> queue;
  queue.push(root);
  while (queue.size() > 0) {
    auto vertex = queue.front();
    queue.pop();
    minSpanTree[vertex] = "[]"_json;
    for (auto& adjacentVertex : adjacencyList[vertex]) {
      if (nodesVisited.count(adjacentVertex) == 0) {
        nodesVisited.insert(adjacentVertex);
        queue.push(adjacentVertex);
        minSpanTree[vertex].push_back(adjacentVertex);
      }
    }
  }
}

/*!
  @brief
    Invokes the constructMinSpanTree function for each vertex as a root, each in a separate thread.
    Time complexity is thus O(ve), where v is the number vertices, e the number of edges.
    Note that the split and the merge both occur in this function,
    and the minSpanTrees are all independent of one another. This is how
    we intend to avoid race conditions, and the expectation is that no
    locks or mutexes would be needed elsewhere.
  @param[in] adjacencyList
    directly translated to constructMinSpanTree without modification;
    see documentation there for expected format
  @param[out] allMinSpanTrees
    a mapping of vertices to the output of constructMinSpanTree when
    that vertex is used as the root, e.g.:
    {
      "dog": {...minSpanTree...},
      "dot": {...minSpanTree...},
      "hit": {...minSpanTree...},
      "hoh": {...minSpanTree...},
      "hot": {...minSpanTree...},
    }
  @return Nothing
*/
void constructAllMinSpanTreesConcurrently (nlohmann::json& adjacencyList, nlohmann::json& allMinSpanTrees) {
  std::vector<std::thread> threads;
  for (auto& [vertex, _] : adjacencyList.items()) {
    allMinSpanTrees[vertex] = nlohmann::json();
    std::thread th = std::thread([vertex, adjacencyList, &allMinSpanTrees]() mutable { 
      constructMinSpanTree(adjacencyList, vertex, allMinSpanTrees[vertex]);
    });
    threads.push_back(std::move(th));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

int main() {
  auto adjacencyList = parseAdjacencyListFromStdIn();
  nlohmann::json allMinSpanTrees;
  constructAllMinSpanTreesConcurrently(adjacencyList, allMinSpanTrees);
  std::cout << allMinSpanTrees.dump(2) << std::endl;
  return 0;
}
