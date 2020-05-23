#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>

#include "./range_distinct.hpp"
#include "stool/src/debug.hpp"

namespace stool
{
    namespace rlbwt
    {

        template <typename RLBWT_STR, typename INDEX_SIZE = uint64_t>
        class Weiner
        {
        private:
            using CHAR = typename RLBWT_STR::char_type;
            using CHAR_VEC = typename RLBWT_STR::char_vec_type;
            using LPOS = std::pair<uint64_t, uint64_t>;

            const RLBWT_STR &_rlbwt;
            std::queue<WeinerInterval<INDEX_SIZE>> queue;
            std::vector<bool> checkerArray;
            std::vector<uint64_t> fposArray;
            std::vector<uint64_t> frunStartingPositionMapperArray;

            //std::vector<uint64_t> lf_mapper;
            RangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> range_distinct_data_structure;

            uint64_t current_length = 0;
            uint64_t str_size;

        public:
            Weiner(const RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt)
            {
                this->str_size = _rlbwt.str_size();
                auto v1 = RLBWTFunctions::construct_fpos_array(_rlbwt);
                this->fposArray.swap(v1);
                this->checkerArray.resize(_rlbwt.rle_size(), false);

                auto v2 = RLBWTFunctions2::construct_rle_lf_lorder(_rlbwt);
                this->frunStartingPositionMapperArray.swap(v2);

                this->queue.push(WeinerInterval<INDEX_SIZE>::get_special());

                range_distinct_data_structure.preprocess(_rlbwt.get_char_vec());
            }
            vector<WeinerInterval<INDEX_SIZE>> computeFirstWeinerIntervals()
            {
                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = _rlbwt.rle_size() - 1;
                uint64_t end_diff = _rlbwt.get_run(end_lindex) - 1;
                //return this->naiveWeinerQuery(begin_lindex, begin_diff, end_lindex, end_diff);
                return RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE>::range_distinct(_rlbwt, range_distinct_data_structure, begin_lindex, begin_diff, end_lindex, end_diff);
            }
            std::vector<LPOS> compute_next_lcp_indexes()
            {
                std::vector<LPOS> r;

                if (this->current_length == 0)
                {
                    this->queue.pop();
                    vector<WeinerInterval<INDEX_SIZE>> vec = this->computeFirstWeinerIntervals();
                    for (auto it : vec)
                    {
                        //uint64_t end_pos = this->fposArray[it.endIndex] + it.endDiff;
                        r.push_back(LPOS(it.endIndex, it.endDiff));
                        this->checkerArray[it.endIndex] = true;

                        this->queue.push(it);
                    }
                    this->queue.push(WeinerInterval<INDEX_SIZE>::get_special());
                }
                else
                {
                    while (true)
                    {
                        WeinerInterval<INDEX_SIZE> front = this->queue.front();
                        //front.print2(this->fposArray);
                        if (front.is_special())
                        {
                            this->queue.pop();
                            this->queue.push(WeinerInterval<INDEX_SIZE>::get_special());
                            break;
                        }
                        else
                        {
                            uint64_t begin_pos = this->fposArray[front.beginIndex] + front.beginDiff;
                            //uint64_t begin_lindex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                            uint64_t begin_lindex = _rlbwt.get_lindex_containing_the_position_with_linear_search(begin_pos, this->frunStartingPositionMapperArray[front.beginIndex]);
                            uint64_t begin_diff = begin_pos - _rlbwt.get_lpos(begin_lindex);

                            uint64_t end_pos = this->fposArray[front.endIndex] + front.endDiff;
                            //uint64_t end_lindex = _rlbwt.get_lindex_containing_the_position(end_pos);
                            uint64_t end_lindex = _rlbwt.get_lindex_containing_the_position_with_linear_search(end_pos, this->frunStartingPositionMapperArray[front.endIndex]);

                            uint64_t end_diff = end_pos - _rlbwt.get_lpos(end_lindex);

                            //vector<WeinerInterval> result = this->naiveWeinerQuery(begin_lindex, begin_diff, end_lindex, end_diff);
                            vector<WeinerInterval<INDEX_SIZE>> result = RangeDistinctDataStructureOnRLBWT<RLBWT_STR,INDEX_SIZE>::range_distinct(_rlbwt, range_distinct_data_structure, begin_lindex, begin_diff, end_lindex, end_diff);
                            if (begin_lindex == end_lindex)
                            {
                                king_counter++;
                            }
                            if (front.beginIndex == front.endIndex)
                            {
                                king_counter2++;
                            }
                            for (auto it : result)
                            {
                                //uint64_t end_pos = this->fposArray[it.endIndex] + it.endDiff;
                                bool b = _rlbwt.get_run(it.endIndex) == (it.endDiff + 1);
                                if (!b || !this->checkerArray[it.endIndex])
                                {
                                    r.push_back(LPOS(it.endIndex, it.endDiff));

                                    if (b){
                                        //std::cout << "i = " << it.endIndex << "/[" << begin_lindex << ", " << begin_diff << "][" << end_lindex << ", " << end_diff << "]" << std::endl;
                                        this->checkerArray[it.endIndex] = true;

                                    }

                                    this->queue.push(it);
                                }
                            }
                            this->queue.pop();
                        }
                    }
                }
                return r;
            }
            std::vector<uint64_t> construct_lcp_array()
            {
                std::vector<uint64_t> r;
                r.resize(this->_rlbwt.str_size(), 0);

                while (this->queue.size() > 0)
                {
                    std::vector<LPOS> next_lcp_indexes = this->compute_next_lcp_indexes();
                    for (auto it : next_lcp_indexes)
                    {
                        uint64_t pos = this->fposArray[it.first] + it.second;
                        r[pos + 1] = this->current_length;
                    }
                    if (next_lcp_indexes.size() == 0 && this->queue.size() == 1)
                    {
                        break;
                    }
                    else
                    {
                        this->current_length++;
                    }
                }
                return r;
            }
            uint64_t total_counter = 0;
            uint64_t king_counter = 0;
            uint64_t king_counter2 = 0;

