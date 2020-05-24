#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./rlbwt_functions.hpp"

namespace stool
{
    namespace rlbwt
    {

        struct HoleStackData
        {
            uint64_t lrun_index;
            uint64_t hole_pos;
            uint64_t hole_length;
        };
        /*
        struct LPoint
        {
            uint64_t index;
            uint64_t diff;
            LPoint(uint64_t _index, uint64_t _diff)
            {
                this->index = _index;
                this->diff = _diff;
            }
        };
        */
        struct SamplingSAStackData
        {
            uint64_t start;
            uint64_t distance;
            bool is_start;
        };
        /*
        struct SamplingSAStackData1
        {
            uint64_t source;
            uint64_t destination;
            uint64_t distance;
            bool is_source_first;
            bool is_destination_first;
        };
        */
        /*
        template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type>
        class LFDataStructure{
            const RLBWT_STR &_rlbwt;
            std::vector<stool::EliasFanoVector> charRankVec;
            std::vector<uint64_t> charStartingPositionVec;
            std::vector<INDEX_SIZE> rankVec;

            LFDataStructure(RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt){

            }
        };
        */

        class SamplingFunctions
        {
        public:
            template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type>
            static void construct_hole_array(const RLBWT_STR &rlbwt, std::vector<INDEX> &fpos_array, std::vector<INDEX> &output_hole_pos_array, std::vector<INDEX> &output_hole_length_array)
            {
                //using INDEX = typename RLBWT_STR::index_type;

                output_hole_pos_array.resize(rlbwt.rle_size(), std::numeric_limits<INDEX>::max());
                output_hole_length_array.resize(rlbwt.rle_size(), std::numeric_limits<INDEX>::max());
                INDEX rleSize = rlbwt.rle_size();
                std::stack<HoleStackData> stack;
                //std::vector<bool> checker;
                //checker.resize(rleSize, false);
                for (INDEX i = 0; i < rleSize; i++)
                {
                    if (output_hole_length_array[i] == std::numeric_limits<INDEX>::max())
                    {
                        HoleStackData hsd;
                        hsd.hole_length = 1;
                        hsd.hole_pos = fpos_array[i];
                        hsd.lrun_index = i;
                        stack.push(hsd);
                    }
                    while (stack.size() > 0)
                    {
                        HoleStackData top = stack.top();
                        stack.pop();

                        uint64_t begin_lindex = rlbwt.get_lindex_containing_the_position(top.hole_pos);
                        uint64_t begin_diff = top.hole_pos - rlbwt.get_lpos(begin_lindex);

                        uint64_t end_pos = top.hole_pos + rlbwt.get_run(top.lrun_index) - 1;
                        uint64_t end_lindex = rlbwt.get_lindex_containing_the_position(end_pos);
                        uint64_t end_diff = end_pos - rlbwt.get_lpos(end_lindex);

                        bool b = (end_diff + 1 == rlbwt.get_run(end_lindex));

                        if (begin_lindex != end_lindex || (begin_lindex == end_lindex && b))
                        {
                            output_hole_pos_array[top.lrun_index] = top.hole_pos;
                            output_hole_length_array[top.lrun_index] = top.hole_length;
                        }
                        else
                        {
                            if (output_hole_length_array[begin_lindex] == std::numeric_limits<INDEX>::max())
                            {
                                stack.push(top);
                                HoleStackData hsd;
                                hsd.hole_length = 1;
                                hsd.hole_pos = fpos_array[begin_lindex];
                                hsd.lrun_index = begin_lindex;
                                stack.push(hsd);
                            }
                            else
                            {

                                top.hole_length = top.hole_length + output_hole_length_array[begin_lindex];

                                top.hole_pos = output_hole_pos_array[begin_lindex] + begin_diff;
                                stack.push(top);
                            }
                        }
                    }
                }

                uint64_t max = 0;
                for (uint64_t i = 0; i < rleSize; i++)
                {
                    if (output_hole_length_array[i] > max)
                    {
                        max = output_hole_length_array[i];
                    }
                }
                std::cout << "max: " << max << std::endl;
            }
            template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type>
            static void construct_SA_hole_array(const RLBWT_STR &rlbwt, std::vector<INDEX> &output_hole_pos_array, std::vector<INDEX> &output_hole_length_array)
            {
                std::vector<INDEX> fpos_array = stool::rlbwt::RLBWTFunctions::construct_fpos_array<RLBWT_STR, INDEX>(rlbwt);

                output_hole_pos_array.resize(rlbwt.rle_size(), std::numeric_limits<INDEX>::max());
                output_hole_length_array.resize(rlbwt.rle_size(), std::numeric_limits<INDEX>::max());
                INDEX rleSize = rlbwt.rle_size();
                std::stack<HoleStackData> stack;
                for (INDEX i = 0; i < rleSize; i++)
                {
                    if (output_hole_length_array[i] == std::numeric_limits<INDEX>::max())
                    {
                        HoleStackData hsd;
                        hsd.hole_length = 1;
                        hsd.hole_pos = fpos_array[i];
                        hsd.lrun_index = i;
                        stack.push(hsd);
                    }
                    while (stack.size() > 0)
                    {
                        HoleStackData top = stack.top();
                        stack.pop();

                        uint64_t begin_lindex = rlbwt.get_lindex_containing_the_position(top.hole_pos);
                        uint64_t begin_diff = top.hole_pos - rlbwt.get_lpos(begin_lindex);

                        uint64_t end_pos = top.hole_pos + rlbwt.get_run(top.lrun_index) - 1;
                        uint64_t end_lindex = rlbwt.get_lindex_containing_the_position(end_pos);
                        uint64_t end_diff = end_pos - rlbwt.get_lpos(end_lindex);

                        bool b = (end_diff + 1 == rlbwt.get_run(end_lindex)) || begin_diff == 0;

                        if (begin_lindex != end_lindex || (begin_lindex == end_lindex && b))
                        {
                            output_hole_pos_array[top.lrun_index] = top.hole_pos;
                            output_hole_length_array[top.lrun_index] = top.hole_length;
                        }
                        else
                        {
                            if (output_hole_length_array[begin_lindex] == std::numeric_limits<INDEX>::max())
                            {
                                stack.push(top);
                                HoleStackData hsd;
                                hsd.hole_length = 1;
                                hsd.hole_pos = fpos_array[begin_lindex];
                                hsd.lrun_index = begin_lindex;
                                stack.push(hsd);
                            }
                            else
                            {

                                top.hole_length = top.hole_length + output_hole_length_array[begin_lindex];

                                top.hole_pos = output_hole_pos_array[begin_lindex] + begin_diff;
                                stack.push(top);
                            }
                        }
                    }
                }

                uint64_t max = 0;
                for (uint64_t i = 0; i < rleSize; i++)
                {
                    if (output_hole_length_array[i] > max)
                    {
                        max = output_hole_length_array[i];
                    }
                }
                std::cout << "max: " << max << std::endl;
            }

