#include "rlbwt_iterator/rlbwt_iterator.hpp"

using namespace std;

// The example that RLBWT is constructed from a string.
void rlbwt_sample1(){

    string text = "pississmpii";
    // The last character of the text of an RLBWT must be 0.
    //text.push_back((char)0);
    // The declaration of RLBWT.
    // The first tamplate is the type of characters.
    // The second template is the type of the number of runs.
    // The third template is the type of the array storing characters.
    // The fourth template is the type of the array storing runs.
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    // Construct the RLBWT of the text.
    stool::rlbwt::Constructor::construct_from_string(rle_str, text );
    rle_str.print_info();
    //RLBWT: i^2, p^2, s^2, m^1, #^1, s^2, i^2
}


// The example that RLBWT is directly constructed.
void rlbwt_sample2(){
    std::vector<char> chars{ 'i', 'p', 's', 'm', 0, 's', 'i' };
    std::vector<uint64_t> run_vec{ 0, 2, 4, 6, 7, 8, 10, 12 };
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    rle_str.set(&chars, &run_vec);
    rle_str.print_info();
    //RLBWT: i^2, p^2, s^2, m^1, #^1, s^2, i^2
}

// The example that RLBWT is constructed from an input text file.
void rlbwt_sample3(){
    ofstream outputfile("./test.txt");
    outputfile << "pississmpii";
    outputfile.close();

    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_file(rle_str, "./test.txt");

    rle_str.print_info();
    //RLBWT: i^2, p^2, s^2, m^1, #^1, s^2, i^2

}

// The example backward enumerating values in the inverse suffix array for the input RLBWT.
void isa_sample1(){

    string text = "pississmpii";
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_string(rle_str, text);

    stool::rlbwt::BackwardISA<> isa;
    isa.construct_from_rlbwt(&rle_str);
    std::cout << "ISA(Reversed): ";
    for(uint64_t p : isa){
        std::cout << p << " ";
    }
    std::cout << std::endl;
    // ISA(Reversed): 0 1 2 6 5 9 11 4 8 10 3 7
}

// The example backward enumerating characters in the original text of the input RLBWT.
void text_sample1(){

    string text = "pississmpii";
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_string(rle_str, text);

    stool::rlbwt::BackwardText<> backward_text;
    backward_text.construct_from_rlbwt(&rle_str);
    std::cout << "Text(Reversed): ";
    for(char p : backward_text){
        std::cout << p << " ";
    }
    std::cout << std::endl;
    // Text(Reversed):  i i p m s s i s s i p
}
// The example enumerating values in the suffix array for the input RLBWT.
void sa_sample1(){

    string text = "pississmpii";
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_string(rle_str, text);

    stool::rlbwt::ForwardSA<> forward_sa;
    forward_sa.construct_from_rlbwt(&rle_str);
    std::cout << "SA: ";
    for(uint64_t p : forward_sa){
        std::cout << p << " ";
    }
    std::cout << std::endl;
    // SA: 11 10 9 1 4 7 8 0 3 6 2 5
}

// The example enumerating values in the longest common prefix array for the input RLBWT.
void lcp_sample1(){
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    rle_str.set(std::vector<char>{ 105, 112, 115, 109, 0, 115, 105 }, std::vector<uint64_t>{ 0, 2, 4, 6, 7, 8, 10, 12 });

    stool::rlbwt::ForwardLCPArray<> forward_lcp;
    forward_lcp.construct_from_rlbwt(&rle_str);
    std::cout << "LCP: ";
    for(uint64_t p : forward_lcp){
        std::cout << p << " ";
    }
    std::cout << std::endl;
    // LCP: 0 0 1 1 3 0 0 2 0 1 1 2
}

// The example postorder enumerating nodes in the suffix tree of the input RLBWT.
void st_sample1(){
    string text = "pississmpii";
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_string(rle_str, text);

    text.push_back('#');
    vector<uint64_t> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<uint64_t>(text);

    stool::rlbwt::PostorderSuffixTree<> st;
    st.construct_from_rlbwt(&rle_str, false);
    std::cout << "Suffix Tree Nodes in the postorder: " << std::endl;
    for(stool::rlbwt::LCPInterval<> p : st){
        string node_str = text.substr(sa[p.i], p.lcp);
        std::cout << "string: " << node_str << ", SA[i, j] = [" << p.i << ", " << p.j << "], node depth = " << p.lcp << std::endl;
    }
    std::cout << std::endl;
    /*
    Suffix Tree Nodes in the postorder: 
    string: #, SA[i, j] = [0, 0], node depth = 1
    string: i#, SA[i, j] = [1, 1], node depth = 2
    string: ii#, SA[i, j] = [2, 2], node depth = 3
    string: ississmpii#, SA[i, j] = [3, 3], node depth = 11
    string: issmpii#, SA[i, j] = [4, 4], node depth = 8
    string: iss, SA[i, j] = [3, 4], node depth = 3
    string: i, SA[i, j] = [1, 4], node depth = 1
    string: mpii#, SA[i, j] = [5, 5], node depth = 5
    string: pii#, SA[i, j] = [6, 6], node depth = 4
    string: pississmpii#, SA[i, j] = [7, 7], node depth = 12
    string: pi, SA[i, j] = [6, 7], node depth = 2
    string: sissmpii#, SA[i, j] = [8, 8], node depth = 9
    string: smpii#, SA[i, j] = [9, 9], node depth = 6
    string: ssissmpii#, SA[i, j] = [10, 10], node depth = 10
    string: ssmpii#, SA[i, j] = [11, 11], node depth = 7
    string: ss, SA[i, j] = [10, 11], node depth = 2
    string: s, SA[i, j] = [8, 11], node depth = 1
    string: , SA[i, j] = [0, 11], node depth = 0
    */
}

// The example postorder enumerating maximal substrings in the input RLBWT.
void ms_sample1(){
    string text = "pississmpii";
    stool::rlbwt::RLBWT<char, uint64_t, vector<char>, vector<uint64_t>> rle_str;
    stool::rlbwt::Constructor::construct_from_string(rle_str, text);

    text.push_back('#');
    vector<uint64_t> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<uint64_t>(text);

    stool::rlbwt::PostorderMaximalSubstrings<> ms;
    ms.construct_from_rlbwt(&rle_str, false);
    std::cout << "Maximal substrings in the postorder: " << std::endl;
    for(stool::rlbwt::LCPInterval<> p : ms){
        string node_str = text.substr(sa[p.i], p.lcp);
        std::cout << "string: " << node_str << ", SA[i, j] = [" << p.i << ", " << p.j << "], node depth = " << p.lcp << std::endl;
    }
    std::cout << std::endl;
    /*
    Maximal substrings in the postorder: 
    string: iss, SA[i, j] = [3, 4], node depth = 3
    string: i, SA[i, j] = [1, 4], node depth = 1
    string: pississmpii#, SA[i, j] = [7, 7], node depth = 12
    string: pi, SA[i, j] = [6, 7], node depth = 2
    string: s, SA[i, j] = [8, 11], node depth = 1
    string: , SA[i, j] = [0, 11], node depth = 0
    */

}


int main()
{
    rlbwt_sample1();
    rlbwt_sample2();
    rlbwt_sample3();

    isa_sample1();
    text_sample1();
    sa_sample1();
    lcp_sample1();
    st_sample1();
    ms_sample1();
    
}
