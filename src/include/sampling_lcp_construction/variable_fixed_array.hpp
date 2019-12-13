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
class BinaryOperations
{
public:
    static uint64_t get_filter_bits(uint8_t zero_starting_pos, uint8_t zero_width)
    {
        if(zero_width != 0){
            uint64_t left = UINT64_MAX << (zero_starting_pos);
            uint64_t right = UINT64_MAX >> (64 - (zero_starting_pos + zero_width));

            return left ^ right;
        }else{
            return UINT64_MAX;
        }
    }
    static uint64_t write_zero_bits(uint64_t value, uint8_t i, uint8_t width)
    {
        uint64_t filter = get_filter_bits(i, width);
        return value & filter;
    }
    static uint64_t write_bits(uint64_t value, uint64_t c, uint8_t i, uint8_t w)
    {
        uint64_t filter = write_zero_bits(value, i, w);
        return filter | (c << i);
    }
    static uint64_t get_bits(uint64_t value, uint8_t i, uint8_t w)
    {
        uint64_t filter = ~get_filter_bits(i, w);
        return (value & filter) >> i;
    }

    static uint8_t getBitWidth(uint64_t x)
    {
        for (int8_t i = 63; i >= 0; i--)
        {
            if (((x >> i) & 0x00000001) > 0)
            {
                return (i + 1);
            }
        }
        return 0;
    }
    static uint64_t get_bits(const vector<uint64_t> &arr, uint64_t i, uint8_t width)
    {

        int16_t x = i / 64;
        int16_t y = i % 64;
        if (y + width <= 64)
        {
            return BinaryOperations::get_bits(arr[x], y, width);
        }
        else
        {
            uint8_t lower_bits = 64 - y;
            uint8_t upper_bits = width - lower_bits;
            uint64_t lower = BinaryOperations::get_bits(arr[x], y, lower_bits);
            uint64_t upper = BinaryOperations::get_bits(arr[x + 1], 0, upper_bits);
            return (upper << lower_bits) | lower;
        }
        //(array[x] & (1ULL << y)) > 0;
    }

    static void write_bits(vector<uint64_t> &arr, uint64_t value, uint64_t i, uint8_t width)
    {
        int16_t x = i / 64;
        int16_t y = i % 64;
        if (y + width <= 64)
        {
            arr[x] = BinaryOperations::write_bits(arr[x], value, y, width);
        }
        else
        {
            uint8_t lower_bits = 64 - y;
            uint8_t upper_bits = width - lower_bits;
            uint64_t lower_value = BinaryOperations::get_bits(value, 0, lower_bits);
            uint64_t upper_value = BinaryOperations::get_bits(value, lower_bits, upper_bits);

            arr[x] = BinaryOperations::write_bits(arr[x], lower_value, y, lower_bits);
            arr[x+1] =  BinaryOperations::write_bits(arr[x+1], upper_value, 0, upper_bits);
        }
    }
};

class NaiveVariableFixedArray
{
public:
    uint64_t byteSize;
    uint64_t num;
    vector<uint64_t> arr;
    vector<uint64_t> bitWidthVec;

    void build(std::vector<uint64_t> &items)
    {
        uint64_t sum_width = 0;
        bitWidthVec.push_back(0);

        for (auto it : items)
        {
            uint64_t width = BinaryOperations::getBitWidth(it);
            sum_width += width;
            bitWidthVec.push_back(sum_width);
        }
        arr.resize((sum_width / 64) + 1);
    }
    uint64_t get_position(uint64_t i) const
    {
        return this->bitWidthVec[i];
    }
    uint64_t get_width(uint64_t i) const
    {
        return this->bitWidthVec[i + 1] - this->bitWidthVec[i];
    }

    uint64_t access(uint64_t i) const
    {
        uint64_t pos = this->get_position(i);
        uint64_t width = this->get_width(i);
        return BinaryOperations::get_bits(arr, pos, width);
    }
    void change(uint64_t i, uint64_t value)
    {
        uint64_t pos = this->get_position(i);
        uint64_t width = this->get_width(i);
        BinaryOperations::write_bits(arr, value, pos, width);
    }
};

class VariableFixedArray
{
public:
    uint64_t num;
    uint64_t bits_sum;
    vector<uint64_t> arr;
    sdsl::bit_vector::select_1_type *width_selecter;

    void build(sdsl::bit_vector::select_1_type *_width_selecter, uint64_t item_count)
    {
        this->num = item_count;
        this->width_selecter = _width_selecter;
        this->bits_sum = (*width_selecter)(item_count+1) - item_count;
        arr.resize((bits_sum / 64) + 1, 0);
    }
    static sdsl::bit_vector create_width_bits(std::vector<uint64_t> &items){
        vector<uint8_t> bitWidthVec;
        uint64_t sum_width = 0;
        bitWidthVec.push_back(0);

        for (auto it : items)
        {
            uint64_t width = BinaryOperations::getBitWidth(it);
            sum_width += width;
            bitWidthVec.push_back(sum_width);
        }
        uint64_t bit_size = sum_width + items.size()+1;
        sdsl::bit_vector bv(bit_size, 0);
        uint64_t x = 0;
        bv[x++] = true;
        for(auto it : items){
            uint64_t width = BinaryOperations::getBitWidth(it);
            for(uint64_t i=0;i<width;i++){
                bv[x++] = false;
            }
            bv[x++] = true;
        }
        return bv;

    }

    uint64_t get_position(uint64_t i) const
    {
        return (*width_selecter)(i+1) - i;
    }
    uint64_t get_width(uint64_t i) const
    {
        return (*width_selecter)(i+2) - (*width_selecter)(i+1) - 1;
    }


    uint64_t operator[](uint64_t i) const
    {
        uint64_t pos = this->get_position(i);
        uint64_t width = this->get_width(i);
        if(width > 64) width = 64;
        return BinaryOperations::get_bits(arr, pos, width);
    }
    void change(uint64_t i, uint64_t value)
    {
        uint64_t pos = this->get_position(i);
        uint64_t width = this->get_width(i);
        if(width > 64) width = 64;
        BinaryOperations::write_bits(arr, value, pos, width);
    }
    uint64_t size() const {
        return this->num;
    }
    uint64_t get_using_memory() const {
        return (this->arr.size() * sizeof(uint64_t) ) + 16;
    }

};
} // namespace stool