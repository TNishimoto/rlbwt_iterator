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
        struct SamplingSAStackData
        {
            LPoint start;
            LPoint end;
            uint64_t distance;
        };
        struct SamplingSAStackData1
        {
            uint64_t source;
            uint64_t destination;
            uint64_t distance;
            bool is_source_first;
            bool is_destination_first;
        };
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
            static std::pair<std::vector<INDEX2>, std::vector<INDEX2>> construct_sampling_sa_lorder(const RLBWT_STR &rlbwt, std::vector<INDEX> &hole_pos_array, std::vector<INDEX> &hole_length_array)
            {
                using STACK_POS = std::pair<INDEX2, bool>;
                using CHAR = typename RLBWT_STR::char_type;
                INDEX2 MAXFLAG = std::numeric_limits<INDEX2>::max();
                std::pair<std::vector<INDEX2>, std::vector<INDEX2>> r;
                std::vector<INDEX2> &beginVec = r.first;
                std::vector<INDEX2> &endVec = r.second;
                INDEX rleSize = rlbwt.rle_size();

                beginVec.resize(rleSize, MAXFLAG);
                endVec.resize(rleSize, MAXFLAG);

                uint64_t total = 0;

                std::stack<STACK_POS> stack;

                for (INDEX i = 0; i < rleSize; i++)
                {
                    if (beginVec[i] == MAXFLAG)
                    {
                        stack.push(STACK_POS(i, true));
                    }
                    if (endVec[i] == MAXFLAG && rlbwt.get_run(i) != 1)
                    {
                        stack.push(STACK_POS(i, false));
                    }

                    while (stack.size() > 0)
                    {
                        STACK_POS top = stack.top();

                        uint64_t lindex = top.first;
                        uint64_t diff = top.second ? 0 : (rlbwt.get_run(lindex) - 1);
                        uint64_t pos = 0;
                        uint64_t distance = 0;
                        bool b = rlbwt.get_run(lindex) == 1;

                        if(stack.size() % 100000 == 0){
                            std::cout << "[" << total << ", " << (rleSize * 2) << ", " << stack.size() << "]" << std::endl;
                        }

                        while (true)
                        {
                            pos = hole_pos_array[lindex] + diff;
                            distance += hole_length_array[lindex];
                            lindex = rlbwt.get_lindex_containing_the_position(pos);
                            diff = pos - rlbwt.get_lpos(lindex);
                            CHAR c = rlbwt.get_char_by_run_index(lindex);

                            if (diff == 0 || diff == rlbwt.get_run(lindex) - 1)
                            {
                                if (c == 0)
                                {
                                    if (top.second)
                                    {
                                        beginVec[top.first] = distance - 1;
                                    }
                                    if ((top.second && b) || !top.second)
                                    {
                                        endVec[top.first] = distance - 1;
                                    }
                                    stack.pop();
                                    total++;
                                }
                                else if (diff == 0 && beginVec[lindex] != MAXFLAG)
                                {
                                    if (top.second)
                                    {
                                        beginVec[top.first] = beginVec[lindex] + distance;
                                    }
                                    if ((top.second && b) || !top.second)
                                    {
                                        endVec[top.first] = beginVec[lindex] + distance;
                                    }
                                    stack.pop();
                                    total++;
                                }
                                else if (diff != 0 && endVec[lindex] != MAXFLAG)
                                {
                                    if (top.second)
                                    {
                                        beginVec[top.first] = endVec[lindex] + distance;
                                    }
                                    if ((top.second && b) || !top.second)
                                    {
                                        endVec[top.first] = endVec[lindex] + distance;
                                    }
                                    stack.pop();
                                    total++;
                                }
                                else
                                {
                                    stack.push(STACK_POS(lindex, diff == 0));
                                }
                                break;
                            }
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
        };
    } // namespace rlbwt
} // namespace stool