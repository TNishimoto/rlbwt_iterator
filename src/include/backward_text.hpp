#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "backward_isa.hpp"
namespace stool
{
namespace rlbwt
{

/*
    This class is the generator for BackwardTextIterator. 
  */
template <typename CHARVEC = std::vector<char>, typename POWVEC = std::vector<uint64_t>, typename VEC = std::vector<uint64_t>>
class BackwardText
{
  using ISA_IT = typename BackwardISA<POWVEC, VEC>::iterator;

  /*
    This iterator enumerates the original input text in the back-to-front order of the text, 
    i.e., the i-th value is T[|T|-i], where T is the original input text.
  */
public:
    using CHAR = typename CHARVEC::value_type;
    using INDEX  = typename VEC::value_type;

  class iterator
  {
  private:
    const CHARVEC &_char_vec;
    ISA_IT _iterator;

  public:
    iterator() = default;
    explicit iterator(const CHARVEC &__char_vec, ISA_IT &__iter) : _char_vec(__char_vec), _iterator(__iter)
    {
    }
    iterator &operator++()
    {
      ++_iterator;
      return *this;
    }
    CHAR operator*() const
    {
      return this->_char_vec[this->_iterator.get_rle_lposition()];
    }
    bool operator!=(const iterator &rhs) const
    {
      return _iterator != rhs._iterator;
    }
    bool is_end() const
    {
      return _iterator.is_end();
    }
    /*
  static string decompress(RLBWT<char> &rlbwt, iterator &&begIt, iterator &&endIt, bool removeEndCharacter)
  {

    uint64_t size = rlbwt.str_size();
    if (removeEndCharacter)
      size--;
    auto p = begIt;
    if (removeEndCharacter)
      ++p;

    string s;
    s.resize(size);
    int64_t x = size - 1;
    std::cout << "Decompressing text" << std::flush;
    uint64_t counter = 0;
    while (p != endIt)
    {
      if (counter != 0)
      {
        --counter;
      }
      else
      {
        std::cout << "." << std::flush;
        counter = 10000000;
      }
      s[x--] = *p;
      ++p;
    }
    std::cout << std::endl;
    return s;
  }
  */
  };

private:
  //const RLBWT<CHAR, INDEX> &_rlbwt;
  const CHARVEC *_char_vec;
  const BackwardISA<POWVEC, VEC> *_isa;
  bool deleteFlag = false;

public:
  BackwardText()
  {
  }
  BackwardText(CHARVEC *&__char_vec, const BackwardISA<POWVEC, VEC> *__isa) : _char_vec(__char_vec), _isa(__isa), deleteFlag(false)
  {
  }

  BackwardText(CHARVEC *&__char_vec, BackwardISA<POWVEC, VEC> &&__isa) : _char_vec(__char_vec), _isa(new BackwardISA<VEC>(std::move(__isa))), deleteFlag(true)
  {
  }

  BackwardText(const BackwardText &obj)
  {
    if (obj._isa != nullptr)
    {
      throw std::logic_error("BackwardText instances cannot call the copy constructor.");
    }
  }
  BackwardText(BackwardText &&obj)
  {
    this->_isa = obj._isa;
    this->_char_vec = obj._char_vec;
    this->deleteFlag = obj.deleteFlag;
    obj.deleteFlag = false;
  }
  void set(const CHARVEC *__char_vec, BackwardISA<POWVEC, VEC> &&__isa)
  {
    this->_char_vec = __char_vec;
    this->_isa = new BackwardISA<POWVEC, VEC>(std::move(__isa));
    deleteFlag = true;
  }

  ~BackwardText()
  {
    if (deleteFlag)
      delete _isa;
  }
  iterator begin() const
  {
    auto it = this->_isa->begin();
    return iterator(*this->_char_vec, it);
  }
  iterator end() const
  {
    auto end = this->_isa->end();
    return iterator(*this->_char_vec, end);
  }

  std::vector<CHAR> to_vector() const
  {
    std::vector<CHAR> r;
    uint64_t size = _isa->str_size();
    r.resize(size);
    uint64_t p = size;
    for (CHAR c : *this)
    {
      r[--p] = c;
    }
    assert(p == 0);
    return r;
  }

  std::string to_string() const
  {
    std::string r;
    uint64_t size = _isa->str_size();
    std::cout << "size: " << size << std::endl;
    r.resize(size);
    uint64_t p = size;
    for (CHAR c : *this)
    {
      r[--p] = c;
    }
    assert(p == 0);
    return r;
  }
  //template <typename RLBWT_STR>
  void construct_from_rlbwt(const RLBWT<CHARVEC, POWVEC> *_rlbwt, bool faster = false)
  {
    BackwardISA<POWVEC> isa;
    isa.construct_from_rlbwt(_rlbwt, faster);
    this->set(_rlbwt->get_char_vec(), std::move(isa));
    //return BackwardText<CHAR, INDEX, BackwardISA<CHAR, INDEX, vector<INDEX>, RLBWT_STR>>(std::move(isa) );
  }
};

} // namespace rlbwt
} // namespace stool