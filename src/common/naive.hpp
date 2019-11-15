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
class NaiveAlgorithms
{
    template <typename INDEX>
    static std::vector<INDEX> naive_sa(std::string &text)
    {
        std::vector<INDEX> sa;
        INDEX n = text.size();
        sa.resize(n);
        for (INDEX i = 0; i < n; i++)
        {
            sa[i] = i;
        }
        std::sort(sa.begin(), sa.end(),
                  [&](const INDEX &x, const INDEX &y) {
                      INDEX size = x < y ? text.size() - y : text.size() - x;
                      for (INDEX i = 0; i < size; i++)
                      {
                          if ((uint8_t)text[x + i] != (uint8_t)text[y + i])
                          {
                              return (uint8_t)text[x + i] < (uint8_t)text[y + i];
                          }
                      }
                      return x <= y ? false : true;
                  });
        return sa;
    }
};
} // namespace stool