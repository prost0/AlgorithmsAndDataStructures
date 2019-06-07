#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

int main()
{
    int n, m, maxSquare = 0;// 1...500
    stack<int> stc;
    cin >> n >> m;
    vector < vector<char> > inputRectangle(n, vector<char> (m));
    vector<int> d(m, -1), dLeft(m), dRight(m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cin >> inputRectangle[i][j];
        }
    }
    
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (inputRectangle[i][j] == '1') {
                d[j] = i;
            }
        }
        
        while (!stc.empty()) {
            stc.pop();
        }
        for (int j = 0; j < m; ++j) {
            while (!stc.empty() && d[stc.top()] <= d[j]) {
                stc.pop();
            }
            dLeft[j] = stc.empty() ? -1 : stc.top();
            stc.push(j);
        }
        while (!stc.empty()) {
            stc.pop();
        }
        for (int j = m - 1; j >= 0; --j) {
            while (!stc.empty() && d[stc.top()] <= d[j]) {
                stc.pop();
            }
            dRight[j] = stc.empty() ? m : stc.top();
            stc.push(j);
        }
        for (int j = 0; j < m; ++j) {
            maxSquare = max(maxSquare, (i - d[j]) * (dRight[j] - dLeft[j] - 1));
        }
    }
    
    cout << maxSquare << "\n";
    return 0;
}
