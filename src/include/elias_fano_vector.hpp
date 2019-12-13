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
#include "stool/src/value_array.hpp"

namespace stool
{
/*
bool getBit(uint64_t x, int8_t nth)
{
    return ((x >> nth) & 0x00000001) > 0;
}
std::string toBinaryString(uint64_t x)
{
    std::string s;
    s.resize(64, '0');
    for (uint64_t i = 0; i < 64; i++)
    {
        s[i] = getBit(x, i) ? '1' : '0';
    }
    return s;
}
*/

class EliasFanoVector
{
public:
    class iterator
    {
    private:
        uint64_t index = 0;
        const EliasFanoVector *efs;

    public:
        using difference_type = uint64_t;
        using value_type = uint64_t;
        using pointer = uint64_t *;
        using reference = uint64_t &;
        using iterator_category = std::random_access_iterator_tag;
        //using value_type = uint64_t;

        iterator(const EliasFanoVector *_efs, uint64_t _index) : index(_index), efs(_efs)
        {
        }

        iterator &operator++()
        {
            this->index++;
            return *this;
        }
        uint64_t operator*()
        {
            return efs->access(this->index);
        }
        bool operator!=(const iterator &rhs)
        {
            return (index != rhs.index);
        }
        bool operator==(const iterator &rhs)
        {
            return (index == rhs.index);
        }

        iterator &operator+=(int64_t p)
        {
            this->index += p;
            return *this;
        }

        bool operator<(const iterator &rhs)
        {
            return (index < rhs.index);
        }
        bool operator>(const iterator &rhs)
        {
            return (index > rhs.index);
        }
        bool operator<=(const iterator &rhs)
        {
            return (index <= rhs.index);
        }
        bool operator>=(const iterator &rhs)
        {
            return (index >= rhs.index);
        }
        uint64_t operator-(const iterator &rhs)
        {
            if (this->index < rhs.index)
            {
                return rhs.index - this->index;
            }
            else
            {
                return this->index - rhs.index;
            }
        }
    };

private:
    uint64_t _size = 0;
    stool::ValueArray lower_bits;
    sdsl::bit_vector upper_bits;
    sdsl::bit_vector::select_1_type upper_selecter;
    uint8_t upper_bit_size;
    uint8_t lower_bit_size;
    uint64_t max_value = 0;

    template <typename VEC = std::vector<uint64_t>>
    void construct_lower_data_structure(const VEC &seq)
    {

        this->lower_bits.set(seq, true);
    }
    void build_lower_data_structure_from_bit_vector(const std::vector<bool> &seq, uint64_t item_count)
    {
        uint64_t zero_counter = 0;
        uint64_t max_value = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            if (seq[i])
            {
                uint64_t lower_value = get_upper_and_lower_bits(zero_counter).second;
                if (max_value < lower_value)
                    max_value = lower_value;
            }
            else
            {
                zero_counter++;
            }
        }
        if (max_value <= UINT8_MAX)
        {
            this->lower_bits.resize(item_count, 1);
        }
        else if (max_value <= UINT16_MAX)
        {
            this->lower_bits.resize(item_count, 2);
        }
        else if (max_value <= UINT32_MAX)
        {

            this->lower_bits.resize(item_count, 4);
        }
        else
        {

            this->lower_bits.resize(item_count, 8);
        }
        zero_counter = 0;
        uint64_t ith = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            if (seq[i])
            {
                uint64_t lower_value = get_upper_and_lower_bits(zero_counter).second;
                this->lower_bits.change(ith, lower_value);
                ith++;
            }
            else
            {
                zero_counter++;
            }
        }

    }

    template <typename VEC = std::vector<uint64_t>>
    void construct_upper_data_structure(const VEC &seq)
    {

        vector<bool> tmp2;
        uint64_t tmp2_pos = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            uint64_t upper_value = get_upper_and_lower_bits(seq[i]).first;
            if (tmp2_pos == upper_value)
            {
                tmp2.push_back(true);
            }
            else if (tmp2_pos < upper_value)
            {
                while (tmp2_pos < upper_value)
                {
                    tmp2.push_back(false);
                    ++tmp2_pos;
                }
                tmp2.push_back(true);
            }
            else
            {
                std::runtime_error("error");
            }
            //std::cout << upper_bits[i] << std::endl;
        }
        tmp2.push_back(false);

        sdsl::bit_vector b(tmp2.size(), 0);
        for (uint64_t i = 0; i < tmp2.size(); i++)
            b[i] = tmp2[i] ? 1 : 0;
        //sdsl::bit_vector::select_1_type b_sel(&b);
        upper_bits.swap(b);
        sdsl::bit_vector::select_1_type b_sel(&upper_bits);
        upper_selecter.set_vector(&upper_bits);
        upper_selecter.swap(b_sel);
    }
    void build_upper_data_structure_from_bit_vector(const std::vector<bool> &seq)
    {

        vector<bool> tmp2;
        uint64_t tmp2_pos = 0;
        uint64_t zero_counter = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            if (seq[i])
            {
                uint64_t upper_value = get_upper_and_lower_bits(zero_counter).first;
                if (tmp2_pos == upper_value)
                {
                    tmp2.push_back(true);
                }
                else if (tmp2_pos < upper_value)
                {
                    while (tmp2_pos < upper_value)
                    {
                        tmp2.push_back(false);
                        ++tmp2_pos;
                    }
                    tmp2.push_back(true);
                }
                else
                {
                    std::runtime_error("error");
                }
            }
            else
            {
                zero_counter++;
            }
            //std::cout << upper_bits[i] << std::endl;
        }
        tmp2.push_back(false);
        sdsl::bit_vector b(tmp2.size(), 0);
        for (uint64_t i = 0; i < tmp2.size(); i++)
            b[i] = tmp2[i] ? 1 : 0;
        //sdsl::bit_vector::select_1_type b_sel(&b);
        upper_bits.swap(b);
        sdsl::bit_vector::select_1_type b_sel(&upper_bits);
        upper_selecter.set_vector(&upper_bits);
        upper_selecter.swap(b_sel);

    }

    std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value)
    {
        uint64_t upper = value >> this->lower_bit_size;
        uint64_t upper_filter = 64 - this->lower_bit_size;
        uint64_t lower = (value << upper_filter) >> upper_filter;
        return std::pair<uint64_t, uint64_t>(upper, lower);
    }

    uint64_t recover(uint64_t lower, uint64_t upper)
    {
        return (upper << lower_bit_size) | lower;
    }

