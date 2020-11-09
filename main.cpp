#include <iostream>

#include <functional>
#include <vector>
#include "podflow/graph.h"

using namespace std;
using namespace podflow;

int main() {
    // Building graph
    Graph g;

    Node* a = g.AddNode(nullptr, "a");
    Edge* sa = g.AddEdge(g.source_node(), a);

    Node* f = g.AddNode(nullptr, "f");
    Edge* sf = g.AddEdge(g.source_node(), f);

    Node* d = g.AddNode(nullptr, "d");
    Edge* sd = g.AddEdge(g.source_node(), d);

    Node* e = g.AddNode(nullptr, "e");
    Edge* se = g.AddEdge(g.source_node(), e);

    Node* h = g.AddNode([](std::vector<void*>& inputs, std::vector<void*>& outputs) {
        int* first_input = static_cast<int*>(inputs.front());
        int* second_input = static_cast<int*>(inputs.back());
        int* output = static_cast<int*>(outputs.front());
        *output = *first_input + *second_input;
    }, "h");
    int dh_int = 0;
    Edge* dh = g.AddEdge(d, h, &dh_int);
    int eh_int = 0;
    Edge* eh = g.AddEdge(e, h, &eh_int);
    int hs_int = 0;
    Edge* hs = g.AddEdge(h, g.sink_node(), &hs_int);

    Node* b = g.AddNode([](std::vector<void*>& inputs, std::vector<void*>& outputs) {
        int* input = static_cast<int*>(inputs.front());

        int* first_output = static_cast<int*>(outputs[0]);
        int* second_output = static_cast<int*>(outputs[1]);
        int* third_output = static_cast<int*>(outputs[2]);

        *first_output = *input + 1;
        *second_output = *input + 2;
        *third_output = *input + 3;
    }, "b");

    Node* c = g.AddNode([](std::vector<void*>& inputs, std::vector<void*>& outputs) {
        int* first_input = static_cast<int*>(inputs.front());
        int* second_input = static_cast<int*>(inputs.back());
        int* output = static_cast<int*>(outputs.front());
        *output = *first_input + *second_input;
    }, "c");

    int ab_int = 0;
    Edge* ab = g.AddEdge(a, b, &ab_int);

    int bs_int = 0;
    Edge* bs = g.AddEdge(b, g.sink_node(), &bs_int);

    int bs1_int = 0;
    Edge* bs1 = g.AddEdge(b, g.sink_node(), &bs1_int);

    int bc_int = 0;
    Edge* bc = g.AddEdge(b, c, &bc_int);

    int fc_int = 0;
    Edge* fc = g.AddEdge(f, c, &fc_int);

    int cs_int = 0;
    Edge* cs = g.AddEdge(c, g.sink_node(), &cs_int);

    // In real context, ab_int, fc_int etc can be used as market data.
    ab_int = 1;
    fc_int = 1;
    dh_int = 1;
    eh_int = 1;
    g.flow();
    cout << "graphviz:" << endl;
    cout << g.DebugString() << endl;

    ab_int = 2;  // Though ab_int is updated, pods of descent edges are not updated. Because the
    eh_int = 2;  // sa edge are not includes in the fire edges.
    g.SetSourceEdges({se});
    g.flow();

    cout << "graphviz:" << endl;
    g.RestoreSourceEdges();
    cout << g.DebugString() << endl;

    // Please paste graphviz text in web https://dreampuf.github.io/GraphvizOnline

    return 0;
}
