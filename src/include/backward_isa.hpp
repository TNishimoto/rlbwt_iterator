#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
//#include "lfitem.hpp"
#include "other_functions.hpp"
#include "rle_farray.hpp"
#include "rlbwt_functions.hpp"

namespace stool
{
namespace rlbwt
{


/*
    This class is the generator for BackwardISAIterator. 
  */
template <typename RUNVEC = std::vector<uint64_t>, typename VEC = std::vector<typename RUNVEC::value_type> >
class BackwardISA
{
/*
    This iterator enumerates ranks of suffixes in the input text in the back-to-front order of the text, 
    i.e., the i-th value is ISA[|T|-i], where T is the input text and ISA is the inverse suffix array of T.
  */
public:
  using INDEX = typename RUNVEC::value_type;

class iterator
{
private:
  const RUNVEC &_run_vec;
  const VEC &_findexes_lorder;
  const VEC *_rle_lf_lorder = nullptr;

  //const RLBWT_STR &_rlbwt;
  INDEX _diff = std::numeric_limits<INDEX>::max();
  INDEX _rle_lposition = std::numeric_limits<INDEX>::max();
  INDEX _end_lposition = std::numeric_limits<INDEX>::max();

public:
  iterator() = default;

  iterator(INDEX __rle_lposition, INDEX __diff, const RUNVEC &__run_vec, const VEC &__findexes_lorder)
      : _run_vec(__run_vec), _findexes_lorder(__findexes_lorder), _diff(__diff), _rle_lposition(__rle_lposition), _end_lposition(__rle_lposition)
  {
  }
  iterator(const RUNVEC &__run_vec, const VEC &__findexes_lorder) : _run_vec(__run_vec), _findexes_lorder(__findexes_lorder)
  {
  }

  INDEX fpos() const
  {
    return this->_findexes_lorder[_rle_lposition] + _diff;
  }
  /*
  CHAR character() const
  {
    return this->_rlbwt.get_char_vec()[this->_rle_lposition];
  }
  */
  void set_rle_lf_lorder(VEC *__rle_lf_lorder)
  {
    this->_rle_lf_lorder = __rle_lf_lorder;
  }

  INDEX lpos() const
  {
    return this->_run_vec(_rle_lposition) + _diff;
  }
  INDEX get_rle_lposition() const
  {
    return this->_rle_lposition;
  }
  INDEX get_diff() const
  {
    return this->_diff;
  }

  iterator &operator++()
  {
    if (this->_rle_lf_lorder == nullptr)
    {
      //INDEX next_lpos = this->_rlbwt.get_lindex_containing_the_position(this->fpos());
      INDEX next_lpos = RLBWTArrayFunctions::get_lindex_containing_the_position(this->_run_vec, this->fpos());

      INDEX x = this->_run_vec[next_lpos];
      _diff = this->fpos() - x;
      _rle_lposition = next_lpos;
      if (_rle_lposition == this->_end_lposition)
      {
        _rle_lposition = std::numeric_limits<INDEX>::max();
        _diff = std::numeric_limits<INDEX>::max();
      }
      return *this;
    }
    else
    {
      INDEX findex = this->fpos();
      INDEX start_pos = (*this->_rle_lf_lorder)[this->_rle_lposition];
      int64_t j = SortedVec<RUNVEC >::pred_by_linear_search(this->_run_vec, findex, start_pos);
      assert(j != -1);
      _diff = findex - this->_run_vec[j];
      _rle_lposition = j;
      if (_rle_lposition == this->_end_lposition)
      {
        _rle_lposition = std::numeric_limits<INDEX>::max();
        _diff = std::numeric_limits<INDEX>::max();
      }

      return *this;
    }
  }
  INDEX operator*() const
  {
    return this->fpos();
  }
  bool operator!=(const iterator &rhs) const
  {
    return (_rle_lposition != rhs._rle_lposition) || (_diff != rhs._diff);
  }
  static std::vector<INDEX> construct_suffix_array(INDEX str_size, iterator &&beginIt, iterator &&endIt)
  {
    std::vector<INDEX> r;
    r.resize(str_size, std::numeric_limits<INDEX>::max());
    INDEX x = str_size - 1;
    while (beginIt != endIt)
    {
      r[beginIt.fpos()] = x;
      --x;
      ++beginIt;
    }
    return r;
  }
  INDEX str_size() const
  {
    return this->_rlbwt.str_size();
  }
  bool is_end() const
  {
    return _rle_lposition == std::numeric_limits<INDEX>::max() && _diff == std::numeric_limits<INDEX>::max();
  }
};

private:
  //const RLBWT_STR *_rlbwt = nullptr;
  const RUNVEC *_run_vec = nullptr;
  const VEC *_findexes_lorder = nullptr;
  VEC *_rle_lf_lorder = nullptr;
  INDEX _rle_end_lposition;
  bool deleteFlag = false;

public:
  using const_iterator = iterator;
  BackwardISA()
  {
  }
  /*
  BackwardISA(const RLBWT_STR *__rlbwt, VEC *&__findexes_lorder, VEC *&__rle_lf_lorder, INDEX __end_character_position)
      : _rlbwt(__rlbwt), _findexes_lorder(__findexes_lorder), _rle_lf_lorder(__rle_lf_lorder), _rle_end_lposition(__rlbwt->get_end_rle_lposition()), 
      ,deleteFlag(true)
  {
  }
  BackwardISA(const RLBWT_STR *__rlbwt, VEC *&__findexes_lorder)
      : _rlbwt(__rlbwt), _findexes_lorder(__findexes_lorder), _rle_lf_lorder(nullptr), _rle_end_lposition(__rlbwt->get_end_rle_lposition()), deleteFlag(true)
  {
  }
  BackwardISA(const RLBWT_STR *__rlbwt, VEC &&__findexes_lorder)
      : _rlbwt(__rlbwt), _findexes_lorder(new VEC(__findexes_lorder)), _rle_end_lposition(__rlbwt->get_end_rle_lposition()), deleteFlag(true)
  {
  }
  BackwardISA(const RLBWT_STR *__rlbwt, VEC &&__findexes_lorder, VEC &&__rle_lf_lorder)
      : _rlbwt(__rlbwt), _findexes_lorder(new VEC(__findexes_lorder)), _rle_lf_lorder(new VEC(__rle_lf_lorder)), _rle_end_lposition(__rlbwt->get_end_rle_lposition()), deleteFlag(true)
  {
  }
  */

