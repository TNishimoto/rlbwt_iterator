#!/bin/sh
folderpath="/Users/nishimoto/Documents/test_data/cantrbry"

filearr=( "alice29.txt" "cp.html" "grammar.lsp" "fields.c" "lcet10.txt" "plrabn12.txt" "xargs.1")

for file in ${filearr[@]}; do
	echo "slcp_test with sdsl ${file}"
    ./slcp.out -i ${folderpath}/${file} -m sdsl
	echo "slcp_test with plain vector ${file}"
    ./slcp.out -i ${folderpath}/${file} -m new
 done


 echo "Finished."