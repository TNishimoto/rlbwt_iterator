# RLBWT Iterator

This library provides some iterators (e.g., suffix array, inverse suffix array, longest common perfix array, and etc.) in small working memory using a given Run-Length BWT (RLBWT).  
Our iterating algorithms on RLBWT is based on the following paper.  
  
>Travis Gagie, Gonzalo Navarro, Nicola Prezza:  
>Optimal-Time Text Indexing in BWT-runs Bounded Space. SODA 2018: 1459-1477  

This library supports the following operations:  

+ The construction of the [RLBWT](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/rlbwt.hpp "RLBWT") of T for a given string T.  
    * This construction algorithm uses [OnlineRLBWT](https://github.com/itomomoti/OnlineRlbwt "OnlineRLBWT") implemented by Tomohiro I.  
+ [The backward iterator for the inverse suffix array(ISA)](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/backward_isa.hpp "The backward iterator for the inverse suffix array(ISA)") of T for a given RLBWT of T.  
+ [The backward iterator for the text](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/backward_text.hpp "The backward iterator for the text") of T for a given RLBWT of T.  
+ [The Forward iterator for the suffix array(SA)](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/forward_sa.hpp "The Forward iterator for the suffix array(SA)") of T for a given RLBWT of T.  
+ [The Forward iterator for the longest common prefix(LCP)](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/forward_lcp_array.hpp "The Forward iterator for the longest common prefix(LCP)") of T for a given RLBWT of T.  
+ [The postorder iterator for the suffix tree nodes](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/postorder_suffix_tree.hpp "The postorder iterator for the suffix tree nodes") of T for a given RLBWT of T.  
+ [The postorder iterator for maximal substrings](https://github.com/TNishimoto/rlbwt_iterator/blob/master/src/include/postorder_maximal_substrings.hpp#L185 "The postorder iterator for maximal substrings") in T for a given RLBWT of T.  

## Download

The source codes in 'module' directory are maintained in different repositories.
So, to download all the necessary source codes, do the following:

> git clone https://github.com/TNishimoto/rlbwt_iterator.git  
> cd rlbwt_iterator  
> git submodule init  
> git submodule update  

## Compile

> mkdir build  
> cd build  
> cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="./" ..  
> make  
> make install  

After you excecute above commands, lib and include directories are created in the build directory.  
You can use iterators in this library using the two directories.  
See also [the sample program](https://github.com/TNishimoto/rlbwt_iterator/blob/master/test/sample_main.cpp "the sample program") in test directory 
and the [CMakeLists.txt](https://github.com/TNishimoto/rlbwt_iterator/blob/master/test/CMakeLists.txt "CMakeLists.txt") of the file.  

## license

This program is released under the MIT licence.
