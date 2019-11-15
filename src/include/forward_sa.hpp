#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "rlbwt.hpp"
#include "backward_text.hpp"


namespace stool
{
namespace rlbwt
{

/*
class DataStructuresForSA
{
  
};
*/

template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX>>
class ForwardSA
{
public:
  /*
    This iterator enumerates the suffix array of the original input text, 
    i.e., the i-th value is SA[i], where SA is the suffix array of the original input text.
  */
class iterator
{
private:
  INDEX _tIndex = std::numeric_limits<INDEX>::max();
  INDEX _saIndex = std::numeric_limits<INDEX>::max();
  INDEX _max_sa_index = std::numeric_limits<INDEX>::max();
  INDEX _lcp = 0;
  INDEX _xindex = std::numeric_limits<INDEX>::max();

public:
  const VEC &_sorted_end_ssa;
  const VEC &_succ_ssa_yorder;
  const VEC *_succ_slcp_yorder;
  const VEC *_cache = nullptr;

  iterator() = default;

  iterator(INDEX __tIndex, INDEX __saIndex, INDEX __max_sa_index, const VEC &__sorted_end_ssa, const VEC &__succ_ssa_yorder, const VEC *__succ_slcp_yorder)
      : _tIndex(__tIndex), _saIndex(__saIndex), _max_sa_index(__max_sa_index), _sorted_end_ssa(__sorted_end_ssa), _succ_ssa_yorder(__succ_ssa_yorder), _succ_slcp_yorder(__succ_slcp_yorder)
  {
  }
  iterator(const VEC &__sorted_end_ssa, const VEC &__succ_ssa_yorder) : _sorted_end_ssa(__sorted_end_ssa), _succ_ssa_yorder(__succ_ssa_yorder)
  {
  }

  INDEX get_sa_index()
  {
    return this->_saIndex;
  }
  INDEX get_text_index()
  {
    return this->_tIndex;
  }
  INDEX get_lcp()
  {
    return this->_lcp;
  }
  INDEX get_text_size()
  {
    return this->_max_sa_index + 1;
  }
  void set_cache(const VEC *__cache)
  {
    this->_cache = __cache;
  }

private:
  INDEX successor_on_sorted_end_ssa()
  {
    if (this->_cache != nullptr && this->_xindex != std::numeric_limits<INDEX>::max())
    {
      INDEX end_pos = (*this->_cache)[this->_xindex];
      auto p = SortedVec<std::vector<INDEX>>::succ_by_back_linear_search(_sorted_end_ssa, this->_tIndex, end_pos);
      return p;
    }
    else
    {
      auto p = std::lower_bound(_sorted_end_ssa.begin(), _sorted_end_ssa.end(), this->_tIndex);
      INDEX r = distance(_sorted_end_ssa.begin(), p);
      return r;
    }
  }
  INDEX successor_on_SA(INDEX current_xindex)
  {
    //INDEX xindex = successor_on_sorted_end_ssa();
    INDEX saq = _sorted_end_ssa[current_xindex];
    INDEX r;

    //INDEX fIndexSucc = fIndex < (_first_psa.size() - 1) ? fIndex + 1 : 0;
    if (_tIndex == saq)
    {
      r = _succ_ssa_yorder[current_xindex];
    }
    else
    {
      INDEX saqm = _succ_ssa_yorder[current_xindex];
      r = saqm - (saq - _tIndex);
    }
    return r;
  }

