#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <algorithm>
#include "bwt.hpp"


namespace stool
{
    namespace rlbwt{

template <typename INDEX = uint64_t>
class LCPInterval
{
public:
    INDEX i;
    INDEX j;
    INDEX lcp;
    LCPInterval(){

    }
    LCPInterval(INDEX _i, INDEX _j, INDEX _lcp){
    this->i = _i;
    this->j = _j;
    this->lcp = _lcp;
    }



    std::string to_string() const{
            return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(lcp) + "]";
    }
    /*
    string operator*()
    {
        return this->toString();
    }
    */
    //bool check(int_vector<> &text, vector<uint64_t> &sa);
    //static void print(int_vector<> &text, vector<uint64_t> &sa, vector<LCPInterval> &intervals);

    bool operator<(const LCPInterval &right) const
    {
        if (this->i == right.i)
        {
            if (this->j == right.j)
            {
                return this->lcp < right.lcp;
            }
            else
            {
                return this->j < right.j;
            }
        }
        else
        {
            return this->i < right.i;
        }
    }
    bool operator==(const LCPInterval &rhs) const
    {
        const LCPInterval &lhs = *this;
        bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
        return b;
    }
    bool operator!=(const LCPInterval &rhs) const
    {
        const LCPInterval &lhs = *this;
        bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
        return !b;
    }
    bool isInternal(const LCPInterval &rhs) const
    {
        return rhs.i <= this->i && this->j <= rhs.j;
    }
    bool bottom_up_less_comp(const LCPInterval &rhs) const
    {
        if ((*this) == rhs)
            return false;

        if (this->isInternal(rhs))
        {
            return true;
        }
        else if (rhs.isInternal(*this))
        {
            return false;
        }
        else
        {
            return this->j < rhs.i;
        }
    }

    bool containsPosition(std::vector<INDEX> &sa, INDEX pos) const
    {
        for (INDEX x = this->i; x <= this->j; x++)
        {
            if (sa[x] <= pos && pos <= sa[x] + this->lcp - 1)
            {
                return true;
            }
        }
        return false;
    }
    static std::vector<LCPInterval> createLCPIntervals(std::vector<INDEX> &sa, std::vector<INDEX> &lcp)
    {

        //assert(sa.size() == lcp.size());
        std::vector<LCPInterval> intervals;
        std::stack<LCPInterval> stack;
        int64_t n = sa.size();
        for (int64_t i = 0; i < n; i++)
        {
            LCPInterval fst = LCPInterval(i, i, n - sa[i]);
            while (stack.size() > 1)
            {
                LCPInterval second = stack.top();
                if (lcp[second.i] > lcp[fst.i])
                {
                    stack.pop();
                    uint64_t newLCP = lcp[second.i];
                    uint64_t newj = second.j;
                    uint64_t newi = second.i;

                    while (stack.size() > 0)
                    {
                        LCPInterval third = stack.top();
                        newi = third.i;
                        stack.pop();
                        if (lcp[second.i] != lcp[third.i])
                        {
                            break;
                        }
                    }
                    LCPInterval newInterval(newi, newj, newLCP);
                    stack.push(newInterval);
                    intervals.push_back(newInterval);
                }
                else
                {
                    break;
                }
            }
            intervals.push_back(fst);

            stack.push(fst);
        }
        while (stack.size() > 1)
        {
            LCPInterval second = stack.top();
            stack.pop();
            uint64_t newLCP = lcp[second.i];
            uint64_t newj = second.j;
            uint64_t newi = second.i;
            while (stack.size() > 0)
            {
                LCPInterval third = stack.top();
                newi = third.i;
                stack.pop();
                if (lcp[second.i] != lcp[third.i])
                {
                    break;
                }
            }
            LCPInterval newInterval(newi, newj, newLCP);
            stack.push(newInterval);
            intervals.push_back(newInterval);
        }
        return intervals;
    }
    static LCPInterval naive_create_lcp_intervals(std::string &text, std::vector<INDEX> &sa, std::vector<INDEX> &lcp, INDEX i, INDEX j)
    {
        //INDEX x = 0;
        //INDEX max = std::numeric_limits<INDEX>::max();
        if (j - i == 0)
        {
            return LCPInterval(i, j, text.size() - sa[i] + 1);
        }
        else
        {
            INDEX x = std::numeric_limits<INDEX>::max();
            for (INDEX k = i + 1; k <= j; k++)
            {
                INDEX lcpv = lcp[k];
                x = x < lcpv ? x : lcpv;
            }
            return LCPInterval(i, j, x);
        }
    }
    static std::vector<LCPInterval> naive_create_lcp_intervals(std::string &text, std::vector<INDEX> &sa, std::vector<INDEX> &lcp)
    {
        std::vector<LCPInterval> r;
        for (INDEX i = 0; i < sa.size(); i++)
        {
            for (INDEX j = i; j < sa.size(); j++)
            {
                LCPInterval candidate = naive_create_lcp_intervals(text, sa, lcp, i, j);
                LCPInterval left = i == 0 ? LCPInterval(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max()) : naive_create_lcp_intervals(text, sa, lcp, i - 1, j);
                LCPInterval right = j == text.size() - 1 ? LCPInterval(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max()) : naive_create_lcp_intervals(text, sa, lcp, i, j + 1);
                if (candidate.lcp != left.lcp && candidate.lcp != right.lcp)
                {
                    r.push_back(candidate);
                }
            }
        }
        /*
        vector<LCPInterval> r2;
        for (INDEX i = 0; i < r.size(); i++)
        {
            bool b = true;
            for (INDEX j = 0; j < r.size(); j++)
            {
                if (i != j && r[i].lcp == r[j].lcp)
                {
                    if (r[i].isInternal(r[j]))
                    {
                        b = false;
                        break;
                    }
                }
            }
            if (b)
            {
                r2.push_back(r[i]);
            }
        }
        */
        //INDEX n = 0;
        sort(r.begin(),
             r.end(),
             [&](const LCPInterval &a, const LCPInterval &b) {
                 return a.bottom_up_less_comp(b);
             });

        return r;
    }

};
    }
} // namespace stool
/*
namespace std
{
template <>
struct hash<stool::LCPInterval<uint64_t>>
{
    std::size_t operator()(stool::LCPInterval<uint64_t> const &key) const
    {
        return ((1234567891234ull) * key.i) ^ ((789000001234ull) * key.j) ^ key.lcp;
    }
};
}
*/