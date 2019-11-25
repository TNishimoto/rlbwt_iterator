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
#include "rlbwt_functions.hpp"
#include "sd_vector.hpp"

namespace stool
{
namespace rlbwt
{

template <typename RLBWT_STR>
void Constructor::construct_from_string(RLBWT_STR &rlbwt, std::string &text)
{
    using CHAR = typename RLBWT_STR::char_type;
    using INDEX = typename RLBWT_STR::index_type;
    using RUNVEC = typename RLBWT_STR::run_vec_type;

    std::vector<CHAR> cVec;
    std::vector<INDEX> nVec;
    itmmti::online_rlbwt(text, cVec, nVec, 1);

    rlbwt.set(std::move(cVec), std::move(nVec));
}
template void Constructor::construct_from_string(RLBWT<std::vector<char>, std::vector<uint64_t>> &, std::string &);

template<> void Constructor::construct_from_string(RLBWT<std::vector<char>, SDVectorSeq> &rlbwt, std::string &text)
{

    using RLBWT_STR = RLBWT<std::vector<char>, SDVectorSeq >;
    using CHAR = typename RLBWT_STR::char_type;
    using INDEX = typename RLBWT_STR::index_type;
    using RUNVEC = typename RLBWT_STR::run_vec_type;

    std::vector<CHAR> cVec;
    std::vector<INDEX> nVec;
    itmmti::online_rlbwt(text, cVec, nVec, 1);

            RUNVEC nVec2;
            nVec2.construct(nVec);
            rlbwt.set(std::move(cVec), std::move(nVec2));
}
template<> void Constructor::construct_from_string(RLBWT<std::vector<char>, stool::EliasFanoVector> &rlbwt, std::string &text)
{

    using RLBWT_STR = RLBWT<std::vector<char>, stool::EliasFanoVector>;
    using CHAR = typename RLBWT_STR::char_type;
    using INDEX = typename RLBWT_STR::index_type;
    using RUNVEC = typename RLBWT_STR::run_vec_type;

    std::vector<CHAR> cVec;
    std::vector<INDEX> nVec;
    itmmti::online_rlbwt(text, cVec, nVec, 1);

    RUNVEC nVec2;
    nVec2.construct(&nVec);
    rlbwt.set(std::move(cVec), std::move(nVec2));
}




} // namespace rlbwt
} // namespace stool