public:
    using value_type = uint64_t;
    using const_iterator = iterator;

    EliasFanoVector()
    {
    }

    EliasFanoVector(EliasFanoVector &&obj) : lower_bits(std::move(obj.lower_bits))
    {

        this->_size = obj._size;
        this->upper_bit_size = obj.upper_bit_size;
        this->lower_bit_size = obj.lower_bit_size;
        this->max_value = obj.max_value;
        this->upper_bits.swap(obj.upper_bits);
        this->upper_selecter.set_vector(&this->upper_bits);
        this->upper_selecter.swap(obj.upper_selecter);
    }
    template <typename VEC = std::vector<uint64_t>>
    void construct(VEC *seq)
    {
        for (auto it : *seq)
        {
            if (max_value < it)
                max_value = it;
        }
        _size = seq->size();

        this->upper_bit_size = std::log2(seq->size()) + 1;
        this->lower_bit_size = (std::log2(max_value) + 1) - upper_bit_size;

        this->construct_upper_data_structure(*seq);

        for (uint64_t i = 0; i < _size; i++)
        {
            (*seq)[i] = get_upper_and_lower_bits((*seq)[i]).second;
        }
        this->construct_lower_data_structure(*seq);
        for (uint64_t i = 0; i < _size; i++)
        {
            (*seq)[i] = this->access(i);
        }

        //this->check(*seq);
    }
    void build_from_bit_vector(const std::vector<bool> &seq)
    {
        uint64_t zero_counter = 0;
        uint64_t max_value = 0;
        uint64_t item_count = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            if (seq[i])
            {
                uint64_t value = zero_counter;
                item_count++;
                if (max_value < value)
                    max_value = value;
            }
            else
            {
                zero_counter++;
            }
        }
        _size = item_count;
        this->upper_bit_size = std::log2(item_count) + 1;
        this->lower_bit_size = (std::log2(max_value) + 1) - upper_bit_size;


        this->build_upper_data_structure_from_bit_vector(seq);
        this->build_lower_data_structure_from_bit_vector(seq, item_count);

    }

    uint64_t access(uint64_t i) const
    {
        uint64_t upper = (upper_selecter(i + 1) - i);
        uint64_t lower = lower_bits[i];
        return (upper << lower_bit_size) | lower;
    }

    template <typename VEC = std::vector<uint64_t>>
    void check(const VEC &seq)
    {
        for (uint64_t i = 0; i < seq.size(); i++)
        {
            uint64_t p = this->access(i);
            if (seq[i] != p)
            {
                throw std::logic_error("Something error!");
            }
        }
        std::cout << "OK!" << std::endl;
    }
    uint64_t operator[](uint64_t i) const
    {
        return this->access(i);
    }
    std::vector<uint64_t> to_vector()
    {
        std::vector<uint64_t> r;
        for (uint64_t i = 0; i < _size; i++)
        {
            r.push_back(this->access(i));
        }

        return r;
    }
    uint64_t size() const
    {
        return this->_size;
    }

    iterator begin() const
    {
        auto p = iterator(this, 0);
        return p;
    }
    iterator end() const
    {
        auto p = iterator(this, _size);
        return p;
    }

    uint64_t get_using_memory() const {
        return this->lower_bits.get_using_memory() + (this->upper_bits.size()/8) + 18;
    }
};

} // namespace stool