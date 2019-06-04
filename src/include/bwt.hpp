#pragma once
#include <vector>
#include <string>
#include <memory>
namespace stool
{
  namespace rlbwt{
class SuffixArrayConstructor
{
public:
  //static std::vector<uint64_t> construct_sa(std::string &text);
  //static std::string construct_bwt(std::string &text);
  //static std::vector<uint64_t> construct_lcp(std::string &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa);
static std::vector<uint64_t> construct_lcp(std::string &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa)
{
    std::vector<uint64_t> lcp;

    lcp.resize(text.size(), 0);
    uint64_t n = text.size();
    uint64_t k = 0;
    //std::cout << "Constructing lcp array" << std::flush;
    //uint64_t counter = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        /*
        if (counter != 0)
        {
            --counter;
        }
        else
        {
            std::cout << "." << std::flush;
            counter = 10000000;
        }
        */

        uint64_t x = isa[i];
        if (x == 0)
        {
            lcp[x] = 0;
        }
        else
        {
            while (text[sa[x] + k] == text[sa[x - 1] + k])
            {
                k++;
            }
        }
        lcp[x] = k;
        if (k > 0)
            k--;
    }
    //std::cout << "[END]" << std::endl;
    return lcp;
}

  //static std::vector<uint64_t> construct_isa(std::vector<uint64_t> &sa);

static std::vector<uint64_t> construct_isa(std::vector<uint64_t> &sa)
{
    std::vector<uint64_t> outputISA;
    //std::cout << "Constructing Inverse Suffix Array..." << std::flush;

    uint64_t n = sa.size();
    outputISA.resize(n);

    for (uint64_t i = 0; i < n; ++i)
    {
        outputISA[sa[i]] = i;
    }
    //std::cout << "[END]" << std::endl;
    return outputISA;
}

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
  template <typename INDEX>
  static std::string construct_bwt(std::string &text, std::vector<INDEX> &sa)
  {

    std::string output;
    output.resize(text.size());
    for (uint64_t i = 0; i < text.size(); i++)
    {
      if (sa[i] == 0)
      {
        output[i] = text[text.size() - 1];
      }
      else
      {
        output[i] = text[sa[i] - 1];
      }
    }
    return output;
  }
};
  }
} // namespace stool