  INDEX compute_next_lcp(INDEX current_xindex)
  {

    INDEX saq = _sorted_end_ssa[current_xindex];
    INDEX r;

    //INDEX fIndexSucc = fIndex < (_first_psa.size() - 1) ? fIndex + 1 : 0;
    if (_tIndex == saq)
    {
      r = (*_succ_slcp_yorder)[current_xindex];
    }
    else
    {
      INDEX lcp1 = (*_succ_slcp_yorder)[current_xindex];
      r = lcp1 + (saq - _tIndex);
    }

    return r;

    return std::numeric_limits<INDEX>::max();
  }

public:
  iterator &operator++()
  {
    if (this->_saIndex < _max_sa_index)
    {
      this->_saIndex++;
      this->_xindex = this->successor_on_sorted_end_ssa();

      if (this->_succ_slcp_yorder != nullptr)
      {
        _lcp = this->compute_next_lcp(_xindex);
      }

      this->_tIndex = this->successor_on_SA(_xindex);
    }
    else
    {
      this->_saIndex = std::numeric_limits<INDEX>::max();
      this->_tIndex = std::numeric_limits<INDEX>::max();
    }
    return *this;
  }
  INDEX operator*()
  {
    return this->_tIndex;
  }
  bool operator!=(const iterator &rhs)
  {
    return (_tIndex != rhs._tIndex) || (_saIndex != rhs._saIndex);
  }

  template <typename CHAR = char>
  static std::pair<std::vector<INDEX>, std::vector<INDEX>> construct_sampling_sa_lorder(const RLBWT<CHAR> &rlbwt, typename BackwardISA<INDEX, VEC>::iterator &&beginIt, typename BackwardISA<INDEX, VEC>::iterator &&endIt)
  {
    //std::cout << "Constructing sampled suffix array" << std::flush;

    std::pair<std::vector<INDEX>, std::vector<INDEX>> r;
    std::vector<INDEX> &beginVec = r.first;
    std::vector<INDEX> &endVec = r.second;
    //RLBWT<CHAR> &rlbwt = back.get_rlbwt();

    //vector<INDEX> mapper = RLBWTFunctions<CHAR>::construct_rle_lf_mapper(rlbwt);

    //vector<INDEX> beginVec, _last_psa;
    INDEX size = rlbwt.rle_size();
    beginVec.resize(size, std::numeric_limits<INDEX>::max());
    endVec.resize(size, std::numeric_limits<INDEX>::max());

    int64_t saValue = rlbwt.str_size();

    //INDEX counter = 0;
    for (auto p = beginIt; p != endIt; ++p)
    {
      /*
      if (counter != 0)
      {
        --counter;
      }
      else
      {
        std::cout << "." << std::flush;
        counter = 10000000;
      }
      */

      saValue--;
      INDEX lIndex = p.get_rle_lposition();
      //INDEX fIndex = p.get_rle_fposition(); //mapper[lIndex];
      INDEX pow = rlbwt.get_run(lIndex);
      INDEX diff = p.get_diff();
      if (diff == 0)
      {
        beginVec[lIndex] = saValue;
      }
      if (diff + 1 == pow)
      {
        endVec[lIndex] = saValue;
      }
    }
    //std::cout << "[END]" << std::endl;
    return r;
  }
  /*
  template <typename CHAR = char>
  static std::pair<vector<INDEX>, vector<INDEX>> construct_sampling_sa(RLBWT<CHAR> &rlbwt, BackwardIiterator<CHAR> &&beginIt, BackwardIiterator<CHAR> &&endIt)
  {
    std::pair<vector<INDEX>, vector<INDEX>> r = construct_sampling_sa_lorder(rlbwt, std::move(beginIt), std::move(endIt));
    vector<INDEX> mapper = RLBWTFunctions::construct_rle_lf_mapper<INDEX>(rlbwt);
    r.first = stool::rlbwt::permutate(std::move(r.first), mapper);
    r.second = stool::rlbwt::permutate(std::move(r.second), mapper);

    return r;
  }
  */
 /*
  template <typename CHAR = char>
  static vector<INDEX> construct_sa(RLBWT<CHAR> &rlbwt, BackwardISAIterator<CHAR> &&beginIt, BackwardISAIterator<CHAR> &&endIt)
  {

    INDEX size = rlbwt.str_size();
    vector<INDEX> sa;
    sa.resize(size, std::numeric_limits<INDEX>::max());
    INDEX saValue = size;
    for (auto p = beginIt; p != endIt; ++p)
    {
      saValue--;
      sa[p.fpos()] = saValue;
    }
    return sa;
  }
  */
  static std::vector<INDEX> construct_cache(std::vector<INDEX> &_sorted_end_ssa, std::vector<INDEX> &_succ_ssa_yorder)
  {
    std::vector<INDEX> tmp, r;
    tmp.resize(_sorted_end_ssa.size());
    for (INDEX i = 0; i < tmp.size(); i++)
    {
      tmp[i] = i;
    }
    std::sort(tmp.begin(), tmp.end(), [&](const INDEX &x, const INDEX &y) {
      return _succ_ssa_yorder[x] < _succ_ssa_yorder[y];
    });
    r.resize(_sorted_end_ssa.size());
    INDEX k = 0;
    for (INDEX p : tmp)
    {
      while (_sorted_end_ssa[k] < _succ_ssa_yorder[p])
      {
        k++;
      }
      r[p] = k;
    }
    return r;
  }
  static INDEX get_cache_miss_statics(std::vector<INDEX> &_sorted_end_ssa, std::vector<INDEX> &_succ_ssa_yorder, std::vector<INDEX> &_cache)
  {
    INDEX sum = 0;
    for (INDEX i = 0; i < _sorted_end_ssa.size(); i++)
    {
      INDEX pred = i == 0 ? 0 : _sorted_end_ssa[i - 1];
      int64_t range = _sorted_end_ssa[i] - pred - 1;
      //auto p = std::lower_bound(_sorted_end_ssa.begin(), _sorted_end_ssa.end(), _succ_ssa_yorder[i]);
      //INDEX r = distance(_sorted_end_ssa.begin(), p);
      INDEX r = _cache[i];
      //std::cout << r << std::endl;
      //assert(_cache[i] == r);
      INDEX k = 0;
      while (range > 0)
      {
        if (k != 0)
        {
          pred = r == 0 ? 0 : _sorted_end_ssa[r - 1];
          int64_t diff = _sorted_end_ssa[r] - pred;
          int64_t q = diff > range ? range : diff;
          sum += q * k;
          range -= q;
        }
        else
        {
          pred = r == 0 ? 0 : _sorted_end_ssa[r - 1];
          int64_t diff = _succ_ssa_yorder[i] - pred;
          int64_t q = diff > range ? range : diff;
          sum += q * k;
          range -= q;
        }
        k++;
      }
    }
    return sum;
  }
  static std::vector<INDEX> decompress(RLBWT<char> &rlbwt, iterator &&begIt, iterator &&endIt)
  {
    std::vector<INDEX> sa;

    INDEX size = rlbwt.str_size();

    sa.resize(size, std::numeric_limits<INDEX>::max());
    int64_t x = 0;
    //std::cout << "Decompressing suffix array" << std::flush;
    //INDEX counter = 0;
    while (begIt != endIt)
    {
      /*
      if (counter != 0)
      {
        --counter;
      }
      else
      {
        std::cout << "." << std::flush;
        counter = 10000000;
      }
      */
      sa[x++] = *begIt;
      ++begIt;
    }
    //std::cout << "[END]" << std::endl;

    return sa;
  }
};
private:
  //RLBWT<CHAR> &_rlbwt;

