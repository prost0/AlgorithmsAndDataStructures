//#include "big_integer.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <vector>

class TBigInteger
{
public:
    TBigInteger();
    TBigInteger(int num);
    TBigInteger(const char *str);
    
    void Print() const;
    
    TBigInteger operator + (const TBigInteger &bi) const;
    TBigInteger operator - (const TBigInteger &bi) const;
    TBigInteger operator * (const TBigInteger &bi) const;
    TBigInteger operator / (const TBigInteger &bi) const;
    TBigInteger operator ^ (const TBigInteger &bi) const;
    
    bool operator < (const TBigInteger &bi) const;
    bool operator > (const TBigInteger &bi) const;
    bool operator == (const TBigInteger &bi) const;
    
private:
    std::vector<short> mNums;
    static const short mBASE;
    static const short mBASEPOW;
    
    void DeleteExtraZeros();
    TBigInteger mPower(const TBigInteger &bi) const;
};
///////////////////////////////////////////////////////////

const short TBigInteger::mBASE = 10000;//10000
const short TBigInteger::mBASEPOW = 4;

TBigInteger::TBigInteger()
{
    mNums.push_back(0);
}

TBigInteger::TBigInteger(int num)
{
    if (num == 0) {
        mNums.push_back(0);
    } else {
        while (num != 0) {
            mNums.push_back(num % mBASE);
            num /= mBASE;
        }
    }
}

TBigInteger::TBigInteger(const char *str)
{
    int len = strlen(str);
    
    for (int i = len; i > 0; i -= mBASEPOW) {
        short num = 0;
        
        for (int j = (i - mBASEPOW > 0 ? i - mBASEPOW : 0); j < i; ++j) {
            num = num * 10 + str[j] - '0';
        }
        
        mNums.push_back(num);
    }
    
    DeleteExtraZeros();
}

void TBigInteger::Print() const
{
    int size = mNums.size();
    
    printf("%d", mNums[size - 1]);
    
    for (int i = size - 2; i >= 0; --i) {
        printf("%04d", mNums[i]);
    }
    
    printf("\n");
}

TBigInteger TBigInteger::operator + (const TBigInteger &bi) const
{
    short carry = 0;
    int maxSize = mNums.size() > bi.mNums.size() ? mNums.size() : bi.mNums.size();
    TBigInteger res;
    
    res.mNums.pop_back();
    
    for (int i = 0; i < maxSize; ++i) {
        short x = i < mNums.size() ? mNums[i] : 0;
        short y = i < bi.mNums.size() ? bi.mNums[i] : 0;
        short tmp = x + y + carry;
        
        carry = tmp / mBASE;
        res.mNums.push_back(tmp % mBASE);
    }
    
    if (carry != 0) {
        res.mNums.push_back(carry);
    }
    
    return res;
}

TBigInteger TBigInteger::operator - (const TBigInteger &bi) const
{
    short carry = 0;
    TBigInteger res;
    
    res.mNums.pop_back();
    
    for (int i = 0; i < mNums.size(); ++i) {
        short y = i < bi.mNums.size() ? bi.mNums[i] : 0;
        short tmp = mNums[i] - y + carry;
        
        carry = tmp < 0 ? -1 : 0;
        res.mNums.push_back(tmp < 0 ? tmp + mBASE : tmp);
    }
    
    res.DeleteExtraZeros();
    
    return res;
}

TBigInteger TBigInteger::operator * (const TBigInteger &bi) const
{
    int m = mNums.size();
    int n = bi.mNums.size();
    int maxSize = m > n ? m : n;
    TBigInteger res;
    
    res.mNums.resize(maxSize * 2, 0);
    
    for (int j = 0; j < n; ++j) {
        if (bi.mNums[j] != 0) {
            short carry = 0;
            
            for (int i = 0; i < m; ++i) {
                int tmp = mNums[i] * bi.mNums[j] + res.mNums[i + j] + carry;
                
                carry = tmp / mBASE;
                res.mNums[i + j] = tmp % mBASE;
            }
            
            res.mNums[j + m] = carry;
        }
    }
    
    res.DeleteExtraZeros();
    
    return res;
}

