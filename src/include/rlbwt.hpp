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
#include "other_functions.hpp"
#include "OnlineRlbwt/online_rlbwt.hpp"
//using namespace std;

namespace stool
{
namespace rlbwt
{

template <typename CHAR = char, typename INDEX = uint64_t, typename CHARVEC = std::vector<CHAR>, typename POWVEC = std::vector<INDEX>>
class RLBWT
{
    


    private:
    bool deleteFlag = false;
    const CHARVEC *char_vec = nullptr;
    const POWVEC *run_vec = nullptr;

public:

    RLBWT() : deleteFlag(false)
    {
    }

    RLBWT(std::vector<CHAR> *&__char_vec, std::vector<INDEX> *&__run_vec) : char_vec(__char_vec), run_vec(__run_vec),deleteFlag(false)
    {
    }
    RLBWT(std::vector<CHAR> &&__char_vec, std::vector<INDEX> &&__run_vec) : char_vec(new CHARVEC(std::move(__char_vec))), run_vec(new POWVEC(std::move(__run_vec))), deleteFlag(true)
    {
    }

    ~RLBWT()
    {
        //std::cout << "call destructor RLBWT!" << std::endl;

        if (this->deleteFlag)
        {
            delete this->char_vec;
            delete this->run_vec;
        }
    }
    RLBWT(const RLBWT &obj)
    {
        throw std::logic_error("RLBWT instances cannot call the copy constructor.");
        std::cout << obj.str_size() << std::endl;
    }
    RLBWT(RLBWT &&obj)
    {
        this->char_vec = obj.char_vec;
        this->run_vec = obj.run_vec;
        this->deleteFlag = obj.deleteFlag;
        obj.deleteFlag = false;
        std::cout << "RLBWT MOVE!" << std::endl;
    }
    
    void set(const CHARVEC *__char_vec, const POWVEC *__run_vec)
    {
        this->char_vec = __char_vec;
        this->run_vec = __run_vec;
        deleteFlag = false;
    }
    
    void set(CHARVEC &&__char_vec, POWVEC &&__run_vec)
    {
            this->char_vec = new CHARVEC(std::move(__char_vec));
            this->run_vec = new POWVEC(std::move(__run_vec) );
            deleteFlag = true;
    }


    const POWVEC* get_run_vec() const
    {
        return this->run_vec;
    }
    const CHARVEC* get_char_vec() const
    {
        return this->char_vec;
    }
    CHAR get_char_by_run_index(INDEX _run_index) const
    {
        return (*this->char_vec)[_run_index];
    }

    void clear()
    {
        std::cout << "clear!" << std::endl;
        this->char_vec->resize(0);
        this->char_vec->shrink_to_fit();
        this->run_vec->resize(0);
        this->run_vec->shrink_to_fit();
    }
    std::vector<INDEX> construct_pows() const
    {
        std::vector<INDEX> r;
        for (INDEX i = 0; i < run_vec->size(); i++)
        {
            r.push_back((*run_vec)[i + 1] - (*run_vec)[i]);
        }
        return r;
    }
    void write(std::ofstream &out) const
    {
        out.write((const char *)(&(*this->char_vec)[0]), sizeof(CHAR) * this->char_vec->size());
        std::vector<INDEX> pows = this->construct_pows();
        out.write((const char *)(&pows[0]), sizeof(INDEX) * pows.size());
    }
    void write(std::string filename) const
    {
        std::ofstream out(filename, std::ios::out | std::ios::binary);
        this->write(out);
        out.close();
    }
    INDEX get_run(INDEX i) const
    {
        return (*run_vec)[i + 1] - (*run_vec)[i];
    }
    INDEX get_lpos(INDEX i) const
    {
        return (*run_vec)[i];
    }
    INDEX rle_size() const
    {
        return (*char_vec).size();
    }

    bool check_rlbwt() const
    {
        INDEX endCounter = 0;
        for (INDEX i = 0; i < this->char_vec->size(); i++)
        {
            if ((*char_vec)[i] == 0)
            {
                endCounter++;
            }
        }
        if (endCounter != 1)
        {
            throw std::domain_error("the text must contain one end character!");
        }
        return true;
    }
    static bool check_text_for_rlbwt(std::string &text)
    {

        for (INDEX i = 0; i < text.size() - 1; i++)
        {
            if (text[i] == 0)
            {
                throw std::logic_error("The input text must not contain '0' except for the last character!");
                return false;
            }
        }
        if (text[text.size() - 1] != 0)
        {
            throw std::logic_error("The last character of the input text must be '0'");
            return false;
        }
        return true;
    }
    INDEX str_size() const
    {
        return (*run_vec)[(*run_vec).size() - 1];
    }
    void print_info() const
    {
        std::string msg = "";
        for (INDEX i = 0; i < this->rle_size(); i++)
        {
            if ((*char_vec)[i] == 0)
            {
                msg += "#";
            }
            else
            {
                msg.push_back((char)(*this->char_vec)[i]);
            }

            msg += "^";
            msg += std::to_string(this->get_run(i));
            if(i + 1 < this->rle_size()) msg += ", ";
        }
        std::cout << "RLBWT: " << msg << std::endl; 
        std::cout << "This instance has two vectors: char_vec and run_vec." << std::endl;
        std::cout << "The first array stores " << std::endl;
        for(auto c : *this->char_vec){
            std::cout << c << " ";
        }
        std::cout << std::endl;
        //stool::Printer::print(*this->char_vec);        
        std::cout << "The second array stores " << std::endl;
        for(auto c : *this->run_vec){
            std::cout << c << " ";
        }
        std::cout << std::endl;

        //stool::Printer::print(*this->run_vec);        


    }