  const VEC *_sorted_end_ssa = nullptr;
  const VEC *_succ_ssa_yorder = nullptr;
  const VEC *_succ_slcp_yorder = nullptr;
  const VEC *_cache = nullptr;

  INDEX _first_psa_value;
  INDEX _str_size;
  bool deleteFlag = false;

public:
  ForwardSA()
  {
  }
  ForwardSA(VEC &&__sorted_end_ssa, VEC &&__succ_ssa_yorder, INDEX __first_psa_value, INDEX str_size)
      : _sorted_end_ssa(new VEC(std::move(__sorted_end_ssa))), _succ_ssa_yorder(std::move(new VEC(__succ_ssa_yorder))), _first_psa_value(__first_psa_value), _str_size(str_size), deleteFlag(true)
  {
  }
  ForwardSA(VEC &&__sorted_end_ssa, VEC &&__succ_ssa_yorder, VEC &&__succ_slcp_yorder, INDEX __first_psa_value, INDEX str_size)
      : _sorted_end_ssa(new VEC(std::move(__sorted_end_ssa))), _succ_ssa_yorder(std::move(new VEC(__succ_ssa_yorder))), _succ_slcp_yorder(std::move(new VEC(__succ_slcp_yorder))), _first_psa_value(__first_psa_value), _str_size(str_size), deleteFlag(true)
  {
  }
  void set(VEC &&__sorted_end_ssa, VEC &&__succ_ssa_yorder, INDEX __first_psa_value, INDEX str_size)
  {
    this->_sorted_end_ssa = new VEC(std::move(__sorted_end_ssa));
    this->_succ_ssa_yorder = new VEC(std::move(__succ_ssa_yorder));
    this->_first_psa_value = __first_psa_value;
    this->_str_size = str_size;
    deleteFlag = true;
  }
  void set(VEC &&__sorted_end_ssa, VEC &&__succ_ssa_yorder, VEC &&__succ_slcp_yorder, INDEX __first_psa_value, INDEX str_size)
  {
    this->_sorted_end_ssa = new VEC(std::move(__sorted_end_ssa));
    this->_succ_ssa_yorder = new VEC(std::move(__succ_ssa_yorder));
    this->_succ_slcp_yorder = new VEC(std::move(__succ_slcp_yorder));
    this->_first_psa_value = __first_psa_value;
    this->_str_size = str_size;
    deleteFlag = true;
  }
  void construct(VEC* __sorted_end_ssa, VEC* __succ_ssa_yorder, INDEX __first_psa_value, INDEX str_size)
  {
    this->_sorted_end_ssa = __sorted_end_ssa;
    this->_succ_ssa_yorder = __succ_ssa_yorder;
    this->_first_psa_value = __first_psa_value;
    this->_str_size = str_size;
    deleteFlag = false;
  }

