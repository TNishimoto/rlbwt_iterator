#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "rlbwt.hpp"
#include "backward_text.hpp"
#include "forward_sa.hpp"
#include "./sampling_lcp/practical_sampling_lcp_constructor.hpp"
#include "./sampling_lcp/succinct_slcp_constructor.hpp"
#include "./weiner/hyper_weiner.hpp"

#include "rlbwt_functions.hpp"

namespace stool
{
namespace rlbwt
{

template <typename VEC = std::vector<uint64_t>>
class ForwardLCPArray
{
  using SA_ITERATOR = typename ForwardSA<VEC>::iterator;

public:
  using INDEX = typename VEC::value_type;
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
  /*
    template <typename RLBWT_STR>
    static std::vector<INDEX> construct_sampling_lcp_array(RLBWT_STR &rlbwt, typename BackwardText<typename RLBWT_STR::char_type, INDEX>::iterator &&beginIt, typename BackwardText<typename RLBWT_STR::char_type, INDEX>::iterator &&endIt)
    {
      using CHAR = typename RLBWT_STR::char_type;
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
    */
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
  using SA = ForwardSA<VEC>;
  const ForwardSA<VEC> *_sa = nullptr;
  bool deleteFlag = false;

public:
  using const_iterator = iterator;

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
  void set(ForwardSA<VEC> &&__sa)
  {
    this->_sa = new ForwardSA<VEC>(std::move(__sa));
    this->deleteFlag = true;
  }
  void set(const ForwardSA<VEC> *__sa)
  {
    this->_sa = __sa;
    this->deleteFlag = false;
  }

  ~ForwardLCPArray()
  {
    this->clear();
  }

  void clear()
  {
    if (deleteFlag){
      delete _sa;
      deleteFlag = false;
    }
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
  INDEX size() const{
    return this->_sa->size();
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
  const ForwardSA<VEC>* get_ForwardSA() const {
    return this->_sa;
  }

  template <typename RLBWT_STR>
  void construct_from_rlbwt(const RLBWT_STR *_rlbwt, bool faster = false)
  {
    //using CHAR = typename RLBWT_STR::char_type;

    //INDEX _str_size = _rlbwt->str_size();
    //std::vector<INDEX> succ_slcp_lorder = PracticalSamplingLCPConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(*_rlbwt);
    //std::vector<INDEX> succ_slcp_lorder = SuccinctSLCPConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(*_rlbwt, false);
    std::vector<INDEX> succ_slcp_lorder = stool::rlbwt::HyperSamplingLCPArrayConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(*_rlbwt);

    std::pair<std::vector<INDEX>, std::vector<INDEX>> pairVec = SA::construct_sampling_sa(_rlbwt);
    std::vector<INDEX> _first_psa = std::move(pairVec.first);
    std::vector<INDEX> _last_psa = std::move(pairVec.second);
    INDEX _first_psa_value = _first_psa[0];

    //std::vector<INDEX> succ_slcp_yorder = PracticalSamplingLCPConstructor<RLBWT_STR>::construct_sampling_lcp_array(*_rlbwt, _last_psa);
    std::vector<INDEX> succ_slcp_yorder = PracticalSamplingLCPConstructor<RLBWT_STR>::to_succ_sampling_lcp_array_yorder(std::move(succ_slcp_lorder),*_rlbwt, _last_psa);

    auto _succ_ssa_yorder = SA::construct_succ_ssa_yorder(std::move(_first_psa), _last_psa);
    auto _sorted_end_ssa = SA::construct_sorted_end_ssa(std::move(_last_psa));

    auto _sa = ForwardSA<std::vector<INDEX>>(std::move(_sorted_end_ssa), std::move(_succ_ssa_yorder), std::move(succ_slcp_yorder), _first_psa_value, _rlbwt->str_size());

    if (faster)
    {
      this->set(std::move(_sa));
    }
    else
    {
      this->set(std::move(_sa));
    }
  }
  void print_info() const
  {
    this->_sa->print_info();
  }
};

} // namespace rlbwt
} // namespace stool