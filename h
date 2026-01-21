#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// [CHANGE 1] Moved struct to top so functions can see it
struct node{
	int id, cost;
	bool has_elec;
};

// [CHANGE 2] Helper to look up costs by ID inside Prim's (since 'a' gets sorted)
vector<int> node_costs; 

// [CHANGE 3] Pass 'used' by reference so updates persist across function calls
long long solvewith_prim(int n, int root, const vector<vector<pair<int,int>>> &adj, vector<bool> &used) {
	vector<int> best(n + 1, INF); // Size n+1 for 1-based index
	vector<int> parent(n + 1, -1);
    // Removed local 'used' array, using the passed reference instead

	priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

	best[root] = 0;
	pq.push({0, root});

	long long totalWeight = 0;

	while (!pq.empty()) {
		auto cur = pq.top();
		pq.pop();

		int u = cur.second;
		int w = cur.first;

		if (used[u]) continue;
		used[u] = true;
		totalWeight += w;

		for (auto &edge : adj[u]) {
			int v = edge.first;
			int w = edge.second;
            
            // [CHANGE 4] Crucial Logic Fix:
            // Only add cable if it is cheaper than building a station at 'v' (w < node_costs[v])
			if (!used[v] && w < best[v] && w < node_costs[v]) {
				best[v] = w;
				parent[v] = u;
				pq.push({best[v], v});
			}
		}
	}

	return totalWeight;
}

bool comparator2(const node &a, const node &b) {
    return a.cost < b.cost;
}

int main() {
	int n, m;
	// [CHANGE 5] Added input for m
	if (!(cin >> n >> m)) return 0;

	vector<vector<pair<int,int>>> adj(n + 1);
	vector<node> a(n + 1);
    node_costs.resize(n + 1); // Resize global cost array

	// a[0] init removed as we use 1-based loop below
	for(int i = 1; i <= n; i++){
		cin >> a[i].cost;
		a[i].id = i;
		a[i].has_elec = false;
        node_costs[i] = a[i].cost; // Store cost for O(1) lookup
	}

	sort(a.begin() + 1, a.end(), comparator2); // Sort only 1..N

	for (int i = 0; i < m; i++) {
		int u, v, w;
		cin >> u >> v >> w;
		
        // Optimized check: If cable is more expensive than BOTH stations, ignore it.
		if(node_costs[u] < w && node_costs[v] < w){
			continue;
		}
		else{
		    adj[u].push_back({v, w});
		    adj[v].push_back({u, w});
		}
	}

    // [CHANGE 6] Global visited array to track who has electricity
    vector<bool> visited(n + 1, false);
	long long total = 0;

    // Your Loop Logic (Fixed):
    // Iterate through nodes sorted by cheapest station cost.
    // If a node is not yet powered (visited), build a station there and run Prim's.
	for(int i = 1; i <= n; i++){
        int u = a[i].id; // Get original ID
        
        if(!visited[u]) {
            // 1. Pay for the station
            total += a[i].cost;
            
            // 2. Run Prim's to connect all reachable nodes cheaper than their own stations
            total += solvewith_prim(n, u, adj, visited);
        }
	}
	
	cout << total;

	return 0;
}