    INDEX get_end_rle_lposition() const
    {
        for (INDEX i = 0; i < char_vec->size(); i++)
        {
            if ((*char_vec)[i] == 0)
            {
                return i;
            }
        }
        return std::numeric_limits<INDEX>::max();
    }

    INDEX get_lindex_containing_the_position(INDEX lposition) const
    {
        auto p = std::upper_bound(this->run_vec->begin(), this->run_vec->end(), lposition);
        INDEX pos = distance(this->run_vec->begin(), p) - 1;
        return pos;
    }
    std::pair<INDEX, INDEX> to_rle_lindex(INDEX lposition) const
    {
        INDEX lindex = this->get_lindex_containing_the_position(lposition);
        INDEX diff = lposition - this->get_lpos(lindex);
        return std::pair<INDEX, INDEX>(lindex, diff);
    }
    /*
    stool::rlbwt::ForwardBWT<CHAR, INDEX, CHARVEC,POWVEC> get_bwt(){
        return stool::rlbwt::ForwardBWT<CHAR, INDEX, CHARVEC,POWVEC>(this->char_vec, this->run_vec);
    }
    */
};

class RLBWTFunctions
{
public:
    template <typename INDEX = uint64_t, typename RLBWT_STR>
    static INDEX lf(const RLBWT_STR &rlbwt, INDEX pos, std::vector<INDEX> &fpos_vec)
    {
        INDEX lindex = rlbwt.get_lindex_containing_the_position(pos);
        INDEX diff = pos - rlbwt.get_lpos(lindex);
        INDEX succ_sa_index = fpos_vec[lindex] + diff;
        return succ_sa_index;
    }
    template <typename INDEX = uint64_t, typename RLBWT_STR>
    static std::vector<INDEX> construct_fpos_array(const RLBWT_STR &rlbwt)
    {
        std::vector<INDEX> fvec = construct_rle_fl_mapper<INDEX>(rlbwt);
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
    template <typename INDEX = uint64_t, typename RLBWT_STR>
    static std::vector<INDEX> construct_rle_fl_mapper(const RLBWT_STR &rlbwt)
    {
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
                     return (INDEX)rlbwt.get_char_by_run_index(x) < (INDEX)rlbwt.get_char_by_run_index(y);
                     //return this->char_vec[x] < this->char_vec[y];
                 }
             });
        return indexes;
    }

    template <typename INDEX = uint64_t, typename RLBWT_STR>
    static std::vector<INDEX> construct_rle_lf_mapper(const RLBWT_STR &rlbwt)
    {
        std::vector<INDEX> indexes = construct_rle_fl_mapper<INDEX>(rlbwt);
        return stool::rlbwt::change_inv(std::move(indexes));
    }
};
class RLBWTArrayFunctions{
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
        INDEX pos = distance(run_vec.begin(), p) - 1;
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
    static void construct_from_bwt(RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>> &_rlbwt, std::string &text)
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
    template <typename CHAR = char, typename INDEX = uint64_t>
    static void construct_from_file(RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>> &rlbwt, std::string filepath)
    {
        std::vector<CHAR> cVec;
        std::vector<INDEX> nVec;
        itmmti::online_rlbwt_from_file(filepath, cVec, nVec, 1);
        rlbwt.set(std::move(cVec), std::move(nVec) );
    }
    template <typename CHAR = char, typename INDEX = uint64_t>
    static void construct_from_string(RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>> &rlbwt, std::string &text)
    {
        std::vector<CHAR> cVec;
        std::vector<INDEX> nVec;
        itmmti::online_rlbwt(text, cVec, nVec, 1);
        rlbwt.set(std::move(cVec), std::move(nVec) );
        /*
        string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
        Constructor::construct_from_bwt<CHAR, INDEX>(rlbwt, bwt);
        */
    }
    template <typename CHAR = char, typename INDEX = uint64_t>
    static void construct_from_string(RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>> &rlbwt, std::string &&text)
    {

        std::vector<CHAR> cVec;
        std::vector<INDEX> nVec;
        itmmti::online_rlbwt(text, cVec, nVec, 1);
        rlbwt.set(std::move(cVec), std::move(nVec) );

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
    static RLBWT<CHAR, INDEX, std::vector<CHAR>, std::vector<INDEX>> load_RLBWT_from_file(std::string filename)
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

        RLBWT<CHAR, INDEX> rlestr;
        rlestr.set(std::move(char_vec), std::move(run_vec));
        rlestr.check_rlbwt();
        return rlestr;
    }
};


} // namespace rlbwt
} // namespace stool