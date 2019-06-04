#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "rlbwt.hpp"
#include "forward_lcp_array.hpp"
#include "lcp_interval.hpp"

namespace stool
{
namespace rlbwt
{
template <typename INDEX = uint64_t>
class IncompleteLCPInterval
{
public:
  INDEX i;
  INDEX j;
  INDEX lcp;
  INDEX i_lcp;

  IncompleteLCPInterval();
  IncompleteLCPInterval(INDEX _i, INDEX _j, INDEX _lcp, INDEX _i_lcp) : i(_i), j(_j), lcp(_lcp), i_lcp(_i_lcp)
  {
  }
};

/*
    This iterator enumerates nodes in the suffix tree of the original input text in the postorder. 
    Each node stores its lcp interval and its depth.
  */
template <typename INDEX = uint64_t, typename VEC = std::vector<uint64_t>>
class PostorderSTIterator
{
  LCPIterator<INDEX, VEC> _lcp_forward_iterator;
  std::stack<IncompleteLCPInterval<INDEX>> incompleteStack;
  std::queue<IncompleteLCPInterval<INDEX>> outputQueue;
  LCPInterval<INDEX> _currenct_lcp_interval;

  //int64_t n = 0;
  bool compute_next_lcp_Interval()
  {

    INDEX n = _lcp_forward_iterator.get_text_size();
    INDEX counter_i = _lcp_forward_iterator.get_sa_index();

    while (counter_i < n)
    {
      counter_i = _lcp_forward_iterator.get_sa_index();
      INDEX sa_value = _lcp_forward_iterator.get_text_index();
      INDEX lcp_value = *_lcp_forward_iterator;

      IncompleteLCPInterval<INDEX> fst = IncompleteLCPInterval<INDEX>(counter_i, counter_i, n - sa_value, lcp_value);
      while (incompleteStack.size() > 1)
      {
        IncompleteLCPInterval<INDEX> second = incompleteStack.top();
        if (second.i_lcp > fst.i_lcp)
        {
          incompleteStack.pop();
          INDEX newLCP = second.i_lcp;
          INDEX newj = second.j;
          INDEX newi = second.i;
          INDEX newi_lcp = second.i_lcp;

          while (incompleteStack.size() > 0)
          {
            IncompleteLCPInterval<INDEX> third = incompleteStack.top();
            newi = third.i;
            newi_lcp = third.i_lcp;
            incompleteStack.pop();
            if (second.i_lcp != third.i_lcp)
            {
              break;
            }
          }
          IncompleteLCPInterval<INDEX> newInterval(newi, newj, newLCP, newi_lcp);
          this->outputQueue.push(newInterval);
          incompleteStack.push(newInterval);
        }
        else
        {
          break;
        }
      }
      this->outputQueue.push(fst);
      incompleteStack.push(fst);

      ++_lcp_forward_iterator;

      //counter_i++;

      return true;
    }

    while (incompleteStack.size() > 1)
    {
      IncompleteLCPInterval<INDEX> second = incompleteStack.top();
      incompleteStack.pop();
      INDEX newLCP = second.i_lcp;
      INDEX newj = second.j;
      INDEX newi = second.i;
      INDEX newi_lcp = second.i_lcp;
      while (incompleteStack.size() > 0)
      {
        IncompleteLCPInterval<INDEX> third = incompleteStack.top();
        newi = third.i;
        newi_lcp = third.i_lcp;
        incompleteStack.pop();
        if (second.i_lcp != third.i_lcp)
        {
          break;
        }
      }
      IncompleteLCPInterval<INDEX> newInterval(newi, newj, newLCP, newi_lcp);
      incompleteStack.push(newInterval);
      this->outputQueue.push(newInterval);
      return true;
    }
    return false;
  }
  bool succ()
  {
    while (true)
    {
      if (this->outputQueue.size() > 0)
      {
        IncompleteLCPInterval<INDEX> sli = this->outputQueue.front();
        _currenct_lcp_interval.i = sli.i;
        _currenct_lcp_interval.j = sli.j;
        _currenct_lcp_interval.lcp = sli.lcp;
        this->outputQueue.pop();
        return true;
      }
      else
      {
        bool b = this->compute_next_lcp_Interval();
        if (!b)
        {
          _currenct_lcp_interval.i = std::numeric_limits<INDEX>::max();
          _currenct_lcp_interval.j = std::numeric_limits<INDEX>::max();
          _currenct_lcp_interval.lcp = std::numeric_limits<INDEX>::max();
          return false;
        }
      }
    }
    return true;
  }

public:
  PostorderSTIterator() = default;
  PostorderSTIterator(LCPIterator<INDEX, VEC> &__lcp_forward_iterator, bool isBegin) : _lcp_forward_iterator(__lcp_forward_iterator)
  {
    if (isBegin)
    {
      this->succ();
    }
    else
    {
      this->_currenct_lcp_interval = LCPInterval<>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
    }
  }
  INDEX get_text_size()
  {
    return this->_lcp_forward_iterator.get_text_size();
  }

