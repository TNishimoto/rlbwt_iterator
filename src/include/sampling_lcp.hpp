#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "rlbwt.hpp"
#include "backward_text.hpp"
//#include "forward_sa.hpp"
#include "sampling_findex_iterator.hpp"

namespace stool
{
namespace rlbwt
{

template <typename CHAR = char>
class MultipleTextPositionIterator
{
  private:
    uint64_t _index = 0;
    uint64_t _distance = 0;

    const RLBWT<CHAR> &_rlbwt;
    std::vector<uint64_t> &_findexes_lorder;
    std::vector<uint64_t> _bottom_indexes;
    //std::vector<uint64_t> _xvec;

  public:
    MultipleTextPositionIterator() = default;
    MultipleTextPositionIterator(std::vector<uint64_t> &indexes, const RLBWT<CHAR> &__rlbwt, std::vector<uint64_t> &__findexes_lorder) : _rlbwt(__rlbwt), _findexes_lorder(__findexes_lorder)
    {
        for (auto p : indexes)
        {
            this->_bottom_indexes.push_back(p);
        }
    }

    void process()
    {
        for (uint64_t i = 0; i < this->_bottom_indexes.size(); i++)
        {
            uint64_t value = this->succ(i);

            this->_bottom_indexes[i] = value;
        }
        uint64_t tmp_i = 0;
        for (uint64_t i = 0; i < this->_bottom_indexes.size(); i++)
        {
            if (this->_bottom_indexes[i] != std::numeric_limits<uint64_t>::max())
            {
                this->_bottom_indexes[tmp_i] = this->_bottom_indexes[i];
                tmp_i++;
            }
        }
        this->_bottom_indexes.resize(tmp_i);
        this->_bottom_indexes.shrink_to_fit();
        this->_distance++;
        this->_index = 0;
    }
    void add(uint64_t pos)
    {
        this->_bottom_indexes.push_back(pos);
    }

    std::pair<uint64_t, uint64_t> operator*()
    {
        std::pair<uint64_t, uint64_t> lindex_pair = _rlbwt.to_rle_lindex(_bottom_indexes[this->_index]);
        return lindex_pair;
        //return _bottom_indexes[this->_index];
    }
    MultipleTextPositionIterator &operator++()
    {
        this->_index++;
        if (this->_index == this->_bottom_indexes.size())
        {
            this->_index = std::numeric_limits<uint64_t>::max();
        }
        return *this;
    }
    uint64_t distance(){
        return this->_distance;
    }
    bool isEnd()
    {
        return this->_index == std::numeric_limits<uint64_t>::max();
    }
    uint64_t size(){
        return this->_bottom_indexes.size();
    }

  private:
    uint64_t succ(uint64_t i)
    {
        std::pair<uint64_t, uint64_t> lindex_pair = _rlbwt.to_rle_lindex(_bottom_indexes[i]);
        if (lindex_pair.second == 0)
        {
            return std::numeric_limits<uint64_t>::max();
        }
        else
        {
            uint64_t new_findex = _findexes_lorder[lindex_pair.first] + lindex_pair.second;
            return new_findex;
        }
    }
};
//std::vector<uint64_t> result_lcp_vec;

template <typename CHAR = char>
class SamplingLCP
{
  private:
  public:
    const RLBWT<CHAR> &_rlbwt;
    std::vector<uint64_t> _findexes_lorder;
    std::vector<uint64_t> _intervals;
    std::vector<uint64_t> _prev_mapper;

    std::vector<uint64_t> _sampling_lcp;
    std::vector<bool> _checker;

    uint64_t current_lcp = 0;

    SamplingLCP(const RLBWT<CHAR> &__rlbwt) : _rlbwt(__rlbwt)
    {

        this->_findexes_lorder = RLBWTFunctions::construct_fpos_array<uint64_t>(__rlbwt);
        _sampling_lcp.resize(_rlbwt.rle_size(), std::numeric_limits<uint64_t>::max());

        std::vector<uint64_t> indexes;
        construct_initial_data(__rlbwt, indexes, this->_intervals, this->_prev_mapper, this->_sampling_lcp);
        _checker.resize(_rlbwt.rle_size() * 2, false);
        loop(indexes);
    }
    static uint64_t getSpecialDistance(uint64_t _rle_lindex, uint64_t _diff)
    {
        return (_rle_lindex * 2) + (_diff == 0 ? 0 : 1);
    }
    void loop(std::vector<uint64_t> &indexes)
    {
        MultipleTextPositionIterator<char> it(indexes, _rlbwt, _findexes_lorder);

        while (this->_intervals.size() > 0)
        {
            std::vector<uint64_t> result;
            std::vector<uint64_t> points;
            while (!it.isEnd())
            {
                //_d_lcp_vec[*it] = it._distance;
                auto p = *it;
                uint64_t pos = SamplingLCP<CHAR>::getSpecialDistance(p.first, p.second);
                if (!this->_checker[pos])
                {
                    points.push_back(pos);
                    this->_checker[pos] = true;
                }

                //search(*it, result, it._distance);
                //assert(_d_lcp_vec[*it] == result_lcp_vec[*it]);
                ++it;
            }
            search(points, result, it.distance() );
            it.process();
            for (auto p : result)
            {
                it.add(p);
            }
            //std::cout << it.size() << std::endl;
            //it.succ_process(result);
        }
    }
    bool search(std::vector<uint64_t> &points, std::vector<uint64_t> &result, uint64_t distance)
    {
        std::sort(points.begin(), points.end());
        std::vector<uint64_t> tmp;
        for (auto &it : this->_intervals)
        {
            uint64_t left = SamplingLCP<CHAR>::getSpecialDistance(_prev_mapper[it], 1);
            uint64_t right = SamplingLCP<CHAR>::getSpecialDistance(it, 0);

            auto pointer = std::upper_bound(points.begin(), points.end(), left);
            uint64_t next = pointer != points.end() ? *pointer : std::numeric_limits<uint64_t>::max();
            if (next <= right)
            {
                result.push_back(_findexes_lorder[it]);
                _sampling_lcp[it] = distance + 1;
            }
            else
            {
                tmp.push_back(it);
            }
        }
        bool b = this->_intervals.size() != tmp.size();

        std::swap(this->_intervals, tmp);
        return b;
    }

