//
// Created by linhan5 on 2020/6/30.
//

#include "podflow/graph.h"
#include <sstream>
#include <algorithm>

namespace podflow {

    // Node
    std::string Node::DebugString() const {
        std::ostringstream oss;
        oss << "{name: " << name_ << ", id:" << id_ << "}";
        return oss.str();
    }

    Node::Node() : id_(-1) {}

    void Node::Initialize(int id, const OpFunc &op) {
        id_ = id;
        name_ = "node_" + std::to_string(id_);
        op_ = op;
    }

    void Node::flow(std::vector<void *> &input, std::vector<void *> &output) {
        if (op_)
            op_(input, output);
    }

    // Edge
    std::string Edge::DebugString() const {
        std::ostringstream oss;
        oss << "{name: ";

        return oss.str();
    }

    // Graph
    Graph::Graph() {
        Node* source = new Node();
        source->Initialize(0, nullptr);
        source->set_name("souce");
        nodes_.emplace_back(source);

        Node* sink = new Node();
        sink->Initialize(1, nullptr);
        sink->set_name("sink");
        nodes_.emplace_back(sink);
    }

    Graph::~Graph() {
        for (Node* node : nodes_) {
            if (node != nullptr) {
                delete node;
            }
        }

        for (Edge* edge : edges_) {
            if (edge != nullptr) {
                delete edge;
            }
        }
    }

    std::string Graph::DebugString() const {
        std::ostringstream oss;
        oss << "digraph G { \n";
        for (Node* snode : nodes_) {
            if (snode) {
                for (Edge* edge : snode->out_edges_) {
                    Node* dnode = edge->dst_;
                    oss << snode->name_ << " -> " << dnode->name_;
                    if (edge->pod_) {
                        int* ptr = static_cast<int*>(edge->pod_);
                        oss << " [ label=" << *ptr << "]";
                    }
                    oss << ";\n";
                }
            }
        }
        oss << "}\n";

        oss << "flow order:\n";
        for (Node* node : order_) {
            oss << node->name_ << " ";
        }
        oss << "\n";

        return oss.str();
    }

    Node* Graph::AddNode(const OpFunc &op, const std::string &name)  {
        Node* node = new Node();
        node->Initialize(nodes_.size(), op);
        if (name.empty()) {
            node->set_name("node_" + std::to_string(node->id()));
        } else {
            node->set_name(name);
        }
        nodes_.emplace_back(node);
        return node;
    }

    Edge* Graph::AddEdge(Node *source, Node *dest, void* pod) {
        Edge* e = new Edge();
        e->id_ = edges_.size();
        e->name_ = "edge_" + std::to_string(e->id_);
        e->pod_ = pod;
        e->src_ = source;
        e->dst_ = dest;
        e->src_output_ = source->out_edges_.size();
        e->dst_input_ = dest->in_edges_.size();
        source->out_edges_.emplace_back(e);
        dest->in_edges_.emplace_back(e);
        edges_.emplace_back(e);

        if (source == source_node()) {
            source_edges_.emplace_back(e);
        }

        return e;
    }

    void Graph::DFS(const std::function<void (Node *)> &enter, const std::function<void (Node *)> &leave) {
        struct Work {
            Node* node;
            bool leave;
        };
        std::vector<Work> stack;
        stack.emplace_back(Work{source_node(), false});

        std::vector<bool> visited(nodes_.size(), false);
        while (!stack.empty()) {
            Work w = stack.back();
            stack.pop_back();

            Node* n = w.node;
            if (w.leave) {
                leave(n);
                continue;
            }

            if (visited[n->id()]) continue;
            visited[n->id()] = true;
            if (enter) enter(n);

            if (leave) stack.emplace_back(Work{n, true});

            for (const Edge* out_edge : n->out_edges()) {
                if (!visited[out_edge->dst()->id()]) {
                    stack.emplace_back(Work{out_edge->dst(), false});
                }
            }
        }
    }

    void Graph::UpdateFlowOrder() {
        order_.clear();
        DFS(nullptr, [this](Node* n) { order_.emplace_back(n); });
        std::reverse(order_.begin(), order_.end());
    }

    void Graph::flow() {
        UpdateFlowOrder();
        for (Node* node : order_) {
            std::vector<void*> input;
            std::vector<void*> output;
            for (Edge* e : node->in_edges_) {
                input.emplace_back(e->pod_);
            }
            for (Edge* e : node->out_edges_) {
                output.emplace_back(e->pod_);
            }

            node->flow(input, output);
        }
    }

    void Graph::SetSourceEdges(std::vector<Edge *> fire_edges) {
        source_node()->out_edges_.assign(fire_edges.begin(), fire_edges.end());
    }

    void Graph::RestoreSourceEdges() {
        source_node()->out_edges_.assign(source_edges_.begin(), source_edges_.end());
    }
}
