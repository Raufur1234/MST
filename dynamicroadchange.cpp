#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <queue>

using namespace std;

const int INF = 1e9;

struct Edge {
    int u, v, w;
    int id;
};

// Comparator for initial Kruskal's
bool compareEdges(const Edge &a, const Edge &b) {
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
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            return true;
        }
        return false;
    }
};

// Global State
int N, M, Q;
vector<Edge> all_edges;       // Stores all edges (0-indexed by input order)
vector<bool> is_in_mst;       // Tracks if edge[i] is currently in MST
vector<int> mst_adj[205];     // Adjacency list for current MST
long long current_mst_cost = 0;

// Helper to rebuild adjacency list (optional, but keeps logic clean)
void rebuild_adj() {
    for (int i = 0; i <= N; ++i) mst_adj[i].clear();
    for (const auto& edge : all_edges) {
        if (is_in_mst[edge.id]) {
            mst_adj[edge.u].push_back(edge.v);
            mst_adj[edge.v].push_back(edge.u);
        }
    }
}

// Run standard Kruskal's once at the start
void initial_kruskal() {
    vector<Edge> sorted_edges = all_edges;
    sort(sorted_edges.begin(), sorted_edges.end(), compareEdges);
    
    DSU dsu(N);
    is_in_mst.assign(M, false);
    
    for (const auto& edge : sorted_edges) {
        if (dsu.unite(edge.u, edge.v)) {
            current_mst_cost += edge.w;
            is_in_mst[edge.id] = true;
        }
    }
    rebuild_adj();
}

// BFS to find all nodes reachable from 'start_node' 
// WITHOUT using the edge connecting 'u' and 'v' (effectively finding one cut component)
void get_component(int start_node, int avoid_node, vector<bool>& visited) {
    queue<int> q;
    q.push(start_node);
    visited[start_node] = true;
    
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        
        for (int neighbor : mst_adj[curr]) {
            if (neighbor == avoid_node) continue; // Don't cross the removed edge
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N >> M >> Q)) return 0;

    for (int i = 0; i < M; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        all_edges.push_back({u, v, w, i});
    }

    // Step 1: Compute Initial MST
    initial_kruskal();

    // Step 2: Process Queries
    for (int k = 0; k < Q; ++k) {
        int edge_idx, added_weight;
        cin >> edge_idx >> added_weight;
        int id = edge_idx - 1; // Convert 1-based to 0-based

        // Update the weight permanently
        all_edges[id].w += added_weight;

        // CASE 1: Edge is NOT in MST
        // Increasing its weight makes it even less likely to be useful.
        // Cost remains the same.
        if (!is_in_mst[id]) {
            cout << current_mst_cost << "\n";
            continue;
        }

        // CASE 2: Edge IS in MST
        // We tentatively increase the MST cost
        current_mst_cost += added_weight;

        // We need to check if we can swap this edge for a cheaper one.
        // The edge connects u and v. Removing it splits the tree.
        int u = all_edges[id].u;
        int v = all_edges[id].v;

        // Find which nodes are in u's component after cutting the edge
        vector<bool> in_u_component(N + 1, false);
        get_component(u, v, in_u_component);

        // Find the best replacement edge
        int best_replacement_id = -1;
        int min_replacement_w = INF;

        for (const auto& edge : all_edges) {
            // We are looking for an edge that crosses the cut:
            // One endpoint is in u's component, the other is NOT (i.e., in v's component)
            bool u_side = in_u_component[edge.u];
            bool v_side = in_u_component[edge.v];

            if (u_side != v_side) {
                // This edge crosses the cut.
                // It must be cheaper than the current (now increased) weight of our MST edge.
                // And it shouldn't be the edge itself (though checking edge.id != id is implicit by weight logic usually)
                if (edge.id != id && edge.w < min_replacement_w) {
                    min_replacement_w = edge.w;
                    best_replacement_id = edge.id;
                }
            }
        }

        // Swap if a better edge exists
        // Note: We only swap if the replacement is STRICTLY better than the NEW weight of the current edge
        if (best_replacement_id != -1 && min_replacement_w < all_edges[id].w) {
            
            // Adjust Cost
            current_mst_cost -= all_edges[id].w;       // Remove expensive edge
            current_mst_cost += min_replacement_w;     // Add cheap replacement
            
            // Update Structure
            is_in_mst[id] = false;
            is_in_mst[best_replacement_id] = true;
            
            // Update Adjacency List for next query
            // (We handle the specific remove/add carefully to avoid full rebuild for speed, 
            //  but full rebuild is fine for N=200. Here we do targeted update).
            
            // Remove u-v
            // (Simpler to just call rebuild_adj() for safety given small N)
            rebuild_adj();
        }

        cout << current_mst_cost << "\n";
    }

    return 0;
}
