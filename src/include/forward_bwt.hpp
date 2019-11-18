#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
//#include "common/io.h"
//#include "common/print.hpp"
#include "other_functions.hpp"
#include "OnlineRlbwt/online_rlbwt.hpp"
//using namespace std;

namespace stool
{
namespace rlbwt
{

template <typename CHAR = char, typename INDEX = uint64_t, typename CHARVEC = std::vector<CHAR>, typename POWVEC = std::vector<INDEX>>
class ForwardBWT
{
public:
    class iterator
    {
    private:
        const CHARVEC *_char_vec = nullptr;
        const POWVEC *_run_vec = nullptr;
        //RLBWT *_rlbwt;
        INDEX _char_index = 0;
        INDEX _run_index = 0;

    public:
        iterator(CHARVEC *__char_vec, POWVEC *__run_vec, bool _isBegin) : _char_vec(__char_vec), _run_vec(__run_vec)
        {
            if (!_isBegin)
            {
                this->_run_index = _run_vec->size();
            }
        }

        iterator &operator++()
        {
            ++_char_index;
            if ((*this->_run_vec)[this->_run_index] == _char_index)
            {
                _char_index = 0;
                ++_run_index;
            }
            return *this;
        }
        CHAR operator*()
        {
            return (*this->char_vec)[this->_run_index];
        }
        bool operator!=(const iterator &rhs) const
        {
            return (_run_index != rhs._run_index) || (_char_index != rhs._char_index);
        }
    };
};

private:
const CHARVEC *_char_vec = nullptr;
const POWVEC *_run_vec = nullptr;

public:
ForwardBWT(CHARVEC *__char_vec, POWVEC *__run_vec) : _char_vec(__char_vec), _run_vec(__run_vec)
{
}
auto begin() const -> iterator
{

    auto it = iterator(this->_char_vec, this->_run_vec, true);
    return it;
}

auto end() const -> iterator
{
    auto it = iterator(this->_char_vec, this->_run_vec, false);
    return it;
}
} // namespace rlbwt
} // namespace stool