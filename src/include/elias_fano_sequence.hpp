#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sdsl/bit_vectors.hpp>

namespace stool
{
/*
class FitValueArray
{

    uint64_t unitByteSize;
    uint64_t num;
    std::vector<uint64_t> arr;

  public:
    uint64_t size(){
        return this->num;
    }
    FitValueArray(){

    }
    uint64_t operator[](uint64_t i)
    {
        uin64_t pos = (i * unitByteSize);
        uint64_t large_index = pos / 64;
        uint64_t small_index = pos - (64 * large_index);
        if(small_index + unitByteSize < 64){
            arr[large_index]
        }else{

        }
    }
    void add(uint64_t value){
        std::pair<uint64_t, uint8_t> p = this->get_upper_and_lower_bits(this->num);
        uint64_t& large_index = p.first;
        uint8_t& small_index = p.second;

        if(small_index + unitByteSize < 64){
            arr[large_index] |= (small_index << small_index);
        }else{

        }

    }

    std::pair<uint64_t,uint64_t> get_upper_and_lower_bits(uint64_t i){
        uin64_t pos = (i * unitByteSize);
        uint64_t large_index = pos / 64;
        uint8_t small_index = pos - (64 * large_index);
        return std::pair<uint64_t, uint8_t>(large_index, small_index);
    }
};
*/


class EliasFanoSequence
{

private:
    uint64_t _size = 0;
    std::vector<uint64_t> lower_vec;
    sdsl::bit_vector upper_vec;
    uint8_t upper_bit_size;
    uint8_t lower_bit_size;

public:
    using value_type = uint64_t;

    EliasFanoSequence(){

    }
    template<typename VEC = std::vector<uint64_t>>
    void construct(const VEC &seq){
        this->upper_bit_size = std::log2(seq.size())+1;
        this->lower_bit_size = 64 - upper_bit_size;

        for(auto it : seq){
            std::pair<uint64_t,uint64_t> upper_and_lower_bits = get_upper_and_lower_bits(it);

        }
    }

    std::pair<uint64_t,uint64_t> get_upper_and_lower_bits(uint64_t value){
        uint64_t upper = value >> this->lower_bit_size;
        uint64_t lower = (value << this->upper_bit_size) >> this->upper_bit_size;
        return std::pair<uint64_t, uint64_t>(upper, lower);
    }

};

} // namespace stool