  /*
  ForwardSA(RLBWT<CHAR> &__rlbwt, bool faster = false)
  {
    _str_size = __rlbwt.str_size();
    this->construct(__rlbwt, faster);
  }
  */

  ForwardSA(ForwardSA &&obj)
  {
    this->_sorted_end_ssa = obj._sorted_end_ssa;
    this->_succ_ssa_yorder = obj._succ_ssa_yorder;
    this->_succ_slcp_yorder = obj._succ_slcp_yorder;
    this->_cache = obj._cache;
    this->_first_psa_value = obj._first_psa_value;
    this->_str_size = obj._str_size;
    this->deleteFlag = obj.deleteFlag;
    obj.deleteFlag = false;
  }
  ForwardSA(const ForwardSA &obj)
  {
    if (obj._sorted_end_ssa != nullptr)
    {
      throw std::logic_error("ForwardSA instances cannot call the copy constructor.");
    }
  }

  ~ForwardSA()
  {
    //std::cout << "call destructor Forward ISA!" << std::endl;

    if (deleteFlag)
    {

      if (_sorted_end_ssa != nullptr)
        delete _sorted_end_ssa;
      if (_succ_ssa_yorder != nullptr)
        delete _succ_ssa_yorder;
      if (_succ_slcp_yorder != nullptr)
        delete _succ_slcp_yorder;
      if (_cache != nullptr)
        delete _cache;
    }
  }
  /*
  ForwardSA(RLBWT<CHAR> &__rlbwt, vector<INDEX> &&__succ_slcp_yorder, bool faster = false)
      : _succ_slcp_yorder(__succ_slcp_yorder)
  {
    _str_size = __rlbwt.str_size();
    this->construct(__rlbwt, faster);
  }
*/

