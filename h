#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

struct node{
	int id, cost;
};

vector<int> node_costs; 

long long solvewith_prim(int n, int root, const vector<vector<pair<int,int>>> &adj, vector<bool> &visited) {
	vector<int> best(n + 1, INF); 
	
	priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

	best[root] = 0;
	pq.push({0, root});

	long long totalWeight = 0;

	while (!pq.empty()) {
		auto cur = pq.top();
		pq.pop();

		int u = cur.second;
		int w = cur.first;

		if (visited[u]) continue;
		visited[u] = true;
		totalWeight += w;

		for (auto &edge : adj[u]) {
			int v = edge.first;
			int w = edge.second;
            
			if (!visited[v] && w < best[v] && w < node_costs[v]) {
				best[v] = w;
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
	int n;
	if(!(cin >> n)) return 0;
	
	vector<vector<pair<int,int>>> adj(n + 1);
	vector<node> a(n + 1);
    node_costs.resize(n + 1); 
    vector<bool> visited(n + 1, false); 

	for(int i = 1; i <= n; i++){
		cin >> a[i].cost;
		a[i].id = i;
        node_costs[i] = a[i].cost; 
	}

	sort(a.begin() + 1, a.end(), comparator2); 
	
    int m;
	cin >> m;
	for (int i = 0; i < m; i++) {
		int u, v, w;
		cin >> u >> v >> w;
		
		if(node_costs[u] < w && node_costs[v] < w){
			continue;
		}
		else{
		    adj[u].push_back({v, w});
		    adj[v].push_back({u, w});
		}
	}

	long long total = 0;

	for(int i = 1; i <= n; i++){
        int u = a[i].id; 
        
        if(!visited[u]){ 
            total += a[i].cost;
            total += solvewith_prim(n, u, adj, visited);
        }
	}
	cout << total;

	return 0;
}
