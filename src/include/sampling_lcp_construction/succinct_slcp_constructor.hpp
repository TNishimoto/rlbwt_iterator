#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "./multiple_text_position_iterator.hpp"
#include "./wt_select_on_rlbwt.hpp"

#include "./succinct_interval_tree.hpp"
#include "stool/src/debug.hpp"
#include <sdsl/wavelet_trees.hpp>

namespace stool
{
namespace rlbwt
{

class RLBWTLeftIntervals
{
public:
    RLBWTLeftIntervals()
    {
    }
    uint64_t operator[](uint64_t i) const
    {
        return ((i + 1) * 2);
    }
};

template <typename SELECTER>
class RLBWTRightIntervals
{
public:
    SELECTER *selecter;
    //std::vector<RLE_SIZE_TYPE> *next_c_index_vector;
    RLBWTRightIntervals(SELECTER *_selecter) : selecter(_selecter)
    {
    }
    uint64_t operator[](uint64_t i) const
    {
        uint64_t x = selecter->get_next_c_index(i);
        return x * 2;
    }
};

template <typename RLBWT_STR>
class SuccinctSLCPConstructor
{
private:
public:
    using CHAR = typename RLBWT_STR::char_type;
    /*
    const RLBWT_STR &_rlbwt;
    static std::vector<uint64_t> compute_slcp_array_on_L(const RLBWT_STR &__rlbwt)
    {
        //std::vector<uint64_t> previous_c_index_vector = construct_initial_data(__rlbwt);
        std::vector<uint64_t> __sampling_lcp_array_on_L = loop(__rlbwt);
        return __sampling_lcp_array_on_L;
    }
    */
    static uint64_t getSpecialDistance(uint64_t _rle_lindex, uint64_t _diff)
    {
        return (_rle_lindex * 2) + (_diff == 0 ? 0 : 1);
    }
    template <typename SELECTER, typename FINDEXES_VEC>
    static std::vector<uint64_t> construct_zero_lcp_findexes(SELECTER &selecter, FINDEXES_VEC &_findexes_lorder)
    {
        std::vector<uint64_t> zero_lcp_findexes;
        for (uint64_t i = 0; i < selecter.size(); i++)
        {
            if (selecter.get_first_c_flag(i))
            {
                zero_lcp_findexes.push_back(_findexes_lorder[i]);
            }
        }
        return zero_lcp_findexes;
    }

    template <typename SELECTER>
    static std::vector<bool> construct_interval_flag_vec(SELECTER &selecter)
    {
        std::vector<bool> interval_flag_vec;
        interval_flag_vec.resize(selecter.size(), false);
        for (uint64_t i = 0; i < selecter.size(); i++)
        {
            interval_flag_vec[i] = selecter.get_next_c_index(i) != selecter.null_flag();
        }

        return interval_flag_vec;
    }

