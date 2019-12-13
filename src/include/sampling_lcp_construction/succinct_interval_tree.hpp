#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "../elias_fano_vector.hpp"

namespace stool
{

namespace rlbwt
{

class NaiveIntervalTree
{
public:
    using ITEM = std::pair<uint64_t, uint64_t>;
    std::vector<ITEM> items;

    void construct(std::vector<ITEM> &_items)
    {
        for (auto it : _items)
        {
            this->add(it.first, it.second);
        }
    }
    void add(uint64_t i, uint64_t j)
    {
        this->items.push_back(ITEM(i, j));
    }
    std::vector<ITEM> report_and_remove(uint64_t x)
    {
        std::vector<ITEM> r;
        std::vector<ITEM> r2;

        for (auto &p : this->items)
        {
            if (p.first <= x && x <= p.second)
            {
                r.push_back(p);
            }
            else
            {
                r2.push_back(p);
            }
        }
        this->items.swap(r2);
        return r;
    }
};

template <typename INTERVAL_SUM, typename INTERVAL_SINGLE>
struct SuccinctIntervalTreePointer
{
    INTERVAL_SUM line_rank;
    //uint64_t mid_point;
    INTERVAL_SINGLE height;

    string to_string() const
    {
        string s;
        s += "[";
        s += std::to_string(this->line_rank);
        s += ", ";
        // s += std::to_string(this->mid_point);
        // s += ", ";
        s += std::to_string(this->height);
        s += "]";
        return s;
    }
};

template <typename INTERVAL_SUM, typename INTERVAL_SINGLE, typename LEFT_ARRAY, typename RIGHT_ARRAY>
class SuccinctIntervalTree
{
public:
    using POINTER = SuccinctIntervalTreePointer<INTERVAL_SUM, INTERVAL_SINGLE>;
    class iterator
    {
        //std::stack<SuccinctIntervalTreePointer> st;
        INTERVAL_SINGLE height;
        INTERVAL_SUM line_rank;
        const SuccinctIntervalTree *tree;

    public:
        iterator() = default;

        iterator(const SuccinctIntervalTree *_tree, bool is_start) : tree(_tree)
        {
            if (is_start)
            {
                POINTER root = tree->get_root();
                this->height = root.height;
                this->line_rank = root.line_rank;
            }
            else
            {
                this->height = 0;
                this->line_rank = 0;
            }
        }

        bool isEnd()
        {
            return this->height == 0 && this->line_rank == 0;
        }
        POINTER operator*() const
        {
            POINTER p;
            p.height = this->height;
            p.line_rank = this->line_rank;
            return p;
        }
        iterator &operator++()
        {
            this->line_rank++;
            if (this->line_rank == tree->get_line_size(this->height))
            {
                this->line_rank = 0;
                this->height--;
            }
            return *this;
            /*
            if (st.size() > 0)
            {
                POINTER top = st.top();
                st.pop();
                if (top.height > 1)
                {
                    POINTER right = tree->get_right_child(top);
                    POINTER left = tree->get_left_child(top);
                    st.push(right);
                    st.push(left);
                }
            }
            else
            {
                throw std::logic_error("error");
            }
            */
        }
        uint64_t rank() const
        {
            return tree->get_rank(this->line_rank, this->height);
        }
        bool operator!=(const iterator &rhs) const
        {
            return (this->rank() != rhs.rank());
        }
    };

    using ITEM = std::pair<INTERVAL_SUM, INTERVAL_SUM>;
    LEFT_ARRAY *left_array;
    RIGHT_ARRAY *right_array;

    //using INTERVAL_SIZE_TYPE = uint8_t;
    //std::vector<ITEM> *items;

    uint64_t depth;
    uint64_t item_count = 0;
    std::vector<uint64_t> tree_size_vec;
    std::vector<uint64_t> leave_size_vec;
    std::vector<uint64_t> depth_first_node_rank_vec;

    std::vector<INTERVAL_SUM> left_ordered_intervals_vec;
    std::vector<INTERVAL_SINGLE> right_ordered_intervals_vec;
    std::vector<INTERVAL_SINGLE> current_left_offset_vec;
    std::vector<INTERVAL_SINGLE> current_right_offset_vec;

    stool::EliasFanoVector intervals_size_sequence;
    std::vector<bool> reported_checker;

