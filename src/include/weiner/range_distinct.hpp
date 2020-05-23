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
#include "stool/src/elias_fano_vector.hpp"
#include <sdsl/rmq_support.hpp> // include header for range minimum queries

namespace stool
{
    namespace rlbwt
    {
        template <typename INDEX_SIZE>
        struct WeinerInterval
        {
            INDEX_SIZE beginIndex;
            INDEX_SIZE beginDiff;
            INDEX_SIZE endIndex;
            INDEX_SIZE endDiff;

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
                return this->beginIndex == std::numeric_limits<INDEX_SIZE>::max();
            }

            static WeinerInterval get_special()
            {
                WeinerInterval r;
                r.beginIndex = std::numeric_limits<INDEX_SIZE>::max();
                return r;
            }
        };

        template <typename CHAR_VEC, typename INDEX_SIZE>
        class RangeDistinctDataStructure
        {
        private:
            using CHAR = typename CHAR_VEC::value_type;

            const CHAR_VEC *_char_vec;
            std::vector<stool::EliasFanoVector> positionVec;
            std::vector<uint64_t> rankVec;
            uint64_t size;
            sdsl::rmq_succinct_sada<> RMQ;
            sdsl::rmq_succinct_sada<> RmQ;
            std::vector<uint64_t> tmpRangeDistinctResult;
            std::stack<uint64_t> tmpSearchStack;

            //std::unordered_map<CHAR, uint64_t> tmpRangeDistinctResult;

            uint64_t get_next(uint64_t i)
            {
                CHAR c = (*_char_vec)[i];
                uint64_t rank = rankVec[i];
                if (positionVec[(uint8_t)c].size() == rank + 1)
                {
                    return UINT64_MAX;
                }
                else
                {
                    return positionVec[(uint8_t)c][rank + 1];
                }
            }
            int64_t get_prev(uint64_t i)
            {
                CHAR c = (*_char_vec)[i];
                uint64_t rank = rankVec[i];
                if (rank == 0)
                {
                    return -1;
                }
                else
                {
                    return positionVec[(uint8_t)c][rank - 1];
                }
            }
            std::vector<uint64_t> construct_next_vector()
            {
                std::vector<uint64_t> r;
                r.resize(size, 0);
                for (uint64_t i = 0; i < size; i++)
                {
                    r[i] = this->get_next(i);
                }
                return r;
            }
            std::vector<uint64_t> construct_rev_next_vector()
            {
                std::vector<uint64_t> r;
                r.resize(size, 0);
                for (uint64_t i = 0; i < size; i++)
                {
                    uint64_t p = this->get_next(i);
                    if (p == UINT64_MAX)
                    {
                        r[i] = 0;
                    }
                    else
                    {
                        r[i] = size - p;
                    }
                }
                return r;
            }

            std::vector<int64_t> construct_prev_vector()
            {
                std::vector<int64_t> r;
                r.resize(size, 0);
                for (uint64_t i = 0; i < size; i++)
                {
                    r[i] = this->get_prev(i);
                }
                return r;
            }

            void search_less(uint64_t x, uint64_t i, uint64_t j, std::stack<uint64_t> &output)
            {
                std::vector<uint64_t> r;
                uint64_t p = RmQ(i, j);
                int64_t value = this->get_prev(p);
                //std::cout << "RMQ[" << i << "," << j << "]=" << p << std::endl;
                if (value >= (int64_t)x)
                {
                    return;
                }
                else
                {
                    output.push(p);
                    if (p > i)
                    {
                        search_less(x, i, p - 1, output);
                    }

                    if (p < j)
                    {
                        search_less(x, p + 1, j, output);
                    }
                }
            }
            void search_than(uint64_t x, uint64_t i, uint64_t j, std::stack<uint64_t> &output)
            {
                std::vector<uint64_t> r;
                uint64_t p = RMQ(i, j);
                uint64_t value = this->get_next(p);
                //std::cout << "RMQ[" << i << "," << j << "]=" << p << std::endl;
                if (value <= x)
                {
                    return;
                }
                else
                {
                    output.push(p);
                    if (p > i)
                    {
                        search_than(x, i, p - 1, output);
                    }

                    if (p < j)
                    {
                        search_than(x, p + 1, j, output);
                    }
                }
            }

