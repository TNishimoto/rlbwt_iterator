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
#include "sd_vector.hpp"
#include "elias_fano_vector.hpp"
//using namespace std;

namespace stool
{
namespace rlbwt
{

//template <typename CHAR = char, typename INDEX = uint64_t, typename CHARVEC = std::vector<CHAR>, typename POWVEC = std::vector<INDEX>>
template <typename CHARVEC = std::vector<char>, typename POWVEC = std::vector<uint64_t>, typename RLE_SIZE_TYPE = uint64_t>
class RLBWT
{

private:
    bool deleteFlag = false;
    const CHARVEC *char_vec = nullptr;
    const POWVEC *run_vec = nullptr;

public:
    typedef CHARVEC char_vec_type;
    typedef POWVEC run_vec_type;
    using index_type = typename POWVEC::value_type;
    using char_type = typename CHARVEC::value_type;
    using CHAR = char_type;
    using INDEX = index_type;

    RLBWT() : deleteFlag(false)
    {
    }

    RLBWT(std::vector<CHAR> *&__char_vec, std::vector<INDEX> *&__run_vec) : char_vec(__char_vec), run_vec(__run_vec), deleteFlag(false)
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
        this->run_vec = new POWVEC(std::move(__run_vec));
        deleteFlag = true;
    }

    const POWVEC *get_run_vec() const
    {
        return this->run_vec;
    }
    POWVEC *get_run_vec_no_const() const
    {
        return this->run_vec;
    }

    const CHARVEC *get_char_vec() const
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
        return (*run_vec)[(i + 1)] - (*run_vec)[i];
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
            if (i + 1 < this->rle_size())
                msg += ", ";
        }
        std::cout << "RLBWT: " << msg << std::endl;
        std::cout << "This instance has two vectors: char_vec and run_vec." << std::endl;
        std::cout << "The first array stores " << std::endl;
        for (auto c : *this->char_vec)
        {
            std::cout << c << " ";
        }
        std::cout << std::endl;
        //stool::Printer::print(*this->char_vec);
        std::cout << "The second array stores " << std::endl;
        for (auto c : *this->run_vec)
        {
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
        INDEX pos = std::distance(this->run_vec->begin(), p) - 1;
        return pos;
    }
    std::pair<INDEX, INDEX> to_rle_lindex(INDEX lposition) const
    {
        INDEX lindex = this->get_lindex_containing_the_position(lposition);
        INDEX diff = lposition - this->get_lpos(lindex);
        return std::pair<INDEX, INDEX>(lindex, diff);
    }
    /*
    uint64_t get_using_memory() const {
        uint64_t m = 0;
        if(is_same<CHARVEC, std::vector<CHAR>>::value ){
            m += this->rle_size() * sizeof(CHAR);
        }else{
            m += this->char_vec->get_using_memory();
        }
        if(is_same<POWVEC, std::vector<INDEX>>::value){
            m += this->rle_size() * sizeof(INDEX);

        }else{
            m += this->run_vec->get_using_memory();            
        }
        return m;

    }
    */
    /*
    stool::rlbwt::ForwardBWT<CHAR, INDEX, CHARVEC,POWVEC> get_bwt(){
        return stool::rlbwt::ForwardBWT<CHAR, INDEX, CHARVEC,POWVEC>(this->char_vec, this->run_vec);
    }
    */
};

} // namespace rlbwt
} // namespace stool