    static void construct_initial_data(const RLBWT<CHAR> &_rlbwt, std::vector<uint64_t> &output_indexes, std::vector<uint64_t> &output_intervals, std::vector<uint64_t> &output_prev_mapper, std::vector<uint64_t> &output_sampling_lcp)
    {
        SamplingFIndexGenerator<CHAR, uint64_t, RLBWT<CHAR>> generator(_rlbwt);
        
        output_prev_mapper.resize(_rlbwt.rle_size());

        uint64_t prev_c = std::numeric_limits<uint64_t>::max();
        uint64_t prev_rle_lindex = std::numeric_limits<uint64_t>::max();

        for (SamplingFIndexIterator<CHAR, uint64_t, RLBWT<CHAR>> it = generator.begin(); it != generator.end(); ++it)
        {

            uint64_t rle_findex = it.rle_findex();
            uint64_t rle_lindex = it.rle_lindex();

            uint64_t c = it.character();

            if (rle_findex == 0 || prev_c != c)
            {
                output_sampling_lcp[rle_lindex] = 0;
                uint64_t findex = it.findex();
                output_indexes.push_back(findex);
                output_prev_mapper[rle_lindex] = std::numeric_limits<uint64_t>::max();
            }
            else
            {
                output_intervals.push_back(rle_lindex);
                output_prev_mapper[rle_lindex] = prev_rle_lindex;
            }
            prev_c = c;
            prev_rle_lindex = rle_lindex;
        }
    }
    static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT<CHAR> &__rlbwt)
    {
        SamplingLCP slcp(__rlbwt);
        return slcp._sampling_lcp;
    }

    static std::vector<uint64_t> construct_sampling_lcp_array(const RLBWT<CHAR> &__rlbwt, std::vector<uint64_t> &__sampling_end_sa)
    {
        std::vector<uint64_t> r = construct_sampling_lcp_array_lorder(__rlbwt);
        return to_succ_sampling_lcp_array_yorder(std::move(r), __rlbwt, __sampling_end_sa);
    }
    static std::vector<uint64_t> to_succ_sampling_lcp_array_yorder(std::vector<uint64_t> &&slcp_lorder, const RLBWT<CHAR> &__rlbwt, std::vector<uint64_t> &__sampling_end_sa)
    {
        std::vector<uint64_t> lf = RLBWTFunctions::construct_rle_lf_mapper<uint64_t>(__rlbwt);
        std::vector<uint64_t> slcp_forder = stool::rlbwt::permutate(std::move(slcp_lorder), lf);
        lf.resize(0);
        lf.shrink_to_fit();

        //std::vector<uint64_t> yf = construct_yf_mapper(__rlbwt,__sampling_end_sa);
        std::vector<uint64_t> yf = stool::rlbwt::get_sorted_positions(__sampling_end_sa);
        std::vector<uint64_t> succ_slcp = stool::rlbwt::rotate(std::move(slcp_forder));
        std::vector<uint64_t> fy = stool::rlbwt::change_inv(std::move(yf));
        std::vector<uint64_t> succ_slcp_yorder = stool::rlbwt::permutate(std::move(succ_slcp), fy);
        return succ_slcp_yorder;
        //to_ylcp(slcp_lorder, yf);
    }

    /*
    static void to_ylcp(std::vector<uint64_t> &slcp, std::vector<uint64_t> &yf)
    {
        uint64_t rle_size = yf.size();
        std::vector<uint64_t> tmp;
        tmp.resize(rle_size, 0);

        for (uint64_t i = 0; i < rle_size; i++)
        {
            tmp[i] = yf[i] + 1 == rle_size ? std::numeric_limits<uint64_t>::max() : slcp[yf[i] + 1];
        }
        std::swap(tmp, slcp);
    }
    */
};
} // namespace rlbwt
} // namespace stool