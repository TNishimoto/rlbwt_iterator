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
//#include "other_functions.hpp"
#include "OnlineRlbwt/online_rlbwt.hpp"
#include "rlbwt.hpp"
#include "stool/src/elias_fano_vector.hpp"
//using namespace std;

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

        class RLBWTFunctions
        {
        public:
            template <typename RLBWT_STR>
            static typename RLBWT_STR::index_type lf(const RLBWT_STR &rlbwt, typename RLBWT_STR::index_type pos, std::vector<typename RLBWT_STR::index_type> &fpos_vec)
            {
                using INDEX = typename RLBWT_STR::index_type;
                INDEX lindex = rlbwt.get_lindex_containing_the_position(pos);
                INDEX diff = pos - rlbwt.get_lpos(lindex);
                INDEX succ_sa_index = fpos_vec[lindex] + diff;
                return succ_sa_index;
            }
            /*
    Let R be the output array of length r.
    R[i] stores the starting position of the F-run corresponding to i-th L-run.
    */
            template <typename RLBWT_STR>
            static std::vector<typename RLBWT_STR::index_type> construct_fpos_array(const RLBWT_STR &rlbwt)
            {
                using INDEX = typename RLBWT_STR::index_type;
                std::vector<INDEX> fvec = construct_rle_fl_mapper(rlbwt);
                std::vector<INDEX> output;
                output.resize(fvec.size(), 0);
                INDEX fsum = 0;
                for (INDEX i = 0; i < fvec.size(); i++)
                {
                    output[fvec[i]] = fsum;
                    fsum += rlbwt.get_run(fvec[i]);
                }
                return output;
            }
            /*
    Let R be the output array of length r.
    R[i] stores the index of the L-run corresponding to i-th F-run.
    */
            template <typename RLBWT_STR>
            static std::vector<typename RLBWT_STR::index_type> construct_rle_fl_mapper(const RLBWT_STR &rlbwt)
            {
                using INDEX = typename RLBWT_STR::index_type;
                std::vector<INDEX> indexes;
                indexes.resize(rlbwt.rle_size());
                for (INDEX i = 0; i < rlbwt.rle_size(); i++)
                {
                    indexes[i] = i;
                }
                sort(indexes.begin(), indexes.end(),
                     [&](const INDEX &x, const INDEX &y) {
                         if (rlbwt.get_char_by_run_index(x) == rlbwt.get_char_by_run_index(y))
                         {
                             return x < y;
                         }
                         else
                         {
                             return (uint64_t)rlbwt.get_char_by_run_index(x) < (uint64_t)rlbwt.get_char_by_run_index(y);
                             //return this->char_vec[x] < this->char_vec[y];
                         }
                     });
                return indexes;
            }

            /*
    Let R be the output array of length r.
    R[i] stores the index of the F-run corresponding to i-th L-run.
    */
            template <typename RLBWT_STR>
            static std::vector<typename RLBWT_STR::index_type> construct_rle_lf_mapper(const RLBWT_STR &rlbwt)
            {
                std::vector<typename RLBWT_STR::index_type> indexes = construct_rle_fl_mapper(rlbwt);
                return stool::rlbwt::change_inv(std::move(indexes));
            }

            template <typename RLBWT_STR>
            static void construct_hole_array(const RLBWT_STR &rlbwt, std::vector<uint64_t> &fpos_array, std::vector<uint64_t> &output_hole_pos_array, std::vector<uint64_t> &output_hole_length_array)
            {
                //using INDEX = typename RLBWT_STR::index_type;

                output_hole_pos_array.resize(rlbwt.rle_size(), UINT64_MAX);
                output_hole_length_array.resize(rlbwt.rle_size(), UINT64_MAX);
                uint64_t rleSize = rlbwt.rle_size();
                std::stack<HoleStackData> stack;
                //std::vector<bool> checker;
                //checker.resize(rleSize, false);
                for (uint64_t i = 0; i < rleSize; i++)
                {
                    if (output_hole_length_array[i] == UINT64_MAX)
                    {
                        uint64_t runlength = rlbwt.get_run(i);
                        if (runlength == 1)
                        {
                            output_hole_pos_array[i] = rlbwt.get_lpos(i);
                            output_hole_length_array[i] = 0;
                        }
                        else
                        {
                            HoleStackData hsd;
                            hsd.hole_length = 1;
                            hsd.hole_pos = fpos_array[i];
                            hsd.lrun_index = i;
                            stack.push(hsd);
                        }
                    }
                    while (stack.size() > 0)
                    {
                        HoleStackData top = stack.top();
                        stack.pop();

                        uint64_t begin_lindex = rlbwt.get_lindex_containing_the_position(top.hole_pos);
                        uint64_t begin_diff = top.hole_pos - rlbwt.get_lpos(begin_lindex);

                        uint64_t end_pos = top.hole_pos + rlbwt.get_run(top.lrun_index) - 1;
                        uint64_t end_lindex = rlbwt.get_lindex_containing_the_position(end_pos);

                        if (begin_lindex == end_lindex)
                        {
                            if (output_hole_length_array[begin_lindex] == UINT64_MAX)
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
                        else
                        {

                            output_hole_pos_array[top.lrun_index] = top.hole_pos;
                            output_hole_length_array[top.lrun_index] = top.hole_length;
                        }
                    }
                }

                uint64_t max = 0;
                for (uint64_t i = 0; i < rleSize; i++)
                {
                    if(output_hole_length_array[i] > max){
                        max = output_hole_length_array[i];
                    }
                }
                std::cout << "max: " << max << std::endl;
            }
        };
        class RLBWTArrayFunctions
        {
        public:
            /*
    template <typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
    static uint64_t get_lpos(){

    }
    */

            template <typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
            static INDEX get_lindex_containing_the_position(const VEC &run_vec, INDEX lposition)
            {
                auto p = std::upper_bound(run_vec.begin(), run_vec.end(), lposition);
                INDEX pos = std::distance(run_vec.begin(), p) - 1;
                return pos;
            }

            template <typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
            static INDEX str_size(const VEC &run_vec)
            {
                return (run_vec)[run_vec.size() - 1];
            }
        };

        class Constructor
        {

        public:
            template <typename CHAR = char, typename INDEX = uint64_t>
            static void construct_from_bwt(RLBWT<std::vector<CHAR>, std::vector<INDEX>> &_rlbwt, std::string &text)
            {
                std::vector<INDEX> _pows;
                std::vector<CHAR> _char_vec;

                if (text.size() == 0)
                {
                    //return RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>>(_char_vec, _pows);
                }
                else
                {
                    char c = text[0];
                    INDEX pow = 1;
                    for (INDEX i = 1; i < text.size(); i++)
                    {
                        if (text[i] != text[i - 1])
                        {
                            _char_vec.push_back(c);
                            _pows.push_back(pow);
                            c = text[i];
                            pow = 1;
                        }
                        else
                        {
                            pow++;
                        }
                    }
                    _char_vec.push_back(c);
                    _pows.push_back(pow);

                    std::vector<INDEX> _run_vec = construct_run_vec(_pows);

                    //_rlbwt.char_vec = _char_vec;
                    //_rlbwt.run_vec = _run_vec;
                    _rlbwt.set(std::move(_char_vec), std::move(_run_vec));

                    //return RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>>(_char_vec, _run_vec);
                }
            }
            template <typename RLBWT_STR>
            static void construct_from_file(RLBWT_STR &rlbwt, std::string filepath)
            {
                using INDEX = typename RLBWT_STR::index_type;
                using CHAR = typename RLBWT_STR::char_type;
                std::vector<CHAR> cVec;
                std::vector<INDEX> nVec;
                itmmti::online_rlbwt_from_file(filepath, cVec, nVec, 1);
                rlbwt.set(std::move(cVec), std::move(nVec));
            }
            template <typename RLBWT_STR>
            static void construct_from_string(RLBWT_STR &rlbwt, std::string &text);

            template <typename CHAR = char, typename INDEX = uint64_t>
            static void construct_vectors_for_rlbwt(std::string &text, std::vector<CHAR> &output_char_vec, std::vector<INDEX> &output_run_vec)
            {
                itmmti::online_rlbwt(text, output_char_vec, output_run_vec, 1);
            }

            template <typename RLBWT_STR>
            static void construct_from_string(RLBWT_STR &rlbwt, std::string &&text)
            {

                using INDEX = typename RLBWT_STR::index_type;
                using CHAR = typename RLBWT_STR::char_type;

                std::vector<CHAR> cVec;
                std::vector<INDEX> nVec;
                itmmti::online_rlbwt(text, cVec, nVec, 1);
                rlbwt.set(std::move(cVec), std::move(nVec));

                //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
                //Constructor::construct_from_bwt<CHAR, INDEX>(rlbwt, bwt);
            }

            template <typename INDEX = uint64_t>
            static std::vector<INDEX> construct_run_vec(std::vector<INDEX> &_pows)
            {
                std::vector<INDEX> r;
                INDEX sum = 0;
                r.push_back(0);
                for (INDEX i = 0; i < _pows.size(); i++)
                {
                    sum += _pows[i];
                    r.push_back(sum);
                }
                return r;
            }
            template <typename CHAR = char, typename INDEX = uint64_t>
            static RLBWT<std::vector<CHAR>, std::vector<INDEX>> load_RLBWT_from_file(std::string filename)
            {

                std::ifstream inp;
                std::vector<CHAR> char_vec;

                inp.open(filename, std::ios::binary);
                bool inputFileExist = inp.is_open();
                if (!inputFileExist)
                {
                    std::cout << filename << " cannot open." << std::endl;

                    throw std::runtime_error("error");
                }

                inp.seekg(0, std::ios::end);
                INDEX n = (unsigned long)inp.tellg();
                inp.seekg(0, std::ios::beg);
                INDEX len = n / (sizeof(CHAR) + sizeof(INDEX));
                char_vec.resize(len);

                std::vector<INDEX> pows;
                pows.resize(len);
                inp.read((char *)&(char_vec)[0], len * sizeof(CHAR));
                inp.read((char *)&(pows)[0], len * sizeof(INDEX));

                std::vector<INDEX> run_vec = construct_run_vec(pows);
                inp.close();

                RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr;
                rlestr.set(std::move(char_vec), std::move(run_vec));
                rlestr.check_rlbwt();

                return rlestr;
            }
            template <typename CHAR = char, typename INDEX = uint64_t>
            static RLBWT<std::vector<CHAR>, stool::EliasFanoVector> load_RLBWT_from_file2(std::string filename)
            {
                //using INDEX = typename RLBWT<std::vector<CHAR>, stool::EliasFanoVector>::index_type;
                std::ifstream inp;
                std::vector<CHAR> char_vec;

                inp.open(filename, std::ios::binary);
                bool inputFileExist = inp.is_open();
                if (!inputFileExist)
                {
                    std::cout << filename << " cannot open." << std::endl;

                    throw std::runtime_error("error");
                }

                inp.seekg(0, std::ios::end);
                INDEX n = (unsigned long)inp.tellg();
                inp.seekg(0, std::ios::beg);
                INDEX len = n / (sizeof(CHAR) + sizeof(INDEX));
                char_vec.resize(len);

                std::vector<INDEX> pows;
                pows.resize(len);
                inp.read((char *)&(char_vec)[0], len * sizeof(CHAR));
                inp.read((char *)&(pows)[0], len * sizeof(INDEX));

                std::vector<INDEX> run_vec = construct_run_vec(pows);
                inp.close();

                stool::EliasFanoVector nVec2;
                nVec2.construct(&run_vec);

                RLBWT<std::vector<CHAR>, stool::EliasFanoVector> rlestr;
                rlestr.set(std::move(char_vec), std::move(nVec2));
                rlestr.check_rlbwt();
                return rlestr;
            }
        };

    } // namespace rlbwt
} // namespace stool