        public:
            RangeDistinctDataStructure()
            {
            }
            void preprocess(const CHAR_VEC *__char_vec)
            {
                int32_t charMaxSize = ((int32_t)UINT8_MAX) + 1;
                this->_char_vec = __char_vec;
                this->positionVec.resize(charMaxSize);

                std::vector<std::vector<uint64_t>> positionSeqVec;
                positionSeqVec.resize(charMaxSize, std::vector<uint64_t>());
                tmpRangeDistinctResult.resize(charMaxSize, 0);

                //std::unordered_map<CHAR, std::vector<uint64_t>> positionSeqMap;
                size = _char_vec->size();
                this->rankVec.resize(size, 0);

                for (uint64_t i = 0; i < size; i++)
                {
                    uint8_t c = (uint8_t)(*_char_vec)[i];
                    this->rankVec[i] = positionSeqVec[c].size();

                    positionSeqVec[c].push_back(i);
                }

                for (uint64_t i = 0; i < positionSeqVec.size(); i++)
                {
                    if (positionSeqVec[i].size() > 0)
                    {
                        positionVec[i].construct(&positionSeqVec[i]);
                    }
                }

                auto next_vec = this->construct_rev_next_vector();
                sdsl::rmq_succinct_sada<> next_rmq(&next_vec);
                this->RMQ.swap(next_rmq);

                next_vec.resize(0);
                next_vec.shrink_to_fit();

                auto prev_vec = this->construct_prev_vector();
                sdsl::rmq_succinct_sada<> prev_rmq(&prev_vec);
                this->RmQ.swap(prev_rmq);

            }
            std::vector<std::pair<uint64_t, uint64_t>> range_distinct(uint64_t i, uint64_t j)
            {
                std::vector<std::pair<uint64_t, uint64_t>> r;

                //std::vector<uint64_t> output;
                search_less(i, i, j, tmpSearchStack);
                while(tmpSearchStack.size() > 0){
                    uint64_t p = tmpSearchStack.top();
                    uint8_t c = (uint8_t)(*_char_vec)[p];
                    tmpRangeDistinctResult[c] = p;
                    tmpSearchStack.pop();
                }
                search_than(j, i, j, tmpSearchStack);

                while(tmpSearchStack.size() > 0){
                    uint64_t p = tmpSearchStack.top();
                    uint8_t c = (uint8_t)(*_char_vec)[p];
                    auto pair = std::pair<uint64_t, uint64_t>(tmpRangeDistinctResult[c], p);
                    r.push_back(pair);

                    tmpSearchStack.pop();
                }
                return r;
            }
        };
        template <typename RLBWT_STR, typename INDEX_SIZE>
        class RangeDistinctDataStructureOnRLBWT
        {
        public:
            using CHAR = typename RLBWT_STR::char_type;
            using CHAR_VEC = typename RLBWT_STR::char_vec_type;
            static std::vector<WeinerInterval<INDEX_SIZE>> range_distinct(const RLBWT_STR &_rlbwt, RangeDistinctDataStructure<CHAR_VEC, INDEX_SIZE> &rd, uint64_t &begin_lindex, uint64_t &begin_diff, uint64_t &end_lindex, uint64_t &end_diff)
            {

                vector<WeinerInterval<INDEX_SIZE>> r;

                vector<std::pair<uint64_t, uint64_t>> rangeVec = rd.range_distinct(begin_lindex, end_lindex);

                for (auto &it : rangeVec)
                {
                    //CHAR c = _rlbwt.get_char_by_run_index(it.first);
                    uint64_t cBeginIndex = it.first;
                    uint64_t cEndIndex = it.second;
                    uint64_t cBeginDiff = cBeginIndex == begin_lindex ? begin_diff : 0;
                    uint64_t cEndDiff = cEndIndex == end_lindex ? end_diff : _rlbwt.get_run(cEndIndex) - 1;

                    WeinerInterval<INDEX_SIZE> cInterval;
                    cInterval.beginIndex = cBeginIndex;
                    cInterval.beginDiff = cBeginDiff;
                    cInterval.endIndex = cEndIndex;
                    cInterval.endDiff = cEndDiff;

                    r.push_back(cInterval);
                }
                return r;
            }
        };

    } // namespace rlbwt
} // namespace stool