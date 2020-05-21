#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>

#include "../rlbwt.hpp"
#include "../backward_text.hpp"
#include "../rle_farray.hpp"
#include "../rlbwt_functions.hpp"
#include "stool/src/debug.hpp"

namespace stool
{
    namespace rlbwt
    {
        struct WeinerInterval
        {
            uint64_t beginIndex;
            uint64_t beginDiff;
            uint64_t endIndex;
            uint64_t endDiff;

            void print()
            {
                std::cout << "[" << this->beginIndex << ", " << this->beginDiff << ", " << this->endIndex << ", " << this->endDiff << "]" << std::endl;
            }
            void print2(std::vector<uint64_t> &fposArray)
            {
                if (this->is_special())
                {
                    std::cout << "[BOTTOM]" << std::endl;
                }
                else
                {
                    uint64_t begin_pos = fposArray[this->beginIndex] + this->beginDiff;
                    uint64_t end_pos = fposArray[this->endIndex] + this->endDiff;

                    std::cout << "[" << begin_pos << ", " << end_pos << "]" << std::endl;
                }
            }

            bool is_special()
            {
                return this->beginIndex == UINT64_MAX;
            }

            static WeinerInterval get_special()
            {
                WeinerInterval r;
                r.beginIndex = UINT64_MAX;
                return r;
            }
        };

        template <typename RLBWT_STR>
        class Weiner
        {
        private:
            using CHAR = typename RLBWT_STR::char_type;
            const RLBWT_STR &_rlbwt;
            std::queue<WeinerInterval> queue;
            std::vector<bool> checkerArray;
            std::vector<uint64_t> fposArray;
            std::vector<uint64_t> lf_mapper;

            uint64_t current_length = 0;
            uint64_t str_size;

        public:
            Weiner(const RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt)
            {
                this->str_size = _rlbwt.str_size();
                auto v1 = RLBWTFunctions::construct_fpos_array(_rlbwt);
                this->fposArray.swap(v1);
                //stool::Printer::print(this->fposArray);

                auto v2 = RLBWTFunctions::construct_rle_lf_mapper(_rlbwt);
                this->lf_mapper.swap(v2);

                this->checkerArray.resize(str_size, false);

                this->queue.push(WeinerInterval::get_special());
            }
            vector<WeinerInterval> computeFirstWeinerIntervals()
            {
                uint64_t begin_lindex = 0;
                uint64_t begin_diff = 0;
                uint64_t end_lindex = _rlbwt.rle_size() - 1;
                uint64_t end_diff = _rlbwt.get_run(end_lindex) - 1;
                return this->naiveWeinerQuery(begin_lindex, begin_diff, end_lindex, end_diff);
                /*
                vector<WeinerInterval> r;
                auto vec = RLBWTFunctions::construct_rle_fl_mapper(_rlbwt);
                uint64_t begin = 0;
                for (uint64_t i = 1; i < vec.size(); i++)
                {
                    CHAR c1 = _rlbwt.get_char_by_run_index(vec[i - 1]);
                    CHAR c2 = _rlbwt.get_char_by_run_index(vec[i]);
                    if (c1 != c2)
                    {
                        WeinerInterval interval;
                        interval.beginIndex = begin;
                        interval.beginDiff = 0;
                        interval.endIndex = i - 1;
                        interval.endDiff = _rlbwt.get_run(vec[i - 1]) - 1;
                        r.push_back(interval);
                        begin = i;
                    }
                }
                WeinerInterval interval;
                interval.beginIndex = begin;
                interval.beginDiff = 0;
                interval.endIndex = vec.size() - 1;
                interval.endDiff = _rlbwt.get_run(vec[vec.size() - 1]) - 1;
                r.push_back(interval);

                return r;
                */
            }
            std::vector<uint64_t> compute_next_lcp_indexes()
            {
                std::vector<uint64_t> r;

                if (this->current_length == 0)
                {
                    this->queue.pop();
                    vector<WeinerInterval> vec = this->computeFirstWeinerIntervals();
                    for (auto it : vec)
                    {
                        uint64_t end_pos = this->fposArray[it.endIndex] + it.endDiff;
                        r.push_back(end_pos);
                        //std::cout << "PUSH ";
                        //it.print();
                        //it.print2(this->fposArray);

                        this->queue.push(it);
                    }
                    this->queue.push(WeinerInterval::get_special());
                }
                else
                {
                    while (true)
                    {
                        WeinerInterval front = this->queue.front();
                        //front.print2(this->fposArray);
                        if (front.is_special())
                        {
                            this->queue.pop();
                            this->queue.push(WeinerInterval::get_special());
                            break;
                        }
                        else
                        {
                            uint64_t begin_pos = this->fposArray[front.beginIndex] + front.beginDiff;
                            uint64_t begin_lindex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                            uint64_t begin_diff = begin_pos - _rlbwt.get_lpos(begin_lindex);

                            uint64_t end_pos = this->fposArray[front.endIndex] + front.endDiff;
                            uint64_t end_lindex = _rlbwt.get_lindex_containing_the_position(end_pos);
                            uint64_t end_diff = end_pos - _rlbwt.get_lpos(end_lindex);

                            vector<WeinerInterval> result = this->naiveWeinerQuery(begin_lindex, begin_diff, end_lindex, end_diff);
                            for (auto it : result)
                            {
                                uint64_t end_pos = this->fposArray[it.endIndex] + it.endDiff;
                                if (!this->checkerArray[end_pos])
                                {
                                    r.push_back(end_pos);
                                    //std::cout << "PUSH ";
                                    //it.print2(this->fposArray);
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
                    std::vector<uint64_t> next_lcp_indexes = this->compute_next_lcp_indexes();
                    for (auto it : next_lcp_indexes)
                    {
                        //std::cout << "LCP[" << (it + 1) << "] = " << this->current_length << std::endl;
                        r[it+1] = this->current_length;
                        this->checkerArray[it] = true;
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
            vector<WeinerInterval> naiveWeinerQuery(uint64_t &begin_lindex, uint64_t &begin_diff, uint64_t &end_lindex, uint64_t &end_diff)
            {
                vector<WeinerInterval> r;
                /*
                uint64_t begin_pos = this->fposArray[interval.beginIndex] + interval.beginDiff;
                uint64_t begin_lindex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                uint64_t begin_diff = begin_pos - _rlbwt.get_lpos(begin_lindex);

                uint64_t end_pos = this->fposArray[interval.endIndex] + interval.endDiff;
                uint64_t end_lindex = _rlbwt.get_lindex_containing_the_position(end_pos);
                uint64_t end_diff = end_pos - _rlbwt.get_lpos(end_lindex);
                */

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

            void test()
            {
                auto vec = this->computeFirstWeinerIntervals();
                for (uint64_t i = 0; i < vec.size(); i++)
                {
                    vec[i].print();
                }
                _rlbwt.print_info();
            }
        };
    } // namespace rlbwt
} // namespace stool