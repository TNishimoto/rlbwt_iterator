#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "rlbwt.hpp"
#include "backward_text.hpp"
#include "forward_sa.hpp"
#include "sampling_lcp.hpp"

namespace stool
{
namespace rlbwt
{

template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX>>
class ForwardLCPArray
{
  using SA_ITERATOR = typename ForwardSA<INDEX, VEC>::iterator;

public:
  /*
    This iterator enumerates the LCP array of the original input text, 
    i.e., the i-th value is LCP[i], where LCP is the LCP array of the original input text.
  */
  //template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX> >
  class iterator
  {
  private:
    SA_ITERATOR _sa_iterator;

  public:
    iterator() = default;
    iterator(SA_ITERATOR &__sa_iterator) : _sa_iterator(__sa_iterator)
    {
    }
    INDEX get_sa_index()
    {
      return this->_sa_iterator.get_sa_index();
    }
    INDEX get_text_index()
    {
      return this->_sa_iterator.get_text_index();
    }
    INDEX get_lcp()
    {
      return this->_sa_iterator.get_lcp();
    }
    INDEX get_text_size()
    {
      return this->_sa_iterator.get_text_size();
    }

    iterator &operator++()
    {
      ++(this->_sa_iterator);
      return *this;
    }
    INDEX operator*()
    {
      return this->_sa_iterator.get_lcp();
    }
    bool operator!=(const iterator &rhs)
    {
      bool b = (_sa_iterator) != (rhs._sa_iterator);
      return b;
    }

    template <typename CHAR = char>
    static std::vector<INDEX> construct_sampling_lcp_array(RLBWT<CHAR> &rlbwt, typename BackwardText<CHAR, INDEX>::iterator &&beginIt, typename BackwardText<CHAR, INDEX>::iterator &&endIt)
    {
      std::vector<INDEX> lcp_vec = construct_lcp_array(rlbwt, std::forward<typename BackwardText<CHAR, INDEX>::iterator>(beginIt), std::forward<typename BackwardText<CHAR, INDEX>::iterator>(endIt));

      std::vector<INDEX> mapper = rlbwt.construct_rle_fl_mapper();

      std::vector<INDEX> samp_lcp_vec;
      samp_lcp_vec.resize(mapper.size(), 0);
      INDEX u = 0;
      for (INDEX i = 1; i < samp_lcp_vec.size(); i++)
      {
        u += rlbwt.get_run(mapper[i - 1]);
        samp_lcp_vec[i] = lcp_vec[u];
      }
      return samp_lcp_vec;
    }
    /*
  template <typename CHAR = char>
  static std::vector<INDEX> construct_lcp_array(RLBWT<CHAR> &rlbwt, BackwardTextIterator<CHAR> &&beginIt, BackwardTextIterator<CHAR> &&endIt)
  {
    string str = BackwardTextIterator<CHAR>::decompress(rlbwt, std::forward<BackwardTextIterator<CHAR>>(beginIt), std::forward<BackwardTextIterator<CHAR>>(endIt), false);
    std::vector<INDEX> sa = SuffixArrayConstructor::construct_sa(str);
    std::vector<INDEX> isa = SuffixArrayConstructor::construct_isa(sa);
    std::vector<INDEX> lcp_vec = SuffixArrayConstructor::construct_lcp(str, sa, isa);
    return lcp_vec;
  }
  */
  };
  using SA = ForwardSA<INDEX, VEC>;
  const ForwardSA<INDEX, VEC> *_sa = nullptr;
  bool deleteFlag = false;

public:
  ForwardLCPArray()
  {
  }

  ForwardLCPArray(SA &&__sa) : _sa(new SA(std::move(__sa))), deleteFlag(true)
  {
  }
  ForwardLCPArray(ForwardLCPArray &&obj)
  {
    this->_sa = obj._sa;
    this->deleteFlag = obj.deleteFlag;
    obj.deleteFlag = false;
  }
  ForwardLCPArray(const ForwardLCPArray &obj)
  {
    if (obj._sa != nullptr)
    {
      throw std::logic_error("ForwardLCPArray instances cannot call the copy constructor.");
    }
  }
  void set(ForwardSA<INDEX, VEC> &&__sa)
  {
    this->_sa = new ForwardSA<INDEX, VEC>(std::move(__sa));
    this->deleteFlag = true;
  }
  void set(const ForwardSA<INDEX, VEC> *__sa)
  {
    this->_sa = __sa;
    this->deleteFlag = false;
  }

  ~ForwardLCPArray()
  {
    if (deleteFlag)
      delete _sa;
  }
  iterator begin() const
  {
    auto it = this->_sa->begin();
    return iterator(it);
  }
  iterator end() const
  {
    auto it = this->_sa->end();
    return iterator(it);
  }
  std::vector<INDEX> to_lcp_array() const
  {
    std::vector<INDEX> r;
    r.resize(this->_sa->str_size());
    INDEX p = 0;
    for (INDEX c : *this)
    {
      r[p++] = c;
    }
    return r;
  }
  std::vector<INDEX> copy_slcp_array() const
  {
    return this->_sa->copy_slcp_array();
  }
  const ForwardSA<INDEX,VEC>* get_ForwardSA() const {
    return this->_sa;
  }

  template <typename CHAR, typename CHARVEC, typename POWVEC>
  void construct_from_rlbwt(const RLBWT<CHAR, INDEX, CHARVEC, POWVEC> *_rlbwt, bool faster = false)
  {

    //INDEX _str_size = _rlbwt->str_size();
    std::pair<std::vector<INDEX>, std::vector<INDEX>> pairVec = SA::construct_sampling_sa(_rlbwt);
    std::vector<INDEX> _first_psa = std::move(pairVec.first);
    std::vector<INDEX> _last_psa = std::move(pairVec.second);
    INDEX _first_psa_value = _first_psa[0];

    std::vector<INDEX> succ_slcp_yorder = SamplingLCP<CHAR>::construct_sampling_lcp_array(*_rlbwt, _last_psa);

    auto _succ_ssa_yorder = SA::construct_succ_ssa_yorder(std::move(_first_psa), _last_psa);
    auto _sorted_end_ssa = SA::construct_sorted_end_ssa(std::move(_last_psa));

    auto _sa = ForwardSA<INDEX, std::vector<INDEX>>(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), std::move(succ_slcp_yorder), _first_psa_value, _rlbwt->str_size());

    if (faster)
    {
      this->set(std::move(_sa));
    }
    else
    {
      this->set(std::move(_sa));
    }
  }
};

} // namespace rlbwt
} // namespace stool