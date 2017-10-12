#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;


struct TGraph {
    vector<vector< pair<int, long long> > > vertices;
    long long nOfVertices;
    long long nOfEdge;
    vector<long long> d;
};


void CreateGraph(TGraph &graph);
void PrintGraph(TGraph &graph);
void Dijkstra(TGraph &graph, int start, vector<long long> &vec);
bool BelmanFord(TGraph &graph);


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    TGraph graph;
    CreateGraph(graph);
    
    if (!BelmanFord(graph)) {
        cout << "Negative cycle\n";
        return 0;
    }
    
    for (int j = 0; j < graph.nOfVertices; ++j) {
        for (int k = 0; k < graph.vertices[j].size(); ++k) {
            graph.vertices[j][k].second += graph.d[j] - graph.d[graph.vertices[j][k].first];
        }
    }
    
    vector <long long> lineOfResMatrix(graph.nOfVertices);
    for (int j = 0; j < graph.nOfVertices; ++j) {
        Dijkstra(graph, j, lineOfResMatrix);
        for (int k = 0; k < graph.nOfVertices - 1; k++) {
            if (lineOfResMatrix[k] == LLONG_MAX) {
                cout << "inf ";
            } else {
                cout <<  lineOfResMatrix[k] - (graph.d[j] - graph.d[k]) << " ";
            }
        }
        
        if (lineOfResMatrix[graph.nOfVertices - 1] == LLONG_MAX) {
            cout << "inf";
        } else {
            cout <<  lineOfResMatrix[graph.nOfVertices - 1] - (graph.d[j] - graph.d[graph.nOfVertices - 1]);
        }
        
        cout << "\n";
    }
    return 0;
}

inline long long SumNoOverflow(long long a, long long b)
{
    long long sum = a + b;
    if (a < 0 && b < 0 && sum > 0) {
        return -LLONG_MAX;
    } else if (a > 0 && b > 0 && sum < 0) {
        return LLONG_MAX;
    } else {
        return sum;
    }
}


bool BelmanFord(TGraph &graph)
{
    for (int i = 0; i < graph.nOfVertices - 1; ++i) {
        graph.vertices[graph.nOfVertices - 1].push_back(make_pair(i, 0));
        graph.vertices[i].shrink_to_fit();
    }
    
    graph.d.resize(graph.nOfVertices, LLONG_MAX);
    for (int i = 0; i < graph.nOfVertices; i++) {
        graph.d[i] = LLONG_MAX;
    }
    
    graph.d.shrink_to_fit();
    graph.d[graph.nOfVertices - 1] = 0;
    
    bool any;
    
    for (int i = 0; i < graph.nOfVertices; ++i) {
        any = false;
        
        // cout << "III " << i << "   ";
        // for (int m = 0; m < graph.nOfVertices; ++m) {
        //     cout << graph.d[m] << " ";
        // }
        // cout < "\n";
        
        for (int j = 0; j < graph.nOfVertices; ++j) {
            for (int k = 0; k < graph.vertices[j].size(); ++k) {
            
                // cout << "i " << i << "; j = " <<  j << "; k == " << k << "; d[j] " << graph.d[j] << "\n";
                // cout << "Is " << j << " v " << graph.vertices[j][k].first << "\n";
                
                if (graph.d[j] < LLONG_MAX) {
                    if (SumNoOverflow(graph.d[j], graph.vertices[j][k].second) < graph.d[graph.vertices[j][k].first]) {
                        if (SumNoOverflow(graph.d[j], graph.vertices[j][k].second) == -LLONG_MAX) {
                            return false;
                        }
                        //cout << "FIRST " << graph.d[j] << " SECOND " << graph.vertices[j][k].second << " SUM " << graph.d[j] + graph.vertices[j][k].second << "\n";
                        graph.d[graph.vertices[j][k].first] = SumNoOverflow(graph.d[j],  graph.vertices[j][k].second) - 1;
                        //cout << "\n                     d[j][k] NEW " << graph.d[graph.vertices[j][k].first] << "; j " << j << "\n\n";
                        any = true;
                        if (i == graph.nOfVertices - 1) {
                            return false;
                        }
                    }
                }
                
                
            }
        }
        if (!any) {
            break;
        }
    }
    graph.vertices.resize(graph.vertices.size() - 1);
    graph.vertices.shrink_to_fit();
    graph.nOfVertices--;
    
    return true;
}


void Dijkstra(TGraph &graph, int start, vector<long long> &vec)
{
    //typedef pair<int, long long> pair<int, long long>;
    priority_queue<pair<int, long long>, vector<pair<int, long long>>, greater<pair<int, long long>> > prQueue;
    
    for (int i = 0; i < graph.nOfVertices; ++i) {
        vec[i] = LLONG_MAX;
    }
    vec[start] = 0;
    prQueue.push(make_pair(0, start));
    vector<pair<int, long long> >::iterator i;
    
    long long active;
    long long tmp;
    long long weight;
    while (!prQueue.empty()) {
        active = prQueue.top().second;
        prQueue.pop();
        
        for (i = graph.vertices[active].begin(); i != graph.vertices[active].end(); ++i) {
            tmp = (*i).first;
            weight = (*i).second;
            
            if (vec[tmp] > vec[active] + weight) {
                vec[tmp] = vec[active] + weight;
                prQueue.push(make_pair(vec[tmp], tmp));
            }
        }
    }
}


void CreateGraph(TGraph &graph)
{
    int vertex1;
    int vertex2;
    long long weight;
    graph.nOfVertices = 0;
    graph.nOfEdge = 0;
    cin >> graph.nOfVertices >> graph.nOfEdge;
    
    graph.nOfVertices++;//for BelmanFord
    graph.vertices.resize(graph.nOfVertices);
    while (cin >> vertex1 >> vertex2 >> weight) {
        graph.vertices[vertex1 - 1].push_back(make_pair(vertex2 - 1, weight));
    }
}


void PrintGraph(TGraph &graph)
{
    for (long long i = 0; i < graph.nOfVertices; ++i) {
        cout << ">  Vertex " << i + 1 << endl;
        for (long long j = 0; j < graph.vertices[i].size(); ++j) {
            cout << j + 1 << ") Vertex    " << graph.vertices[i][j].first + 1 << endl;
            cout << j + 1 << ") Weight    " << graph.vertices[i][j].second << endl;
            cout << "===========" << endl;
        }
    }
}