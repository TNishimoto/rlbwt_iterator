#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include <sdsl/bp_support.hpp>

using namespace std;
using namespace stool;
using namespace stool::rlbwt;
using namespace sdsl;

struct SuccinctIntervalTreePointer{
    uint64_t rank;
    uint64_t mid_point;
    uint8_t height;

    string to_string() const {
        string s;
        s += "[";
        s += std::to_string(this->rank);
        s += ", ";
        s += std::to_string(this->mid_point);
        s += ", ";
        s += std::to_string(this->height);
        s += "]";
        return s;
    }
};



template <typename INTERVAL_SUM, typename INTERVAL_SINGLE>
class SuccinctIntervalTree
{
public:
    class iterator
    {
        std::stack<SuccinctIntervalTreePointer> st;
        const SuccinctIntervalTree *tree;

        public:
        iterator() = default;
        
        iterator(const SuccinctIntervalTree *_tree, bool is_start) : tree(_tree) 
        {
            if(is_start){
                SuccinctIntervalTreePointer root = tree->get_root();
                this->st.push(root);
            }else{

            }
        }
        
        bool isEnd(){
            return this->st.size() == 0;
        }
        SuccinctIntervalTreePointer operator*() const
        {
            SuccinctIntervalTreePointer p = this->st.top();
            return p;
        }
        iterator &operator++()
        {
            if(st.size() > 0){
                SuccinctIntervalTreePointer top = st.top();
                st.pop();
                if(top.height > 1){
                    SuccinctIntervalTreePointer right = tree->get_right_child(top);
                    SuccinctIntervalTreePointer left = tree->get_left_child(top);
                    st.push(right);
                    st.push(left);
                }
                return *this;
            }else{
                throw std::logic_error("error");
            }
        }
        uint64_t rank() const {
            if(st.size() > 0){
                SuccinctIntervalTreePointer p = this->st.top();
                return p.rank;
            }else{
                return tree->get_bottom_rank();
            }
        }
        bool operator!=(const iterator &rhs) const
        {            
            return (this->rank() != rhs.rank());
        }
    };

    using ITEM = std::pair<uint64_t, uint64_t>;
    //using INTERVAL_SIZE_TYPE = uint8_t;
    std::vector<ITEM> items;

    uint64_t depth;
    std::vector<uint64_t> tree_size_vec;
    std::vector<uint64_t> leave_size_vec;
    std::vector<INTERVAL_SINGLE> interval_pointer_vec;
    std::vector<uint64_t> left_ordered_intervals_vec;
    std::vector<uint64_t> right_ordered_intervals_vec;

    uint64_t get_tree_size(uint64_t height) const {
        return this->tree_size_vec[height];
    }
    uint64_t get_leave_size(uint64_t height) const {
        return this->leave_size_vec[height];
    }
    uint64_t get_bottom_rank() const {
        return this->tree_size_vec[this->depth];
    }
    SuccinctIntervalTreePointer get_left_child(SuccinctIntervalTreePointer &top) const {
        if(top.height != 0){
            SuccinctIntervalTreePointer left;
            left.rank = top.rank+1;
            left.height = top.height-1;
            if(left.height == 1){
                left.mid_point = top.mid_point - 1;
            }else{
                uint64_t left_leave_num = this->get_leave_size(left.height);
                left.mid_point = top.mid_point - (left_leave_num/2);
            }
            return left;
        }else{
            throw std::logic_error("error");
        }
    }
    SuccinctIntervalTreePointer get_right_child(SuccinctIntervalTreePointer &top) const {
        if(top.height != 0){
            SuccinctIntervalTreePointer right;
            right.height = top.height-1;
            right.rank = top.rank+(this->get_tree_size(right.height)) + 1;
            if(right.height == 1){
                right.mid_point = top.mid_point;
            }else{
                uint64_t right_leave_num = this->get_leave_size(right.height);
                right.mid_point = top.mid_point + (right_leave_num/2);
            }
            return right;
        }else{
            throw std::logic_error("error");
        }
    }
    bool is_leaf(SuccinctIntervalTreePointer &top) const {
        return top.height == 1;
    }

    //uint64_t get_left_tree_size(uint64_t height){
    //    return this->tree_size_vec[height-1];
    //}
    SuccinctIntervalTreePointer get_root() const {
        SuccinctIntervalTreePointer sitp;
        sitp.rank = 0;
        sitp.height = this->depth;
        sitp.mid_point = leave_size_vec[sitp.height-1];
        return sitp;
    }
    void initialize(uint64_t max_right){

        this->tree_size_vec.push_back(0);
        this->leave_size_vec.push_back(0);
        uint64_t t=1;
        uint64_t l=1;
        uint64_t d=1;        
        this->tree_size_vec.push_back(t);
        this->leave_size_vec.push_back(l);

        while(l <= max_right){
            t = (2*t)+1;
            l*=2;
            d++;
            this->tree_size_vec.push_back(t);
            this->leave_size_vec.push_back(l);
        }
        std::cout << t << "/" << l << "/" << d << std::endl;
        this->depth = d;
    }
    iterator begin() const {
        return iterator(this, true);
    }
    iterator end() const {
        return iterator(this, false);
    }
    
    

    void construct(std::vector<ITEM> &_items)
    {
        this->items.swap(_items);
        std::stack<std::vector<uint64_t>> st;
            std::vector<uint64_t> top_items;
        for(uint64_t i=0;i<this->items.size();i++){
            top_items.push_back(i);
        }
        uint64_t rank = 0;
        //SuccinctIntervalTreeTmp tt;
        //tt.rank = 0;
        //tt.b = false;
        //tt.items.swap(top_items);
        st.push(top_items);

        while(st.size() > 0){
            auto top = st.top();
            st.pop();

        }
        

    }
    std::vector<ITEM> reportAndRemove(uint64_t x)
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

int main(int argc, char *argv[])
{
    SuccinctIntervalTree<uint64_t, uint8_t> tree;
    tree.initialize(20);
    auto root = tree.get_root();
    std::cout << root.to_string() << std::endl;

    for(auto it : tree){
        std::cout << (it).to_string() << std::endl;

    }
    /*
    auto beg = tree.begin();
    auto end = tree.end();

    for(uint64_t i=0;i<100;i++){
        std::cout << (*beg).to_string() << std::endl;
        ++beg;

        if(!(beg != end)){
            break;
        }
    }
    */
    //std::cout << 
    //              0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
    //              ( ( ( ) ( ) ) ( ) ( ( ( ) ( ) ) ( ) ) )
    //bit_vector b = {1,1,1,0,1,0,0,1,0,1,1,1,0,1,0,0,1,0,0,0};
    /*
    bit_vector b = {1,1,1,0,0,0};

    bp_support_sada<> bps(&b); // <- pointer to b
    for (size_t i=0; i < 3; ++i)
        cout << bps.find_open(i)<< " ";
    cout << endl;
    cout << bps.rank(0) << ", " // inclusive rank for BPS!!!
         << bps.select(4) << endl;
        */
}