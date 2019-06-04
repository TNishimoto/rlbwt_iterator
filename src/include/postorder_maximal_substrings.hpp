#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "rlbwt.hpp"
#include "forward_sa.hpp"
#include "postorder_suffix_tree.hpp"



namespace stool
{
namespace rlbwt
{
template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX> >
class PostorderMSIterator
{
    PostorderSTIterator<INDEX,VEC> _begin;
    PostorderSTIterator<INDEX,VEC> _end;
    LCPInterval<INDEX> _current_ms_interval;
    const VEC &_pows;

public:
    PostorderMSIterator() = default;
    PostorderMSIterator(PostorderSTIterator<INDEX,VEC> &__begin, PostorderSTIterator<INDEX,VEC> &__end, const VEC &__pows) : _begin(__begin), _end(__end), _pows(__pows)
    {
        if (_begin != _end)
        {
            this->succ();
        }
        else
        {
            this->_current_ms_interval = LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
        }
    }

    void succ()
    {
        //std::cout << "succ" << std::endl;
        while (_begin != _end)
        {
            this->_current_ms_interval = *_begin;
            if (this->_current_ms_interval.j - this->_current_ms_interval.i == 0)
            {
                INDEX str_size = _begin.get_text_size();
                if (str_size == this->_current_ms_interval.lcp)
                {
                    break;
                }
            }
            else
            {
                auto left = std::upper_bound(_pows.begin(), _pows.end(), this->_current_ms_interval.i);
                INDEX left_index = distance(_pows.begin(), left);
                auto right = std::upper_bound(_pows.begin(), _pows.end(), this->_current_ms_interval.j);
                INDEX right_index = distance(_pows.begin(), right);
                //stool::Printer::print(_pows);
                if (left_index != right_index)
                {
                    break;
                }
            }
            ++_begin;
        }

        if (_begin != _end)
        {
            ++_begin;
        }
        else
        {
            this->_current_ms_interval = LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
        }
    }
    PostorderMSIterator &operator++()
    {
        this->succ();
        return *this;
    }
    LCPInterval<INDEX> operator*()
    {
        return this->_current_ms_interval;
    }
    bool operator!=(const PostorderMSIterator &rhs)
    {
        return _current_ms_interval != rhs._current_ms_interval;
    }
    static std::vector<std::string> naive_compute_maximal_substrings(std::string &text)
    {
        std::unordered_map<std::string, INDEX> map;
        for (INDEX i = 0; i < text.size(); i++)
        {
            for (INDEX j = i; j < text.size(); j++)
            {
                std::string sub = text.substr(i, 1 + j - i);
                auto it = map.find(sub);
                if (it != map.end())
                {
                    map[sub]++;
                }
                else
                {
                    map[sub] = 1;
                }
            }
        }
        std::unordered_set<std::string> checker;
        for (auto p : map)
        {
            std::string sub = p.first;
            if (sub.size() > 1)
            {
                std::string pref = sub.substr(0, sub.size() - 1);
                if (map[pref] == p.second)
                {
                    checker.insert(pref);
                }
                std::string suf = sub.substr(1);
                if (map[suf] == p.second)
                {
                    checker.insert(suf);
                }
            }
        }
        std::vector<std::string> r;
        for (auto p : map)
        {
            std::string sub = p.first;
            auto it = checker.find(sub);
            if (it == checker.end())
            {
                r.push_back(sub);
            }
        }
        r.push_back("");
        std::sort(r.begin(), r.end());
        return r;
    }
    static std::vector<LCPInterval<INDEX>> naive_compute_maximal_substrings2(std::string &text)
    {
        std::vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
        std::vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
        std::vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);
        std::string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
        //std::vector<LCPInterval> lcps = LCPInterval::naive_create_lcp_intervals(text, sa,lcp);
        std::vector<LCPInterval<INDEX>> lcps = LCPInterval<INDEX>::createLCPIntervals(sa, lcp);
        std::cout << "computing ms" << std::endl;
        std::vector<LCPInterval<INDEX>> r;

        for (auto &it : lcps)
        {
            if (it.j - it.i == 0)
            {
                if (it.lcp == text.size())
                {
                    r.push_back(it);
                }
            }
            else
            {
                bool b = false;
                for (INDEX i = it.i + 1; i <= it.j; i++)
                {
                    if (bwt[i] != bwt[i - 1])
                    {
                        b = true;
                        break;
                    }
                }
                if (b)
                {
                    r.push_back(it);
                }
            }
        }
        //r.push_back(LCPInterval(0, text.size()-1, ))
        return r;
    }
};

template <typename INDEX = uint64_t, typename VEC = std::vector<INDEX> >
class PostorderMaximalSubstrings
{
    const PostorderSuffixTree<INDEX,VEC> *_st_generator;
    const VEC *_pows;

    bool deleteFlag = false;
    //PostorderMSIterator<INDEX, VEC> *_end = nullptr;

public:
    PostorderMaximalSubstrings() : deleteFlag(false){

    }
    PostorderMaximalSubstrings(PostorderSuffixTree<INDEX,VEC> &&__st_generator, VEC &&__pows) 
    : _st_generator(new PostorderSuffixTree<INDEX,VEC>(std::move(__st_generator)) ), _pows(new VEC(std::move(__pows))),deleteFlag(true){

    }

    /*
    PostorderMaximalSubstrings(RLBWT<CHAR> &__rlbwt) : _rlbwt(__rlbwt), deleteFlag(true)
    {
        _pows = __rlbwt.get_run_vec();
        this->_str_size = __rlbwt.str_size();
        this->_st_generator = new ST(__rlbwt);
    }
    */

    ~PostorderMaximalSubstrings()
    {
        if (deleteFlag){
            delete _st_generator;
            delete _pows;
        }
    }

    void set(PostorderSuffixTree<INDEX,VEC> &&__st, VEC && __pows)
      {
        this->_st_generator = new PostorderSuffixTree<INDEX, VEC>(std::move(__st));
        this->_pows = new VEC(std::move(__pows));
        deleteFlag = true;
    }


    PostorderMSIterator<INDEX, VEC> begin() const
    {
        PostorderSTIterator<INDEX,VEC> _beg = this->_st_generator->begin();
        PostorderSTIterator<INDEX,VEC> _end = this->_st_generator->end();

        auto it = PostorderMSIterator<INDEX, VEC>(_beg,_end, *this->_pows );
        return it;
    }
    PostorderMSIterator<INDEX, VEC> end() const
    {
        PostorderSTIterator<INDEX,VEC> _end = this->_st_generator->end();

        return PostorderMSIterator<INDEX, VEC>(_end,_end, *this->_pows);
        //return it;
    }
    std::vector<LCPInterval<INDEX>> to_ms_intervals() const
    {
        std::vector<LCPInterval<INDEX>> r;
        for (LCPInterval<INDEX> c : *this)
        {
            r.push_back(c);
        }
        return r;
    }

    //template <typename CHAR = char, typename RLBWT_STR = RLBWT<CHAR, INDEX>>

  template <typename RLBWT_STR>
    void construct_from_rlbwt(const RLBWT_STR *_rlbwt, bool faster = false)
    {
        std::vector<INDEX> _pows = *_rlbwt->get_run_vec();
        rlbwt::PostorderSuffixTree<> postorder_st;
        postorder_st.construct_from_rlbwt(_rlbwt, faster);
        this->set(std::move(postorder_st), std::move(_pows));
    }
};

} // namespace rlbwt
} // namespace stool