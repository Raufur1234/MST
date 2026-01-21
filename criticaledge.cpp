#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

using namespace std;

struct Edge {
    int u, v, w;
    int id;
};

bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}

// Disjoint Set Union (DSU)
struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n + 1);
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) parent[root_i] = root_j;
    }
};

// Global variables for Tarjan's Bridge Finding
vector<int> tin, low;
int timer;
vector<vector<pair<int, int>>> adj; // adj[u] = {v, edge_index}
vector<string> results;

void dfs_bridges(int u, int p = -1, int edge_id = -1) {
    tin[u] = low[u] = ++timer;
    
    for (auto& edge : adj[u]) {
        int v = edge.first;
        int id = edge.second;
        
        if (id == edge_id) continue; // Don't go back through the same edge
        
        if (tin[v] != -1) {
            // Back-edge
            low[u] = min(low[u], tin[v]);
        } else {
            // Tree-edge
            dfs_bridges(v, u, id);
            low[u] = min(low[u], low[v]);
            
            // Bridge check
            if (low[v] > tin[u]) {
                results[id] = "Critical";
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);
    for (int i = 0; i < m; ++i) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        edges[i].id = i;
    }

    sort(edges.begin(), edges.end(), compareEdges);

    DSU dsu(n);
    results.resize(m, "None"); // Default to None

    // Buffers for bridge finding
    // We map component IDs to small integers 0..k for the temp graph
    tin.resize(n + 1);
    low.resize(n + 1);
    adj.resize(n + 1);

    int i = 0;
    while (i < m) {
        int j = i;
        while (j < m && edges[j].w == edges[i].w) j++;

        // 1. Identify valid edges (Can be in MST)
        // Add them to a temporary graph to find bridges
        vector<int> nodes_in_batch;
        
        for (int k = i; k < j; ++k) {
            int root_u = dsu.find(edges[k].u);
            int root_v = dsu.find(edges[k].v);

            if (root_u != root_v) {
                // This edge connects two previously unconnected components
                results[edges[k].id] = "Pseudo-Critical"; // Tentative assignment
                
                // Build adjacency list for temp graph
                adj[root_u].push_back({root_v, edges[k].id});
                adj[root_v].push_back({root_u, edges[k].id});
                
                nodes_in_batch.push_back(root_u);
                nodes_in_batch.push_back(root_v);
            } else {
                // Loops within the same component -> Never in MST
                results[edges[k].id] = "None";
            }
        }

        // 2. Find Critical edges (Bridges in the temp graph)
        timer = 0;
        // Reset tin/low for involved nodes only
        for (int node : nodes_in_batch) {
            tin[node] = -1;
            low[node] = -1;
        }

        for (int node : nodes_in_batch) {
            if (tin[node] == -1) {
                dfs_bridges(node);
            }
        }

        // 3. Clean up and Union
        for (int node : nodes_in_batch) {
            adj[node].clear(); // Clear adjacency list for next batch
        }

        for (int k = i; k < j; ++k) {
            dsu.unite(edges[k].u, edges[k].v);
        }

        i = j;
    }

    // Output results
    for (int k = 0; k < m; ++k) {
        cout << results[k] << "\n";
    }

    return 0;
}