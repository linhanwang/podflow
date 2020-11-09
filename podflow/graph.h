//
// Created by linhan5 on 2020/6/29.
//

#ifndef PODFLOW_GRAPH_H
#define PODFLOW_GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;         \
  void operator=(const TypeName&) = delete

#ifndef MAX_NODE_NUM
#define MAX_NODE_NUM 1000
#endif

namespace podflow {
    using VecVoidPtr = std::vector<void*>;
    using OpFunc = std::function<void(VecVoidPtr&, VecVoidPtr&)>;

    class Edge;
    class Graph;

    class Node {
    public:
        ~Node() {}
        std::string DebugString() const;
        int id() const { return id_; }
        const std::string& name() const { return name_; }
        void set_name(const std::string& name) { name_ = name; }

        std::size_t num_inputs() const { return in_edges_.size(); }
        std::size_t num_outputs() const { return out_edges_.size(); }

        std::vector<Edge*>& in_edges() { return in_edges_; }
        std::vector<Edge*>& out_edges() { return out_edges_; }

        // Returns the edge connecting to the 'idx' input of this Node.
        Edge* input_edge(int idx) { return in_edges_[idx]; }
        // Returns the edge connecting to the 'idx' output of this Node.
        Edge* output_edge(int idx) { return out_edges_[idx]; }

        // Executes the operator of this Node.
        // Inputs are pods of input edges. Outputs are pods of output edges.
        void flow(std::vector<void*>& inputs,std::vector<void*>& outputs);

    private:
        friend class Graph;
        Node();

        void Initialize(int id, const OpFunc &op);

        int id_;                            // Index in the graph' vector<Node*>.
        std::string name_;
        OpFunc op_;                         // Operator of this Node.

        std::vector<Edge*> in_edges_;       // Input edges connecting to this Node
        std::vector<Edge*> out_edges_;      // Output edges connecting to this Node

        DISALLOW_COPY_AND_ASSIGN(Node);
    };

    // todo: add deleter to Edge pod

    class Edge {
    public:
        ~Edge() {}
        Node* src() const { return src_; }
        Node* dst() const { return dst_; }
        int id() const { return id_; }
        const std::string& name() const { return name_; }
        void set_name(const std::string& name) { name_ = name; }

        void set_pod(void* pod) { pod_ = pod; }
        void* get_pod() { return pod_; }

        int src_output() const { return src_output_; }
        int dst_input() const { return dst_input_; }

        std::string DebugString() const;
    private:
        Edge() : pod_(nullptr) {}

        friend class Graph;

        Node* src_;                     // Source Node
        Node* dst_;                     // Destination Node
        int id_;                        // Index in graph' vector<Edge*>
        std::string name_;
        int src_output_;                // Index in out_edges_ of source node
        int dst_input_;                 // Index in in_edges of destination node
        void* pod_;                     // plain old data of this Edge

        DISALLOW_COPY_AND_ASSIGN(Edge);
    };

    class Graph {
    public:
        Graph();
        ~Graph();

        std::string DebugString() const;
        // Every graph has a single source node.
        Node* source_node() { return nodes_[0]; }
        // Every graph has a single sink node.
        Node* sink_node() { return nodes_[1]; }

        // Add a node to this graph.
        Node* AddNode(const OpFunc & op, const std::string &name = "");
        // Add a node to this graph.
        Edge* AddEdge(Node* source, Node* dest, void* pod = nullptr);

        // Executes flow() of all nodes in order after topological sorting
        void flow();

        // Replaces out_edges of source_node with fire edges.
        // Executing this function before flow() will ignore
        // other out edges of source node when doing topological sorting.
        void SetSourceEdges(std::vector<Edge*> fire_edges);

        // Restore out_edges of source_node with the original one.
        void RestoreSourceEdges();

    private:
        // Perform a depth-first-search on this graph starting at the source node.
        // If enter is not empty, calls enter(n) before visiting any children of n.
        // If leave is not empty, calls leave(n) after visiting all children of n.
        void DFS(const std::function<void(Node*)>& enter, const std::function<void(Node*)>& leave);

        // Topological soring. Orders of nodes are stored in order_.
        void UpdateFlowOrder();

        std::vector<Node*> nodes_;                   // All nodes of the graph.
        std::vector<Edge*> edges_;                   // All edges of the graph.
        std::vector<Edge*> source_edges_;            // Original out_edges_ of source node

        std::vector<Node*> order_;                   // orders of nodes after topological sorting

        DISALLOW_COPY_AND_ASSIGN(Graph);
    };
}


#endif //PODFLOW_GRAPH_H