TBigInteger TBigInteger::operator / (const TBigInteger &bi) const
{
    int norm = mBASE / (bi.mNums.back() + 1);
    
    TBigInteger a = *this * norm;
    TBigInteger b = bi * norm;
    TBigInteger q;
    TBigInteger r;
    
    q.mNums.resize(a.mNums.size());
    
    for (int i = a.mNums.size() - 1; i >= 0; --i) {
        r = r * mBASE;
        r = r + a.mNums[i];
        
        short s1 = r.mNums.size() <= b.mNums.size() ? 0 : r.mNums[b.mNums.size()];
        short s2 = r.mNums.size() <= b.mNums.size() - 1 ? 0 : r.mNums[b.mNums.size() - 1];
        short d = static_cast<short>((static_cast<int>(s1) * mBASE + s2) / b.mNums.back());
        
        TBigInteger tmp = b * d;
        
        while (tmp > r) {
            tmp = tmp - b;
            --d;
        }
        
        r = r - tmp;
        q.mNums[i] = d;
    }
    
    q.DeleteExtraZeros();
    
    return q;
}

TBigInteger TBigInteger::operator ^ (const TBigInteger &bi) const
{
    return mPower(bi);
}

bool TBigInteger::operator < (const TBigInteger &bi) const
{
    if (mNums.size() != bi.mNums.size()) {
        return mNums.size() < bi.mNums.size();
    }
    
    for (int i = mNums.size() - 1; i >= 0; --i) {
        if (mNums[i] != bi.mNums[i]) {
            return mNums[i] < bi.mNums[i];
        }
    }
    
    return false;
}

bool TBigInteger::operator > (const TBigInteger &bi) const
{
    return bi < *this;
}

bool TBigInteger::operator == (const TBigInteger &bi) const
{
    return !(*this < bi) && !(bi < *this);
}

void TBigInteger::DeleteExtraZeros()
{
    while (mNums.size() > 1 && mNums.back() == 0) {
        mNums.pop_back();
    }
}

TBigInteger TBigInteger::mPower(const TBigInteger &b) const
{
    if (b == 0) {
        return 1;
    }
    
    if (b.mNums[0] & 1) {
        return *this * mPower(b - 1);
    }
    
    TBigInteger c = mPower(b / 2);
    
    return c * c;
}


int main()
{
    char arg[100001];
    
    while (scanf("%s", arg) == 1) {
        TBigInteger a(arg);
        
        if (scanf("%s", arg) != 1) {
            printf("ERROR\n");
            
            return 0;
        }
        
        TBigInteger b(arg);
        TBigInteger c;
        
        if (scanf("%s", arg) != 1) {
            printf("ERROR\n");
            
            return 0;
        }
        
        switch (arg[0]) {
            case '+': {
                    c = a + b;
                    c.Print();
                    
                    break;
                }
                
            case '-': {
                    if (a < b) {
                        printf("Error\n");
                    } else {
                        c = a - b;
                        c.Print();
                    }
                    
                    break;
                }
                
            case '*': {
                    c = a * b;
                    c.Print();
                    
                    break;
                }
                
            case '/': {
                    if (b == 0) {
                        printf("Error\n");
                    } else {
                        c = a / b;
                        c.Print();
                    }
                    
                    break;
                }
                
            case '^': {
                    if (a == 0 && b == 0) {
                        printf("Error\n");
                    } else if (a == 0) {
                        printf("0\n");
                    } else {
                        c = a ^ b;
                        c.Print();
                    }
                    
                    break;
                }
                
            case '<': {
                    printf("%s\n", a < b ? "true" : "false");
                    
                    break;
                }
                
            case '>': {
                    printf("%s\n", a > b ? "true" : "false");
                    
                    break;
                }
                
            case '=': {
                    printf("%s\n", a == b ? "true" : "false");
                    
                    break;
                }
        }
    }
    
    return 0;
}