            std::vector<uint64_t> _construct_sampling_lcp_array()
            {
                std::vector<uint64_t> r;
                r.resize(this->_rlbwt.rle_size(), 0);

                while (r.size() != total_counter)
                {
                    this->king_counter = 0;
                    this->king_counter2 = 0;

                    std::vector<LPOS> next_lcp_indexes = this->compute_next_lcp_indexes();
                    uint64_t counter = 0;
                    for (auto it : next_lcp_indexes)
                    {
                        if (it.second == this->_rlbwt.get_run(it.first) - 1)
                        {
                            r[it.first] = this->current_length;

                            counter++;
                            total_counter++;
                        }
                    }
                    //std::cout << "LCP = " << this->current_length << ", " << counter << ", " << next_lcp_indexes.size() << ", " << (r.size() - total_counter) << ", " << this->king_counter << ", " << this->king_counter2 << std::endl;

                    if (next_lcp_indexes.size() == 0 && this->queue.size() == 1)
                    {
                        break;
                    }
                    else
                    {
                        this->current_length++;
                    }
                }

                return r;
            }
            /*
            vector<WeinerInterval> naiveWeinerQuery(uint64_t &begin_lindex, uint64_t &begin_diff, uint64_t &end_lindex, uint64_t &end_diff)
            {
                vector<WeinerInterval> r;

                std::unordered_map<CHAR, uint64_t> beginMap;
                std::unordered_map<CHAR, uint64_t> endMap;

                for (uint64_t i = begin_lindex; i <= end_lindex; i++)
                {
                    CHAR c = _rlbwt.get_char_by_run_index(i);
                    auto it = beginMap.find(c);
                    if (it == beginMap.end())
                    {
                        beginMap[c] = i;
                    }
                }

                for (int64_t i = end_lindex; i >= (int64_t)begin_lindex; --i)
                {
                    CHAR c = _rlbwt.get_char_by_run_index(i);
                    auto it = endMap.find(c);
                    if (it == endMap.end())
                    {
                        endMap[c] = i;
                    }
                }

                for (auto itr = beginMap.begin(); itr != beginMap.end(); ++itr)
                {
                    CHAR c = itr->first;
                    uint64_t cBeginIndex = itr->second;
                    uint64_t cEndIndex = endMap[c];
                    uint64_t cBeginDiff = cBeginIndex == begin_lindex ? begin_diff : 0;
                    uint64_t cEndDiff = cEndIndex == end_lindex ? end_diff : _rlbwt.get_run(cEndIndex) - 1;

                    WeinerInterval cInterval;
                    cInterval.beginIndex = cBeginIndex;
                    cInterval.beginDiff = cBeginDiff;
                    cInterval.endIndex = cEndIndex;
                    cInterval.endDiff = cEndDiff;

                    r.push_back(cInterval);
                }
                return r;
            }
            */
            static std::vector<uint64_t> construct_sampling_lcp_array(const RLBWT_STR &__rlbwt)
            {
                Weiner<RLBWT_STR> weiner(__rlbwt);
                return weiner._construct_sampling_lcp_array();
            }
        };
        template <typename RLBWT_STR>
        class SamplingLCPArrayConstructor
        {

        public:
            static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &__rlbwt)
            {
                auto w = Weiner<RLBWT_STR>::construct_sampling_lcp_array(__rlbwt);

                auto fl_mapper = RLBWTFunctions::construct_rle_fl_mapper(__rlbwt);

                std::vector<uint64_t> r;
                r.resize(__rlbwt.rle_size(), 0);
                for (uint64_t i = 0; i < r.size() - 1; i++)
                {
                    r[fl_mapper[i + 1]] = w[fl_mapper[i]];
                }

                return r;
            }
        };
    } // namespace rlbwt
} // namespace stool