        public:
            template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type, typename INDEX2 = typename RLBWT_STR::index_type>
            static SamplingSAStackData local_function(const RLBWT_STR &rlbwt, std::vector<INDEX> &hole_pos_array, std::vector<INDEX> &hole_length_array, SamplingSAStackData &input)
            {
                SamplingSAStackData output;

                uint64_t lindex = input.start;
                uint64_t diff = input.is_start ? 0 : (rlbwt.get_run(lindex) - 1);
                uint64_t pos = 0;
                uint64_t distance = 0;
                //bool b = rlbwt.get_run(lindex) == 1;
                while (true)
                {
                    pos = hole_pos_array[lindex] + diff;
                    distance += hole_length_array[lindex];
                    lindex = rlbwt.get_lindex_containing_the_position(pos);
                    diff = pos - rlbwt.get_lpos(lindex);
                    if (diff == 0 || diff == rlbwt.get_run(lindex) - 1)
                    {
                        output.start = lindex;
                        output.is_start = diff == 0;
                        output.distance = distance;
                        break;
                    }
                }
                return output;
            }

            template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type, typename INDEX2 = typename RLBWT_STR::index_type>
            static std::pair<std::vector<INDEX2>, std::vector<INDEX2>> construct_sampling_sa_lorder(const RLBWT_STR &rlbwt, std::vector<INDEX> &hole_pos_array, std::vector<INDEX> &hole_length_array)
            {
                //using STACK_POS = std::pair<INDEX2, bool>;
                using CHAR = typename RLBWT_STR::char_type;
                INDEX2 MAXFLAG = std::numeric_limits<INDEX2>::max();
                std::pair<std::vector<INDEX2>, std::vector<INDEX2>> r;
                std::vector<INDEX2> &beginVec = r.first;
                std::vector<INDEX2> &endVec = r.second;
                INDEX rleSize = rlbwt.rle_size();
                INDEX end_lindex = rlbwt.get_end_rle_lposition();

                beginVec.resize(rleSize, MAXFLAG);
                endVec.resize(rleSize, MAXFLAG);

                uint64_t total = 0;

                //std::stack<SamplingSAStackData> stack;
                //uint64_t i = 0;
                //bool is_i_start = true;
                //SamplingSAStackData ssd;

                int64_t current_sa_value = rlbwt.str_size() - 1;
                uint64_t current_lindex = end_lindex;
                uint64_t current_diff = 0;
                beginVec[end_lindex] = current_sa_value;
                endVec[end_lindex] = current_sa_value;

                while (current_sa_value > 0)
                {
                    uint64_t lindex = current_lindex;
                    uint64_t diff = current_diff;
                    uint64_t pos = 0;
                    uint64_t distance = 0;
                    //bool b = rlbwt.get_run(lindex) == 1;
                    while (true)
                    {
                        pos = hole_pos_array[lindex] + diff;
                        distance += hole_length_array[lindex];
                        lindex = rlbwt.get_lindex_containing_the_position(pos);
                        diff = pos - rlbwt.get_lpos(lindex);
                        if (diff == 0 || diff == rlbwt.get_run(lindex) - 1)
                        {
                            current_sa_value -= distance;
                            if (lindex == end_lindex)
                            {
                                break;
                            }

                            if (diff == 0)
                            {
                                beginVec[lindex] = current_sa_value;
                            }
                            if (diff == rlbwt.get_run(lindex) - 1)
                            {
                                endVec[lindex] = current_sa_value;
                            }
                            current_lindex = lindex;
                            current_diff = diff;
                            break;
                        }
                    }
                }

                /*
                while (i < rleSize)
                {
                    bool i_is_empty = false;
                    if (is_i_start && beginVec[i] == MAXFLAG)
                    {
                        ssd.start = i;
                        ssd.is_start = true;
                        ssd.distance = 0;
                        i_is_empty = true;
                    }
                    else if (!is_i_start && endVec[i] == MAXFLAG && rlbwt.get_run(i) != 1)
                    {
                        SamplingSAStackData ssd;
                        ssd.start = i;
                        ssd.is_start = false;
                        ssd.distance = 0;
                        stack.push(ssd);
                        i_is_empty = true;
                    }

                    if (i_is_empty)
                    {
                        uint64_t prev_sa_value = UINT64_MAX;
                        while (true)
                        {
                            auto output = local_function(rlbwt, hole_pos_array, hole_length_array, ssd);
                            ssd.distance = output.distance;
                            CHAR c = rlbwt.get_char_by_run_index(output.start);
                            stack.push(ssd);
                            if (c == 0)
                            {
                                prev_sa_value = rlbwt.str_size() - 1;
                                break;
                            }
                            else if (output.is_start && beginVec[output.start] != MAXFLAG)
                            {
                                prev_sa_value = beginVec[output.start];
                                break;
                            }
                            else if (!output.is_start && endVec[output.start] != MAXFLAG)
                            {
                                prev_sa_value = endVec[output.start];
                                break;
                            }
                            else
                            {
                                ssd.start = output.start;
                                ssd.is_start = output.is_start;
                                ssd.distance = 0;
                            }
                        }
                        while (stack.size() > 0)
                        {

                            auto top = stack.top();
                            stack.pop();
                            bool b = rlbwt.get_run(top.start) == 1;

                            if (prev_sa_value != rlbwt.str_size() - 1)
                            {
                                prev_sa_value = prev_sa_value + top.distance;
                            }
                            else
                            {
                                prev_sa_value = top.distance - 1;
                            }
                            if (top.is_start)
                            {
                                beginVec[top.start] = prev_sa_value;
                            }
                            if ((top.is_start && b) || !top.is_start)
                            {
                                endVec[top.start] = prev_sa_value;
                            }
                        }
                    }

                    if (is_i_start)
                    {
                        is_i_start = false;
                    }
                    else
                    {
                        is_i_start = true;
                        i++;
                    }
                }
                */
                return r;
            }
            template <typename RLBWT_STR, typename INDEX = typename RLBWT_STR::index_type>
            static std::pair<std::vector<uint64_t>, std::vector<uint64_t>> construct_sampling_sa_lorder(const RLBWT_STR &rlbwt)
            {

                std::vector<INDEX> hole_pos_array;
                std::vector<INDEX> hole_length_array;
                stool::rlbwt::SamplingFunctions::construct_SA_hole_array(rlbwt, hole_pos_array, hole_length_array);
                return stool::rlbwt::SamplingFunctions::construct_sampling_sa_lorder(rlbwt, hole_pos_array, hole_length_array);
            }
        };
    } // namespace rlbwt
} // namespace stool