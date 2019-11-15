#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <exception>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <time.h> //#include <stdio.h>

namespace stool
{
/*
    class CorrectRLBWT
    {

    };
    */
class RLBWTPrinter
{
public:
    static std::vector<std::string> toTateLines(std::vector<std::string> &items)
    {
        std::vector<uint64_t> tmp;
        for (auto &it : items)
            tmp.push_back(it.size());

        uint64_t max = *std::max_element(tmp.begin(), tmp.end());

        std::vector<std::string> r;
        r.resize(max);
        for (uint64_t i = 0; i < max; i++)
        {
            uint64_t rank = (max - i) - 1;
            r[i].resize(items.size(), ' ');
            for (uint64_t x = 0; x < items.size(); x++)
            {
                std::string s = items[x] ;
                if (rank < s.size())
                    r[i][x] = s[i - (max - s.size())];
            }
        }
        return r;
    }
    static void printTateLines(std::vector<std::string> &items, uint64_t colorPos, std::string lineName)
    {
        std::vector<std::string> lines = toTateLines(items);
        for (uint64_t x = 0; x < lines.size(); x++)
        {
            std::string &line = lines[x];
            for (uint64_t i = 0; i < line.size(); i++)
            {
                if (i == colorPos)
                {
                    std::cout << "\033[31m";
                    std::cout << line[i];
                    std::cout << "\033[39m";
                }
                else
                {
                    std::cout << line[i];
                }
            }

            if (x == lines.size() - 1)
                std::cout << " :" << lineName << "(Vertical writing)";
            std::cout << std::endl;
            //std::cout << it << std::endl;
        }
    }
    static void printText(std::string &text)
    {
        for (uint64_t i = 0; i < text.size(); i++)
        {
            std::cout << (text[i] == 0 ? '#' : text[i]);
        }

        std::cout << " :Text";
        std::cout << std::endl;
    }

    static std::vector<uint64_t> formatOnRLBWT(std::vector<uint64_t> &items, std::vector<uint64_t> &rlbwt_runs){
        std::vector<uint64_t> r;
        for (uint64_t i = 0; i < rlbwt_runs.size(); i++)
        {
            r.push_back(items[i]);
            for (uint64_t x = 1; x < rlbwt_runs[i]; x++)
            {
                r.push_back(UINT64_MAX);
            }
        }
        return r;        
    }
    static std::vector<char> formatOnRLBWT(std::vector<char> &items, std::vector<uint64_t> &rlbwt_runs){
        std::vector<char> r;
        for (uint64_t i = 0; i < rlbwt_runs.size(); i++)
        {
            r.push_back(items[i]);
            for (uint64_t x = 1; x < rlbwt_runs[i]; x++)
            {
                r.push_back('-');
            }
        }
        return r;        
    }
    static void printRLBWT(std::vector<char> &rlbwt_chars, std::vector<uint64_t> &rlbwt_runs)
    {
        std::vector<char> r = formatOnRLBWT(rlbwt_chars, rlbwt_runs);
        for (uint64_t i = 0; i < r.size(); i++)
        {
            std::cout << (r[i] == 0 ? '#' : r[i]);
        }
        std::cout << " :RLBWT";
        std::cout << std::endl;
    }

    static std::vector<std::string> to_string_vector(std::vector<uint64_t> &items){
        std::vector<std::string> r;
        for(auto& it :items){
            if(it == UINT64_MAX){
                r.push_back("-");
            }else{
                r.push_back(std::to_string(it));
            }
        }
        return r;
    }
    static void printRLBWTIndexes(std::vector<uint64_t> &rlbwt_runs)
    {
        std::vector<uint64_t> r;
        r.resize(rlbwt_runs.size());
        for (uint64_t i = 0; i < rlbwt_runs.size(); i++)
            r[i] = i;

        std::vector<uint64_t> r2 = formatOnRLBWT(r, rlbwt_runs);
        std::vector<std::string> r3 = to_string_vector(r2);


        RLBWTPrinter::printTateLines(r3, UINT64_MAX, "Index");
    }

    static void printOnRLBWT(std::vector<uint64_t> &items, std::vector<uint64_t> &rlbwt_runs, std::string linename)
    {
        std::vector<uint64_t> r = formatOnRLBWT(items, rlbwt_runs);
        std::vector<std::string> r2 = to_string_vector(r);


        RLBWTPrinter::printTateLines(r2, UINT64_MAX,  linename );
    }
    static void printWithFOrder(std::vector<uint64_t> &items, std::vector<uint64_t> &rlbwt_runs, std::vector<uint64_t> &lf_mapper, std::string linename)
    {
        std::vector<uint64_t> runs_forder;
        runs_forder.resize(rlbwt_runs.size(),0);
        for(uint64_t i=0;i<lf_mapper.size();i++){
            runs_forder[lf_mapper[i]] = rlbwt_runs[i];
        }
        std::vector<uint64_t> r2 = formatOnRLBWT(items, runs_forder);
        std::vector<std::string> r3 = to_string_vector(r2);
        RLBWTPrinter::printTateLines(r3, UINT64_MAX,  linename );
    }

    static void print(std::vector<uint64_t> &items, std::string linename)
    {
        std::vector<std::string> r2 = to_string_vector(items);
        RLBWTPrinter::printTateLines(r2, UINT64_MAX,  linename );
    }

    static void printOnRLBWT(std::vector<char> &items, std::vector<uint64_t> &rlbwt_runs, std::string linename)
    {
        std::vector<char> r = formatOnRLBWT(items, rlbwt_runs);
        for (uint64_t i = 0; i < r.size(); i++)
        {
            std::cout << (r[i] == 0 ? '#' : r[i]);
        }
        std::cout << " :" << linename;
        std::cout << std::endl;
    }


};
} // namespace stool