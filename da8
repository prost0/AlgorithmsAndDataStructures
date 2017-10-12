#include <iostream>
#include <vector>
#include <stack>
using namespace std;
int main()
{
    long long n, p, curCoin = 1, m;
    cin >> n >> p >> m;
    vector <long long> coins;
    vector <int> answer(n);
    for (int i = 0; i < n; ++i) {
        coins.push_back(curCoin);
        curCoin *= p;
    }
    for (int i = n - 1; i >= 0; --i) {
        answer[i] = m / coins[i];
        m = m % coins[i];
    }
    for (int i = 0; i < n; ++i) {
        cout << answer[i] << "\n";
    }
    return 0;
}