  void set(const RUNVEC *__run_vec, VEC &&__findexes_lorder, INDEX __end_rle_lposition)
  {
    this->_run_vec = __run_vec;
    this->_findexes_lorder = new VEC(std::move(__findexes_lorder));
    this->_rle_end_lposition = __end_rle_lposition;
    //this->_end_character_position = 0;
    this->deleteFlag = true;
  }
  void set(const RUNVEC *__run_vec, VEC &&__findexes_lorder, VEC &&__rle_lf_lorder, INDEX __end_rle_lposition)
  {
    this->_run_vec = __run_vec;
    this->_findexes_lorder = new VEC(std::move(__findexes_lorder));
    this->_rle_lf_lorder = new VEC(std::move(__rle_lf_lorder));
    this->_rle_end_lposition = __end_rle_lposition;
    this->deleteFlag = true;
  }

  BackwardISA(const BackwardISA &obj)
  {
    throw std::logic_error("BackwardISA instances cannot call the copy constructor.");
    std::cout << obj.str_size() << std::endl;
  }

  BackwardISA(BackwardISA &&obj)
  {
    //this->_rlbwt = obj._rlbwt;
    this->_run_vec = obj._run_vec;

    this->_findexes_lorder = obj._findexes_lorder;
    this->_rle_lf_lorder = obj._rle_lf_lorder;
    this->_rle_end_lposition = obj._rle_end_lposition;
    this->deleteFlag = obj.deleteFlag;

    obj.deleteFlag = false;
  }

  ~BackwardISA()
  {
    //std::cout << "call destructor Backward ISA!" << std::endl;
    this->clear();
  }
  void clear()
  {
    if (deleteFlag)
    {
      delete this->_findexes_lorder;
      if (_rle_lf_lorder != nullptr)
        delete this->_rle_lf_lorder;
      deleteFlag = false;
    }
  }
  VEC &sampling_findexes()
  {
    return this->_findexes_lorder;
  }

  iterator begin() const
  {
    auto p = iterator(this->_rle_end_lposition, 0, *this->_run_vec, *_findexes_lorder);
    if (_rle_lf_lorder != nullptr)
    {
      p.set_rle_lf_lorder(this->_rle_lf_lorder);
    }
    return p;
  }
  iterator end() const
  {
    return iterator(*this->_run_vec, *_findexes_lorder);
  }
  std::vector<INDEX> to_isa() const
  {
    std::vector<INDEX> r;
    INDEX size = this->str_size();
    r.resize(size);
    INDEX p = size;
    for (INDEX c : *this)
    {
      r[--p] = c;
    }
    assert(p == 0);
    return r;
  }
  INDEX str_size() const
  {
    return RLBWTArrayFunctions::str_size<INDEX, RUNVEC>(*this->_run_vec);
  }

  template <typename RLBWT_STR>
  void construct_from_rlbwt(const RLBWT_STR *_rlbwt, bool faster = false)
  {
    if (faster)
    {
      this->set(_rlbwt->get_run_vec(), RLBWTFunctions::construct_fpos_array(*_rlbwt), RLBWTFunctions2::construct_rle_lf_lorder<RLBWT_STR>(*_rlbwt), _rlbwt->get_end_rle_lposition());
    }
    else
    {
      this->set(_rlbwt->get_run_vec(), RLBWTFunctions::construct_fpos_array(*_rlbwt), _rlbwt->get_end_rle_lposition());
    }
  }
};

} // namespace rlbwt
} // namespace stool