    uint64_t get_tree_size(uint64_t height) const
    {
        return this->tree_size_vec[height];
    }
    uint64_t get_tree_size() const
    {
        return this->tree_size_vec[this->depth];
    }
    uint64_t get_leave_size(uint64_t height) const
    {
        return this->leave_size_vec[height];
    }
    uint64_t get_bottom_rank() const
    {
        return this->tree_size_vec[this->depth];
    }
    uint64_t get_rank(uint64_t line_rank, uint64_t height) const
    {
        return depth_first_node_rank_vec[height] + line_rank;
    }
    uint64_t get_item_count(uint64_t rank) const
    {
        return intervals_size_sequence[rank + 1] - intervals_size_sequence[rank];
    }
    std::pair<uint64_t, uint64_t> get_item(uint64_t item_number) const
    {
        return std::pair<INTERVAL_SUM, INTERVAL_SUM>((*left_array)[item_number], (*right_array)[item_number]);
        //(*this->items)[item_number];
    }
    uint64_t get_original_size() const
    {
        return this->item_count;
    }

    uint64_t get_line_size(uint64_t height) const
    {
        return depth_first_node_rank_vec[height - 1] - depth_first_node_rank_vec[height];
    }
    uint64_t get_interval_starting_position(uint64_t rank) const
    {
        return intervals_size_sequence[rank];
        /*
        uint64_t x = 0;
        for (uint64_t i = 0; i < rank; i++)
        {
            x += intervals_size_vec[i];
        }
        return x;
        */
    }

    uint64_t get_mid_point(uint64_t line_rank, uint64_t height) const
    {
        uint64_t x = leave_size_vec[height] * line_rank;
        if (height == 1)
        {
            return x;
        }
        else
        {
            uint64_t y = leave_size_vec[height - 1];
            return x + y;
        }
    }

    POINTER get_left_child(POINTER &top) const
    {
        if (top.height != 0)
        {
            POINTER left;
            left.line_rank = (top.line_rank) * 2;
            left.height = top.height - 1;
            /*
            if(left.height == 1){
                left.mid_point = top.mid_point - 1;
            }else{
                uint64_t left_leave_num = this->get_leave_size(left.height);
                left.mid_point = top.mid_point - (left_leave_num/2);
            }
            */
            return left;
        }
        else
        {
            throw std::logic_error("POINTER.height is 0");
        }
    }
    POINTER get_right_child(POINTER &top) const
    {
        if (top.height != 0)
        {
            POINTER right;
            right.line_rank = ((top.line_rank) * 2) + 1;
            right.height = top.height - 1;
            /*
            if(right.height == 1){
                right.mid_point = top.mid_point;
            }else{
                uint64_t right_leave_num = this->get_leave_size(right.height);
                right.mid_point = top.mid_point + (right_leave_num/2);
            }
            */
            return right;
        }
        else
        {
            throw std::logic_error("POINTER.height is 0");
        }
    }

    //uint64_t get_left_tree_size(uint64_t height){
    //    return this->tree_size_vec[height-1];
    //}
    POINTER get_root() const
    {
        POINTER sitp;
        sitp.line_rank = 0;
        sitp.height = this->depth;
        return sitp;
    }
    void initialize(uint64_t max_right)
    {

        this->tree_size_vec.push_back(0);
        this->leave_size_vec.push_back(0);
        uint64_t t = 1;
        uint64_t l = 1;
        uint64_t d = 1;
        this->tree_size_vec.push_back(t);
        this->leave_size_vec.push_back(l);

        while (l <= max_right)
        {
            t = (2 * t) + 1;
            l *= 2;
            d++;
            this->tree_size_vec.push_back(t);
            this->leave_size_vec.push_back(l);
        }
        this->depth = d;

        std::vector<uint64_t> depth_node_num_vec;
        depth_node_num_vec.resize(depth + 1, 0);
        depth_first_node_rank_vec.resize(depth + 1, 0);
        uint64_t x = 1;
        uint64_t r = 0;

        for (int64_t y = depth; y >= 0; --y)
        {
            if (y != 0)
                depth_node_num_vec[y] = x;
            depth_first_node_rank_vec[y] = r;
            x *= 2;
            r += depth_node_num_vec[y];
        }
    }
    iterator begin() const
    {
        return iterator(this, true);
    }
    iterator end() const
    {
        return iterator(this, false);
    }

