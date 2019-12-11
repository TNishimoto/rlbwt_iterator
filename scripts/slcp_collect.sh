#!/bin/sh
folderpath="/Users/nishimoto/Documents/test_data/cantrbry"

filearr=( "alice29.txt" "cp.html" "grammar.lsp" "fields.c" "lcet10.txt" "plrabn12.txt" "xargs.1")

for file in ${filearr[@]}; do
	echo "slcp_test ${file}"
    ./slcp.out -i ${folderpath}/${file}
 done


 echo "Finished."