    template <typename RLE_SIZE_TYPE, typename SELECTER, typename FINDEXES_VEC>
    static std::vector<uint64_t> compute_slcp_array_on_L(const RLBWT_STR &_rlbwt, SELECTER &selecter, FINDEXES_VEC &_findexes_lorder)
    {
        std::cout << "memory(selecter):" << selecter.get_using_memory() << " bytes" << std::endl;
        uint64_t run_size = _rlbwt.rle_size();
        using UNSIGNED_CHAR = typename make_unsigned<typename RLBWT_STR::CHAR>::type;

        std::vector<bool> _access_checker;
        _access_checker.resize(run_size * 2, false);

        //

        std::vector<bool> interval_flag_vec = construct_interval_flag_vec(selecter);
        std::vector<uint64_t> zero_lcp_findexes = construct_zero_lcp_findexes(selecter, _findexes_lorder);
        std::cout << "memory(zero_lcp_findexes): " << (zero_lcp_findexes.size() * sizeof(uint64_t) )  << " bytes" << std::endl;

        RLBWTLeftIntervals left_intervals;
        RLBWTRightIntervals<SELECTER> right_intervals(&selecter);
        SuccinctIntervalTree<RLE_SIZE_TYPE, UNSIGNED_CHAR, RLBWTLeftIntervals, RLBWTRightIntervals<SELECTER>> intervalTree;
        intervalTree.initialize(getSpecialDistance(run_size, 1));
        intervalTree.construct(&left_intervals, &right_intervals, interval_flag_vec);

        std::cout << "memory(intervalTree): " << intervalTree.get_using_memory() << " bytes" << std::endl;

        //Printer::print(zero_lcp_findexes);

        MultipleTextPositionIterator<RLBWT_STR, FINDEXES_VEC> findex_iterator(zero_lcp_findexes, _rlbwt, _findexes_lorder);

        uint64_t interval_count = run_size - zero_lcp_findexes.size();
        uint64_t nokori_counter = interval_count;

        std::vector<uint64_t> __sampling_lcp_array_on_L;
        __sampling_lcp_array_on_L.resize(run_size, 0);
        std::cout << "memory(__sampling_lcp_array_on_L): " << (__sampling_lcp_array_on_L.size() * sizeof(uint64_t) )  << " bytes" << std::endl;
        std::vector<uint64_t> reportedIndexes;
        while (nokori_counter > 0)
        {
            reportedIndexes.clear();
            //std::cout << "nit size:" << nit.items.size() << std::endl;
            //std::cout << findex_iterator.size() << ", " << std::flush;
            while (!findex_iterator.isEnd())
            {
                auto current_rle_findex = *findex_iterator;
                uint64_t pos = getSpecialDistance(current_rle_findex.first, current_rle_findex.second);
                if (!_access_checker[pos])
                {
                    auto r = intervalTree.report_and_remove(pos);
                    for (auto rep : r)
                    {
                        nokori_counter--;
                        reportedIndexes.push_back(selecter.get_next_c_index(rep));
                    }

                    _access_checker[pos] = true;
                }

                ++findex_iterator;
            }
            uint64_t distance = findex_iterator.distance();
            findex_iterator.process();
            for (auto p : reportedIndexes)
            {
                __sampling_lcp_array_on_L[p] = distance + 1;
                findex_iterator.add(_findexes_lorder[p]);
            }
        }

        return __sampling_lcp_array_on_L;
    }
    static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &_rlbwt, bool use_succinct_data_structure)
    {
        uint64_t run_size = _rlbwt.rle_size();
        if (use_succinct_data_structure)
        {
            using INT_VECTOR = sdsl::int_vector<sizeof(typename RLBWT_STR::CHAR) * 8>;
            using UNSIGNED_CHAR = typename make_unsigned<typename RLBWT_STR::CHAR>::type;

            using WT = sdsl::wt_huff<>;
            using SELECTER = WTSelectOnRLBWT<UNSIGNED_CHAR, WT>;
            //using UNSIGNED_CHAR = make_unsigned<typename RLBWT_STR::CHAR>;
            INT_VECTOR chars;
            //sdsl::int_vector<> chars;
            chars.resize(run_size);
            for (uint64_t i = 0; i < run_size; i++)
            {
                chars[i] = (UNSIGNED_CHAR)_rlbwt.get_char_by_run_index(i);
            }
            WT wt;
            construct_im(wt, chars);
            SELECTER wts;
            wts.set(&wt);

            //auto _findexes_lorder = RLBWTFunctions::construct_fpos_array(_rlbwt);
            SuccinctFIndexesLOrder<UNSIGNED_CHAR, WT> _findexes_lorder2;
            _findexes_lorder2.build(_rlbwt, &wt);
            /*
            for(uint64_t i=0;i<run_size;i++){
                std::cout << (int)_rlbwt.get_char_by_run_index(i) << "/"<< _findexes_lorder[i] << "/" << _findexes_lorder2[i] << std::endl;
                assert(_findexes_lorder[i] == _findexes_lorder2[i]);
            }
            */


            if (run_size < UINT16_MAX)
            {
                return compute_slcp_array_on_L<uint16_t>(_rlbwt, wts, _findexes_lorder2);
            }
            else if (run_size < UINT32_MAX)
            {
                return compute_slcp_array_on_L<uint32_t>(_rlbwt, wts, _findexes_lorder2);
            }
            else
            {
                return compute_slcp_array_on_L<uint64_t>(_rlbwt, wts, _findexes_lorder2);
            }
        }
        else
        {
            SelectOnRLBWT<uint64_t> selecter;
            selecter.build(_rlbwt);
            auto _findexes_lorder = RLBWTFunctions::construct_fpos_array(_rlbwt);

            std::cout << "memory(_findexes_lorder): " << (_findexes_lorder.size() * sizeof(uint64_t)) << " bytes"<< std::endl;
            if (run_size < UINT16_MAX)
            {
                return compute_slcp_array_on_L<uint16_t>(_rlbwt, selecter, _findexes_lorder);
            }
            else if (run_size < UINT32_MAX)
            {
                return compute_slcp_array_on_L<uint32_t>(_rlbwt, selecter, _findexes_lorder);
            }
            else
            {
                return compute_slcp_array_on_L<uint64_t>(_rlbwt, selecter, _findexes_lorder);
            }
        }
    }
};

} // namespace rlbwt
} // namespace stool