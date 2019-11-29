#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace stool
{
namespace rlbwt
{
class CartesianTree
{
public:
    template <typename T>
    static T construct(std::vector<T> &values, std::vector<T> &left, std::vector<T> &right)
    {
        left.resize(values.size(), UINT64_MAX);
        right.resize(values.size(), UINT64_MAX);

        uint64_t min_index = 0;

        std::stack<uint64_t> st;
        uint64_t count = values.size();
        for (uint64_t i = 0; i < count; ++i)
        {
            if(values[i] < values[min_index]){
                min_index = i;
            }
            //uint64_t node = new CartesianTreeNode(array[i], i)
            uint64_t last = UINT64_MAX;
            while (st.size() > 0)
            {
                
                uint64_t top = st.top();
                if (values[top] >= values[i])
                {
                    last = top;
                    st.pop();
                }
                else
                {
                    break;
                }
            }
            if (last != UINT64_MAX)
            {
                left[i] = last;
            }
            if (st.size() > 0)
            {
                uint64_t x = st.top();
                right[x] = i;
            }
            st.push(i);
        }
        return min_index;
    }
};

} // namespace rlbwt
} // namespace stool