#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

const int INF = 1e9;

// Structure to represent an edge
struct Edge {
    int u, v, w;
    int id; // To keep track of original index if needed
};

// Comparator for sorting edges by weight
bool comparator(const Edge &a, const Edge &b) {
    return a.w < b.w;
}

// Disjoint Set Union (DSU) for Kruskal's Algorithm
struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n + 1);
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i)
            return i;
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

// Global adjacency list to store the MST structure
// Stores pairs of {neighbor, edge_weight}
// Size 505 covers the constraint N <= 500
vector<pair<int, int>> mst_adj[505];

// DFS to find the maximum weight edge on the path between u and target in the MST
// Returns:
//   -1: if target is NOT found in this subtree
//   val >= 0: the weight of the heaviest edge on the path found
int getMaxOnPath(int u, int target, int p) {
    // Base Case: We reached the target node. 
    // The max weight on a path of length 0 is 0.
    if (u == target) return 0;

    for (auto &edge : mst_adj[u]) {
        int v = edge.first;
        int w = edge.second;
        
        // Avoid going back to parent
        if (v != p) {
            int res = getMaxOnPath(v, target, u);
            
            // If res is not -1, it means the target was found in this branch
            if (res != -1) {
                // The max edge on the path is either the one we just traversed (w)
                // or the max edge found deeper in the recursion (res)
                return max(w, res);
            }
        }
    }
    
    // Target not found in any branch from this node
    return -1;
}

void solve() {
    int N, M;
    if (!(cin >> N >> M)) return;

    // Reset adjacency list for multiple test cases (if needed)
    for(int i=0; i<=N; ++i) mst_adj[i].clear();

    vector<Edge> edges(M);
    for (int i = 0; i < M; ++i) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        edges[i].id = i;
    }

    // --- Step 1: Standard Kruskal's to build the MST ---
    // We work on a sorted copy of edges for Kruskal's
    vector<Edge> sorted_edges = edges;
    sort(sorted_edges.begin(), sorted_edges.end(), comparator);

    DSU dsu(N);
    long long mst_weight = 0;
    int edges_count = 0;

    for (const auto &edge : sorted_edges) {
        if (dsu.unite(edge.u, edge.v)) {
            mst_weight += edge.w;
            edges_count++;
            
            // Add to MST Adjacency List (Undirected)
            mst_adj[edge.u].push_back({edge.v, edge.w});
            mst_adj[edge.v].push_back({edge.u, edge.w});
        }
    }

    // Edge Case: If the graph is disconnected, we can't form a spanning tree
    if (edges_count < N - 1) {
        cout << "-1" << endl;
        return;
    }

    // --- Step 2: Iterate ALL edges to find the best savings ---
    // We check every possible bridge to enchant.
    // If we enchant edge (u, v), we effectively get a "discount" equal to
    // the heaviest edge on the cycle it forms (or itself if it was in the MST).
    
    int max_savings = 0;

    for (const auto &edge : edges) {
        // Find the heaviest edge on the MST path between u and v
        // If edge was IN the MST, the path is just the edge itself, so it returns edge.w
        // If edge was NOT in the MST, it returns the max weight on the cycle.
        int current_savings = getMaxOnPath(edge.u, edge.v, -1);
        
        if (current_savings > max_savings) {
            max_savings = current_savings;
        }
    }

    // Result is Original MST Cost - Max Possible Saving
    cout << mst_weight - max_savings << endl;
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    solve();
    
    return 0;
}
