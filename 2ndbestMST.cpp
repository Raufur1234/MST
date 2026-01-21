#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

const int INF = 1e9;

struct Edge {
    int u, v, w;
    int id; // Optional: helps if we need to track original indices
};

bool comparator(const Edge &a, const Edge &b) {
    return a.w < b.w;
}

struct DSU {
    vector<int> parent, rnk;
    int num_components; // Track components to check connectivity

    DSU(int n) {
        parent.resize(n + 1); // 1-based indexing safety
        rnk.assign(n + 1, 0);
        num_components = n;
        for (int i = 0; i <= n; i++) { parent[i] = i; }
    }

    int find_set(int v) {
        if (parent[v] == v) return v;
        return parent[v] = find_set(parent[v]);
    }

    bool unite(int a, int b) {
        a = find_set(a);
        b = find_set(b);
        if (a == b) return false;
        
        num_components--; // Decrement component count
        
        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

// Global MST Adjacency List for DFS traversal
// Stores pairs of {neighbor, weight}
vector<pair<int, int>> mst_adj[505]; 

// DFS to find the path between u and target in the MST
// Stores the weights of edges on the path in 'path_weights'
bool getPath(int u, int target, int p, vector<int>& path_weights) {
    if (u == target) return true;

    for (auto &edge : mst_adj[u]) {
        int v = edge.first;
        int w = edge.second;
        if (v != p) {
            path_weights.push_back(w); // Add edge weight to path
            if (getPath(v, target, u, path_weights)) return true; // Found target
            path_weights.pop_back(); // Backtrack
        }
    }
    return false;
}

void solve_second_best_mst(int n, vector<Edge> &edges) {
    sort(edges.begin(), edges.end(), comparator);

    DSU dsu(n);
    long long mstWeight = 0;
    vector<Edge> unused_edges;
    int edges_count = 0;

    // --- Step 1: Standard Kruskal's to build MST ---
    for (auto &edge : edges) {
        if (dsu.unite(edge.u, edge.v)) {
            mstWeight += edge.w;
            edges_count++;
            
            // Build the MST graph for traversal later (Undirected = add both ways)
            mst_adj[edge.u].push_back({edge.v, edge.w});
            mst_adj[edge.v].push_back({edge.u, edge.w});
        } else {
            // If adding this edge creates a cycle, it's a candidate for the 2nd best MST
            unused_edges.push_back(edge);
        }
    }

    // Check if a valid MST even exists
    if (edges_count < n - 1) {
        cout << "-1" << endl;
        return;
    }

    // --- Step 2: Find Strictly Second Best MST ---
    long long min_diff = -1; // -1 indicates no valid strictly greater tree found yet

    // Try adding every unused edge
    for (const auto &new_edge : unused_edges) {
        vector<int> path_weights;
        // Find the path in the MST between the two endpoints of the unused edge
        getPath(new_edge.u, new_edge.v, -1, path_weights);

        // We want to remove an edge from the cycle (path_weights) 
        // such that new_total > mstWeight.
        // new_total = mstWeight - removed_w + new_edge.w
        // Condition: new_edge.w > removed_w
        
        for (int w_removed : path_weights) {
            if (new_edge.w > w_removed) {
                long long diff = new_edge.w - w_removed;
                
                // We want the smallest positive difference
                if (min_diff == -1 || diff < min_diff) {
                    min_diff = diff;
                }
            }
        }
    }

    if (min_diff == -1) {
        cout << "-1" << endl;
    } else {
        cout << mstWeight + min_diff << endl;
    }
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Edge> edges(m);
    for (int i = 0; i < m; i++) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }

    solve_second_best_mst(n, edges);

    return 0;
}