  iterator begin() const
  {
    auto it = iterator(_first_psa_value, 0, _str_size - 1, *this->_sorted_end_ssa, *this->_succ_ssa_yorder, this->_succ_slcp_yorder);
    if (this->_cache != nullptr)
    {
      it.set_cache(this->_cache);
    }
    return it;
  }
  iterator end() const
  {
    return iterator(*this->_sorted_end_ssa, *this->_succ_ssa_yorder);
  }
  std::vector<INDEX> to_sa() const
  {
    std::vector<INDEX> r;
    r.resize(_str_size);
    INDEX p = 0;
    for (INDEX c : *this)
    {
      r[p++] = c;
    }
    return r;
  }
  INDEX str_size() const
  {
    return this->_str_size;
  }
  INDEX size() const{
    return this->_str_size;
  }
std::vector<INDEX> copy_slcp_array() const 
  {
    std::vector<INDEX> r;
    r.resize(this->_succ_slcp_yorder->size() );
    INDEX p = 0;
    for (INDEX c : *this->_succ_slcp_yorder)
    {
      r[p++] = c;
    }
    return r;
  }

  template <typename RLBWT_STR>
  void construct_from_rlbwt(const RLBWT_STR *_rlbwt, bool faster = false)
  {
    std::pair<std::vector<INDEX>, std::vector<INDEX>> pairVec = ForwardSA<INDEX,VEC>::construct_sampling_sa<RLBWT_STR>(_rlbwt);
    //__rlbwt.clear();

    std::vector<INDEX> _first_psa = std::move(pairVec.first);
    std::vector<INDEX> _last_psa = std::move(pairVec.second);
    INDEX _first_psa_value = _first_psa[0];

    auto _succ_ssa_yorder = ForwardSA<INDEX,VEC>::construct_succ_ssa_yorder(std::move(_first_psa), _last_psa);
    auto _sorted_end_ssa = ForwardSA<INDEX,VEC>::construct_sorted_end_ssa(std::move(_last_psa));

    if (faster)
    {
      this->set(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), _first_psa_value, _rlbwt->str_size());
      //auto = _cache = SAIterator<>::construct_cache(this->_sorted_end_ssa, this->_succ_ssa_yorder);
      //return ForwardSA<INDEX, vector<INDEX>>(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), _first_psa_value, _rlbwt->str_size());
    }
    else
    {
      this->set(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), _first_psa_value, _rlbwt->str_size());
      //return ForwardSA<INDEX, vector<INDEX>>(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), _first_psa_value, _rlbwt->str_size());
    }
  }

  public:
  template <typename RLBWT_STR>
  static std::pair<std::vector<INDEX>, std::vector<INDEX>> construct_sampling_sa(const RLBWT_STR *rlbwt)
  {
    BackwardISA<INDEX, std::vector<INDEX>> tpb;
    tpb.construct_from_rlbwt(rlbwt);
    std::pair<std::vector<INDEX>, std::vector<INDEX>> r = iterator::construct_sampling_sa_lorder(*rlbwt, tpb.begin(), tpb.end());

    tpb.clear();

    std::vector<INDEX> mapper = RLBWTFunctions::construct_rle_lf_mapper<INDEX>(*rlbwt);
    r.first = stool::rlbwt::permutate(std::move(r.first), mapper);
    r.second = stool::rlbwt::permutate(std::move(r.second), mapper);

    return r;
  }
  //template <typename INDEX = uint64_t>
  static std::vector<INDEX> construct_sorted_end_ssa(std::vector<INDEX> &&_last_psa)
  {
    std::sort(_last_psa.begin(), _last_psa.end());
    return std::move(_last_psa);
  }
  //template <typename INDEX = uint64_t>
  static std::vector<INDEX> construct_succ_ssa_yorder(std::vector<INDEX> &&_first_psa, std::vector<INDEX> &_last_psa)
  {
    std::vector<INDEX> yf_mapper = get_sorted_positions(_last_psa);
    std::vector<INDEX> fy_mapper = change_inv(std::move(yf_mapper));
    std::vector<INDEX> succ_ssa = stool::rlbwt::rotate(std::move(_first_psa));
    auto _succ_ssa_yorder = stool::rlbwt::permutate(std::move(succ_ssa), fy_mapper);
    return _succ_ssa_yorder;
  }
};

} // namespace rlbwt
} // namespace stool