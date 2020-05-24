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
            }

            /*
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
            */
        };
    } // namespace rlbwt
} // namespace stool