    void construct(LEFT_ARRAY *_left_array, RIGHT_ARRAY *_right_array, std::vector<bool> &item_flag_vec)
    {
        this->left_array = _left_array;
        this->right_array = _right_array;

        uint64_t prev_i = UINT64_MAX;
        for (uint64_t i = 0; i < item_flag_vec.size(); i++)
        {
            if (item_flag_vec[i])
            {
                if (prev_i != UINT64_MAX)
                {
                    if ((*this->left_array)[prev_i] > (*this->left_array)[i])
                    {
                        throw std::logic_error("construct error!");
                    }
                }
                else
                {
                    prev_i = i;
                }
            }
        }

        uint64_t pp = 0;
        for (uint64_t i = 0; i < item_flag_vec.size(); i++)
        {
            if (item_flag_vec[i])
            {
                pp++;
                //_items[pp++] = i;
            }
        }
        this->item_count = pp;

        uint64_t n = this->get_original_size();
        reported_checker.resize(item_flag_vec.size(), false);
        for (uint64_t i = 0; i < item_flag_vec.size(); i++)
        {
            reported_checker[i] = !item_flag_vec[i];
        }

        current_left_offset_vec.resize(this->get_tree_size(), 0);
        current_right_offset_vec.resize(this->get_tree_size(), 0);
        std::vector<bool> checker;
        checker.resize(n, false);
        /*
        for (auto it : items)
        {
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;
        }
        */
        //std::vector<INTERVAL_SUM> intervals_size_vec;
        std::vector<bool> intervals_size_bit_vec;
        intervals_size_bit_vec.push_back(true);
        //intervals_size_vec.push_back(0);

        for (uint64_t h = this->depth; h > 0; --h)
        {
            uint64_t linesize = this->get_line_size(h);
            uint64_t w = 0;
            uint64_t w_index = 0;
            while (w_index < item_flag_vec.size() && !item_flag_vec[w_index])
            {
                w_index++;
            }

            //throw -1;
            for (uint64_t x = 0; x < linesize; x++)
            {
                uint64_t mid_point = this->get_mid_point(x, h);
                std::vector<uint64_t> tmp_intervals;
                while (w < n)
                {
                    ITEM current_item = this->get_item(w_index);
                    if (current_item.first > mid_point)
                    {
                        break;
                    }
                    if (!checker[w])
                    {
                        if (current_item.first <= mid_point && mid_point <= current_item.second)
                        {
                            tmp_intervals.push_back(w_index);
                            checker[w] = true;
                        }
                    }
                    w++;
                    w_index++;
                    while (w_index < item_flag_vec.size() && !item_flag_vec[w_index])
                    {
                        w_index++;
                    }
                }
                for (auto it : tmp_intervals)
                {
                    left_ordered_intervals_vec.push_back(it);
                }

                std::vector<INTERVAL_SINGLE> tmp_right_interval_ids;
                tmp_right_interval_ids.resize(tmp_intervals.size(), 0);
                for (uint64_t i = 0; i < tmp_right_interval_ids.size(); i++)
                {
                    tmp_right_interval_ids[i] = i;
                }

                std::sort(tmp_right_interval_ids.begin(), tmp_right_interval_ids.end(), [&](auto const &lhs, auto const &rhs) {
                    ITEM current_item1 = this->get_item(tmp_intervals[lhs]);
                    ITEM current_item2 = this->get_item(tmp_intervals[rhs]);
                    return current_item1.second > current_item2.second;
                });
                for (auto it : tmp_right_interval_ids)
                {
                    right_ordered_intervals_vec.push_back(it);
                }
                for (uint64_t tmp_i = 0; tmp_i < tmp_intervals.size(); tmp_i++)
                {
                    intervals_size_bit_vec.push_back(false);
                }
                intervals_size_bit_vec.push_back(true);

                //intervals_size_vec.push_back(intervals_size_vec[intervals_size_vec.size()-1] + tmp_intervals.size());
            }
        }
        this->intervals_size_sequence.build_from_bit_vector(intervals_size_bit_vec);
        //auto ppp = this->intervals_size_sequence.to_vector();
        //stool::Printer::print(ppp);

        for (uint64_t i = 0; i < checker.size(); i++)
        {
            if (!checker[i])
                throw std::logic_error("checker error!");
        }
        std::cout << std::endl;
    }
    std::vector<uint64_t> report_and_remove(uint64_t x)
    {
        std::vector<uint64_t> r;
        POINTER p = this->get_root();
        while (true)
        {
            bool b = report_and_remove(x, p, r);
            if (p.height == 1)
            {
                break;
            }
            if (b)
            {
                p = this->get_left_child(p);
            }
            else
            {
                p = this->get_right_child(p);
            }
        }
        return r;
    }
    bool report_and_remove(const uint64_t x, const POINTER &p, std::vector<uint64_t> &output)
    {

        uint64_t mid_point = this->get_mid_point(p.line_rank, p.height);
        uint64_t rank = this->get_rank(p.line_rank, p.height);
        int64_t start = this->get_interval_starting_position(rank);
        int64_t stop = (int64_t)(start + this->get_item_count(rank)) - 1;
        int64_t k;

        // std::cout << "search: mid = " << mid_point << "/" << x <<  p.to_string() << "start: = " << start << ", stop = " << stop << std::endl;
        if (x < mid_point)
        {
            uint64_t t = start + this->current_left_offset_vec[rank];
            //std::cout << "offset = " << (t -start) << std::endl;
            for (k = t; k <= stop; k++)
            {
                uint64_t item_number = this->left_ordered_intervals_vec[k];
                ITEM current_item = this->get_item(item_number);
                if (!reported_checker[item_number])
                {
                    //std::cout << "check[" << current_item.first << ", " << current_item.second << "]" << std::endl;
                    assert(current_item.first <= mid_point && mid_point <= current_item.second);
                    if (current_item.first <= x)
                    {
                        output.push_back(item_number);
                        //std::cout << "reportX[" << current_item.first << ", " << current_item.second << "]" << std::endl;
                        this->current_left_offset_vec[rank]++;
                        reported_checker[item_number] = true;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    this->current_left_offset_vec[rank]++;
                }
            }
        }
        else
        {
            uint64_t t = start + this->current_right_offset_vec[rank];
            //std::cout << "offset = " <<(t-start) << std::endl;

            for (k = t; k <= stop; k++)
            {
                uint64_t tmp_k = this->right_ordered_intervals_vec[k];
                uint64_t item_number = this->left_ordered_intervals_vec[start + tmp_k];
                ITEM current_item = this->get_item(item_number);
                if (!reported_checker[item_number])
                {
                    //std::cout << "check[" << current_item.first << ", " << current_item.second << "]" << std::endl;
                    assert(current_item.first <= mid_point && mid_point <= current_item.second);
                    if (x <= current_item.second)
                    {
                        output.push_back(item_number);
                        //std::cout << "reportY[" << current_item.first << ", " << current_item.second << "]" << std::endl;
                        reported_checker[item_number] = true;
                        this->current_right_offset_vec[rank]++;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    this->current_right_offset_vec[rank]++;
                }
            }
        }
        return x < mid_point;
    }
    uint64_t get_using_memory() const {
        uint64_t m = 8 + 8 + ((tree_size_vec.size() + leave_size_vec.size() + depth_first_node_rank_vec.size()) * sizeof(uint64_t)) ;

        std::cout << "basic: " << m << " bytes" << std::endl;

        uint64_t m1 = left_ordered_intervals_vec.size() * sizeof(INTERVAL_SUM);
        std::cout << "left vec: " << m1 << " bytes" << std::endl;

        uint64_t m2 = right_ordered_intervals_vec.size() * sizeof(INTERVAL_SINGLE);
        std::cout << "right vec: " << m2 << " bytes" << std::endl;

        uint64_t m3 = current_left_offset_vec.size() * sizeof(INTERVAL_SINGLE);
        std::cout << "left offset vec: " << m3 << " bytes" << std::endl;

        uint64_t m4 = current_right_offset_vec.size() * sizeof(INTERVAL_SINGLE);
        std::cout << "right offset vec: " << m4 << " bytes" << std::endl;

        uint64_t m5 = reported_checker.size() / 8;
        std::cout << "checker: " << m5 << " bytes" << std::endl;

        uint64_t m6 = intervals_size_sequence.get_using_memory();
        std::cout << "EF vec: " << m6 << " bytes" << std::endl;

        std::cout << "item num: " << left_ordered_intervals_vec.size() << std::endl;
        std::cout << "tree size: " << current_left_offset_vec.size() << std::endl;

        return m + m1 + m2 + m3 + m4 + m5 + m6;
    }
    std::string get_info(uint64_t line_rank, uint64_t height)
    {
        uint64_t rank = this->get_rank(line_rank, height);
        std::string s;
        s += "[";
        s += std::to_string(line_rank);
        s += ", ";
        s += std::to_string(height);
        s += ", rank = ";
        s += std::to_string(rank);
        s += ", mid = ";
        s += std::to_string(this->get_mid_point(line_rank, height));
        s += ", i_size = ";
        s += std::to_string(this->get_item_count(rank));
        s += "]";

        uint64_t pos = get_interval_starting_position(rank);
        uint64_t size = this->get_item_count(rank);
        for (uint64_t x = pos; x < pos + size; x++)
        {
            s += "[";
            s += std::to_string(this->get_item(left_ordered_intervals_vec[x]).first);
            s += "..";
            s += std::to_string(this->get_item(left_ordered_intervals_vec[x]).second);
            s += "]";
        }
        return s;
    }
};

class SuccinctIntervalTreeDebug
{
public:
    static std::vector<std::pair<uint64_t, uint64_t>> to_intervals(std::string &text)
    {
        std::vector<std::vector<uint64_t>> r;
        r.resize(UINT8_MAX);
        for (uint64_t x = 0; x < r.size(); x++)
        {
            r[x].resize(0);
        }

        for (uint64_t x = 0; x < text.size(); x++)
        {
            uint8_t p = text[x];
            r[p].push_back(x);
        }
        std::vector<std::pair<uint64_t, uint64_t>> intervals;
        for (uint64_t x = 0; x < r.size(); x++)
        {
            for (uint64_t y = 1; y < r[x].size(); y++)
            {
                intervals.push_back(std::pair<uint64_t, uint64_t>(r[x][y - 1] + 1, r[x][y]));
            }
        }
        return intervals;
    }
};

} // namespace rlbwt
} // namespace stool