  //bool takeFront(LCPInterval &outputInterval);

  PostorderSTIterator &operator++()
  {
    this->succ();
    return *this;
  }
  LCPInterval<INDEX> operator*()
  {
    return this->_currenct_lcp_interval;
  }
  bool operator!=(const PostorderSTIterator &rhs)
  {
    return _currenct_lcp_interval != rhs._currenct_lcp_interval;
  }
};

template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX>>
class PostorderSuffixTree
{
  const ForwardLCPArray<INDEX, VEC> *_lcp_generator;
  bool deleteFlag = false;

public:
  PostorderSuffixTree(ForwardLCPArray<INDEX, VEC> &&__lcp_generator) : _lcp_generator(new ForwardLCPArray<INDEX, VEC>(std::move(__lcp_generator))), deleteFlag(true)
  {
  }
  PostorderSuffixTree() : deleteFlag(false)
  {
  }

  ~PostorderSuffixTree()
  {
    if (deleteFlag)
      delete _lcp_generator;
  }

  PostorderSuffixTree(PostorderSuffixTree &&obj)
  {
    this->_lcp_generator = obj._lcp_generator;
    this->deleteFlag = obj.deleteFlag;
    obj.deleteFlag = false;
  }
  PostorderSuffixTree(const PostorderSuffixTree &obj)
  {
    if (obj._lcp_generator != nullptr)
    {
      throw std::logic_error("PostorderSuffixTree instances cannot call the copy constructor.");
    }
  }

  void set(ForwardLCPArray<INDEX, VEC> &&__lcp_generator)
  {
    this->_lcp_generator = new ForwardLCPArray<INDEX, VEC>(std::move(__lcp_generator));
    deleteFlag = true;
  }

  PostorderSTIterator<INDEX, VEC> begin() const
  {
    LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
    auto it = PostorderSTIterator<INDEX, VEC>(lcp_it, true);
    return it;
  }
  PostorderSTIterator<INDEX, VEC> end() const
  {
    LCPIterator<INDEX, VEC> lcp_it = this->_lcp_generator->begin();
    auto it = PostorderSTIterator<INDEX, VEC>(lcp_it, false);
    return it;
  }
  std::vector<LCPInterval<INDEX>> to_lcp_intervals() const
  {
    std::vector<LCPInterval<INDEX>> r;
    for (LCPInterval<INDEX> c : *this)
    {
      r.push_back(c);
    }
    return r;
  }
  template <typename RLBWT_STR>
    void construct_from_rlbwt(const RLBWT_STR *_rlbwt, bool faster = false)
    {
        ForwardLCPArray<INDEX, std::vector<INDEX>> forward_lcp_array;
        forward_lcp_array.construct_from_rlbwt(_rlbwt, faster);
        this->set(std::move(forward_lcp_array));

        //return PostorderSuffixTree<INDEX, std::vector<INDEX>>(std::move(forward_lcp_array));
    }
};

} // namespace rlbwt
} // namespace stool