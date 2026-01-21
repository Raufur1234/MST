#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// Structure to represent an edge
struct Edge {
    int u, v, w, id;
};

// Comparator for sorting edges
bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}

// Disjoint Set Union (DSU) structure
struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n + 1);
        for (int i = 0; i <= n; ++i) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
        }
    }
};

// Global variables for Bridge Finding
const int MAXN = 1005; // Adjust based on N constraints
vector<pair<int, int>> temp_adj[MAXN]; // Adjacency list for component graph: {neighbor, edge_id}
int tin[MAXN], low[MAXN];
int timer;
vector<int> critical_edges; // To store IDs of critical edges

// DFS to find bridges
// u: current node, p: parent edge ID (to avoid going back up the SAME edge)
void dfs_bridges(int u, int p_edge_id = -1) {
    tin[u] = low[u] = ++timer;
    
    for (auto& edge : temp_adj[u]) {
        int v = edge.first;
        int id = edge.second;
        
        if (id == p_edge_id) continue; // Don't go back along the same edge
        
        if (tin[v]) {
            // Ancestor found (back-edge), update low-link
            low[u] = min(low[u], tin[v]);
        } else {
            // Unvisited node, recurse
            dfs_bridges(v, id);
            low[u] = min(low[u], low[v]);
            
            // Bridge Condition: If v can't reach u or above without this edge
            if (low[v] > tin[u]) {
                critical_edges.push_back(id);
            }
        }
    }
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    vector<Edge> edges;
    for (int i = 0; i < M; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        edges.push_back({u, v, w, i});
    }

    // Step 1: Sort edges by weight
    sort(edges.begin(), edges.end(), compareEdges);

    DSU dsu(N);
    int edge_idx = 0;

    // Step 2: Process edges in blocks of equal weight
    while (edge_idx < M) {
        int j = edge_idx;
        
        // Find the range [edge_idx, j) containing all edges of the same weight
        while (j < M && edges[j].w == edges[edge_idx].w) {
            j++;
        }

        // --- PHASE 1: Build the Temporary Component Graph ---
        // Nodes in this graph are the ROOTS of the current DSU components.
        
        // Reset specialized containers for this batch
        timer = 0;
        vector<int> nodes_in_batch; 

        for (int k = edge_idx; k < j; ++k) {
            int u = edges[k].u;
            int v = edges[k].v;
            int root_u = dsu.find(u);
            int root_v = dsu.find(v);

            if (root_u != root_v) {
                // Add edge between components. 
                // Note: We might add multiple edges between the same two roots!
                // The bridge finding logic handles this naturally (multiple edges = cycle = not a bridge).
                temp_adj[root_u].push_back({root_v, edges[k].id});
                temp_adj[root_v].push_back({root_u, edges[k].id});
                
                nodes_in_batch.push_back(root_u);
                nodes_in_batch.push_back(root_v);
                
                // Reset visited arrays for these specific nodes
                tin[root_u] = 0; 
                tin[root_v] = 0;
                low[root_u] = 0;
                low[root_v] = 0;
            }
        }

        // --- PHASE 2: Find Bridges ---
        for (int node : nodes_in_batch) {
            if (!tin[node]) {
                dfs_bridges(node);
            }
        }

        // --- PHASE 3: Clean up and Unite ---
        for (int node : nodes_in_batch) {
            temp_adj[node].clear(); // Clear adjacency list for next batch
        }

        // Now actually perform the unions in DSU
        for (int k = edge_idx; k < j; ++k) {
            dsu.unite(edges[k].u, edges[k].v);
        }

        // Move to next block
        edge_idx = j;
    }

    // Output Result
    sort(critical_edges.begin(), critical_edges.end());
    
    cout << "Number of Critical Edges: " << critical_edges.size() << endl;
    
    // Optional: Print the specific edges (by their original index if needed)
    // for (int id : critical_edges) cout << id << " ";
    // cout << endl;

    return 0;
}