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
        class HyperWeiner
        {
        private:
            using CHAR = typename RLBWT_STR::char_type;
            using CHAR_VEC = typename RLBWT_STR::char_vec_type;
            using LPOS = std::pair<INDEX_SIZE, INDEX_SIZE>;

            const RLBWT_STR &_rlbwt;
            std::queue<WeinerInterval<INDEX_SIZE>> queue;
            std::vector<bool> checkerArray;
            std::vector<INDEX_SIZE> fposArray;
            std::vector<INDEX_SIZE> hole_pos_array;
            std::vector<INDEX_SIZE> hole_length_array;
            std::map<uint64_t, std::vector<WeinerInterval<INDEX_SIZE>>> hyperMap;

            //std::vector<uint64_t> frunStartingPositionMapperArray;

            //std::vector<uint64_t> lf_mapper;
            RangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> range_distinct_data_structure;

            uint64_t current_length = 0;
            uint64_t str_size;

            uint64_t skip_ratio = 16;

        public:
            HyperWeiner(const RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt)
            {
                this->str_size = _rlbwt.str_size();
                auto v1 = RLBWTFunctions::construct_fpos_array(_rlbwt);
                this->fposArray.swap(v1);
                this->checkerArray.resize(_rlbwt.rle_size(), false);

                //auto v2 = RLBWTFunctions2::construct_rle_lf_lorder(_rlbwt);
                //this->frunStartingPositionMapperArray.swap(v2);

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
                return RangeDistinctDataStructureOnRLBWT<RLBWT_STR,INDEX_SIZE>::range_distinct(_rlbwt, range_distinct_data_structure, begin_lindex, begin_diff, end_lindex, end_diff);
            }
            std::pair<WeinerInterval<INDEX_SIZE>, uint64_t> getHoleWeinerInterval(WeinerInterval<INDEX_SIZE> &interval, uint64_t lcp, bool skip_flag)
            {
                WeinerInterval<INDEX_SIZE> output;
                uint64_t begin_pos = this->fposArray[interval.beginIndex] + interval.beginDiff;
                output.beginIndex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                output.beginDiff = begin_pos - _rlbwt.get_lpos(output.beginIndex);

                uint64_t end_pos = this->fposArray[interval.endIndex] + interval.endDiff;
                output.endIndex = _rlbwt.get_lindex_containing_the_position(end_pos);
                output.endDiff = end_pos - _rlbwt.get_lpos(output.endIndex);

                uint64_t b = end_pos == (_rlbwt.get_lpos(output.endIndex) + _rlbwt.get_run(output.endIndex) - 1);
                if (skip_flag)
                {
                    while (output.beginIndex == output.endIndex && !b)
                    {
                        uint64_t length = end_pos - begin_pos + 1;
                        begin_pos = this->hole_pos_array[output.beginIndex] + output.beginDiff;
                        lcp += this->hole_length_array[output.beginIndex];
                        output.beginIndex = _rlbwt.get_lindex_containing_the_position(begin_pos);
                        output.beginDiff = begin_pos - _rlbwt.get_lpos(output.beginIndex);
                        end_pos = begin_pos + length - 1;
                        output.endIndex = _rlbwt.get_lindex_containing_the_position(end_pos);
                        output.endDiff = end_pos - _rlbwt.get_lpos(output.endIndex);
                        b = end_pos == (_rlbwt.get_lpos(output.endIndex) + _rlbwt.get_run(output.endIndex) - 1);
                    }
                }
                return std::pair<WeinerInterval<INDEX_SIZE>, uint64_t>(output, lcp);
            }
            void process_hyper_map(std::vector<LPOS> &output)
            {
                auto map_top = this->hyperMap.begin();
                if (map_top != this->hyperMap.end() && map_top->first == this->current_length)
                {
                    for (auto &it : map_top->second)
                    {
                        vector<WeinerInterval<INDEX_SIZE>> result = RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE>::range_distinct(_rlbwt, range_distinct_data_structure, it.beginIndex, it.beginDiff, it.endIndex, it.endDiff);
                        for (auto it2 : result)
                        {
                            //uint64_t end_pos = this->fposArray[it.endIndex] + it.endDiff;
                            bool b = _rlbwt.get_run(it2.endIndex) == (it2.endDiff + 1);
                            if (!b || !this->checkerArray[it2.endIndex])
                            {
                                output.push_back(LPOS(it2.endIndex, it2.endDiff));
                                if (b)
                                {
                                    //std::cout << "@i = " << it2.endIndex << "/[" << it.beginIndex << ", " << it.beginDiff << "][" << it.endIndex << ", " << it.endDiff << "]" << std::endl;
                                    this->checkerArray[it2.endIndex] = true;
                                }

                                this->queue.push(it2);
                            }
                        }
                    }
                    this->hyperMap.erase(this->hyperMap.begin());
                }
            }
            std::vector<LPOS> compute_next_lcp_indexes(bool skip_flag)
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
                            this->process_hyper_map(r);
                            this->queue.pop();
                            this->queue.push(WeinerInterval<INDEX_SIZE>::get_special());
                            break;
                        }
                        else
                        {
                            std::pair<WeinerInterval<INDEX_SIZE>, uint64_t> front_info = this->getHoleWeinerInterval(front, this->current_length, skip_flag);

                            if (this->current_length == front_info.second)
                            {
                                vector<WeinerInterval<INDEX_SIZE>> result = RangeDistinctDataStructureOnRLBWT<RLBWT_STR, INDEX_SIZE>::range_distinct(_rlbwt, range_distinct_data_structure, front_info.first.beginIndex, front_info.first.beginDiff, front_info.first.endIndex, front_info.first.endDiff);
                                for (auto it : result)
                                {
                                    bool b = _rlbwt.get_run(it.endIndex) == (it.endDiff + 1);
                                    if (!b || !this->checkerArray[it.endIndex])
                                    {
                                        r.push_back(LPOS(it.endIndex, it.endDiff));
                                        if (b)
                                        {
                                            this->checkerArray[it.endIndex] = true;
                                        }

                                        this->queue.push(it);
                                    }
                                }
                            }
                            else
                            {
                                auto findResult = this->hyperMap.find(front_info.second);
                                if (findResult == this->hyperMap.end())
                                {
                                    auto insertResult = this->hyperMap.insert(std::pair<uint64_t, std::vector<WeinerInterval<INDEX_SIZE>>>(front_info.second, std::vector<WeinerInterval<INDEX_SIZE>>()));
                                    insertResult.first->second.push_back(front_info.first);
                                }else{
                                    findResult->second.push_back(front_info.first);
                                }
                                //this->hyperMap[front_info.second].push_back(front_info.first);
                            }
                            this->queue.pop();
                        }
                    }
                }
                return r;
            }
            /*
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
            */
            uint64_t total_counter = 0;

            std::vector<uint64_t> _construct_sampling_lcp_array()
            {
                std::vector<uint64_t> r;
                r.resize(this->_rlbwt.rle_size(), 0);
                uint64_t skip_threshold = (r.size() * (skip_ratio-1)) / skip_ratio; 

                while (!(r.size() == total_counter))
                {

                    if(this->hole_pos_array.size() == 0 && (total_counter > skip_threshold)){
                        RLBWTFunctions::construct_hole_array(_rlbwt, this->fposArray, hole_pos_array, hole_length_array);
                    }

                    std::vector<LPOS> next_lcp_indexes = this->compute_next_lcp_indexes(total_counter > skip_threshold);
                    for (auto it : next_lcp_indexes)
                    {
                        if (it.second == this->_rlbwt.get_run(it.first) - 1)
                        {
                            r[it.first] = this->current_length;
                            total_counter++;
                        }
                    }

                    /*
                    if (counter > 0)
                    {
                        std::cout << "LCP = " << this->current_length << ", " << counter << ", " << (r.size() - total_counter) << "/" << (total_counter > skip_threshold) << std::endl;
                    }
                    */
                    this->current_length++;
                }

                return r;
            }
            static std::vector<uint64_t> construct_sampling_lcp_array(const RLBWT_STR &__rlbwt)
            {
                HyperWeiner<RLBWT_STR> weiner(__rlbwt);
                return weiner._construct_sampling_lcp_array();
            }
        };
        template <typename RLBWT_STR>
        class HyperSamplingLCPArrayConstructor
        {

        public:
            static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &__rlbwt)
            {
                auto w = HyperWeiner<RLBWT_STR, uint64_t>::construct_sampling_lcp_array(__rlbwt);

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