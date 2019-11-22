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

//template <typename CHAR = char, typename INDEX = uint64_t, typename CHARVEC = std::vector<char>, typename POWVEC = std::vector<uint64_t >>
template <typename CHARVEC = std::vector<char>, typename POWVEC = std::vector<uint64_t> >
class ForwardBWT
{
    using CHAR = typename CHARVEC::value_type;
    using INDEX = typename POWVEC::value_type;

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
        iterator(const CHARVEC *__char_vec, const POWVEC *__run_vec, bool _isBegin) : _char_vec(__char_vec), _run_vec(__run_vec)
        {
            if (!_isBegin)
            {
                this->_run_index = _char_vec->size();
            }
        }

        iterator &operator++()
        {
            ++_char_index;
            INDEX current_run = this->get_run(_run_index);
            if (current_run == _char_index)
            {
                _char_index = 0;
                ++_run_index;
            }
            return *this;
        }
        CHAR operator*()
        {
            return (*this->_char_vec)[this->_run_index];
        }
        bool operator!=(const iterator &rhs) const
        {
            return (_run_index != rhs._run_index) || (_char_index != rhs._char_index);
        }

    INDEX get_run(INDEX i) const
    {
        return (*_run_vec)[i + 1] - (*_run_vec)[i];
    }
    };
    private:
const CHARVEC *_char_vec = nullptr;
const POWVEC *_run_vec = nullptr;

public:
ForwardBWT(const RLBWT<CHARVEC,POWVEC> *__rlbwt) : _char_vec(__rlbwt->get_char_vec() ) , _run_vec(__rlbwt->get_run_vec() )
{
}
/*
void set(const CHARVEC *__char_vec,const POWVEC *__run_vec){

}
*/
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
};


} // namespace rlbwt
} // namespace stool