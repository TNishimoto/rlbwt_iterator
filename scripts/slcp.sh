#!/bin/sh
folderpath="/mnt/server/nishimoto/text"
outputpath="/mnt/server/nishimoto/output/rlbwt_txt"

filearr=( "fib41" "rs.13" "tm29" "dblp.xml.00001.1" "dblp.xml.00001.2" "dblp.xml.0001.1" "dblp.xml.0001.2" "sources.001.2" "dna.001.1" "proteins.001.1" "english.001.2" "einstein.de.txt" "einstein.en.txt" "world_leaders" "influenza" "kernel" "cere" "coreutils" "Escherichia_Coli" "para")

#for file in ${filearr[@]}; do
#	echo "sclp test old ${file}"
#    nohup /usr/bin/time -f "#slcp test old ${file}, %e sec, %M KB" ~/rlbwt_iterator/build/time_lcp.out -i ${outputpath}/${file}.rlbwt >> /mnt/server/nishimoto/result/slcp_old_output.log
#done

#for file in ${filearr[@]}; do
#	echo "sclp test new ${file}"
#    nohup /usr/bin/time -f "#slcp test new ${file}, %e sec, %M KB" ~/rlbwt_iterator/build/time_lcp.out -i ${outputpath}/${file}.rlbwt -m new >> /mnt/server/nishimoto/result/slcp0_new_output.log
#done

for file in ${filearr[@]}; do
	echo "sclp test hyper weiner ${file}"
    nohup /usr/bin/time -f "#slcp test hyper weiner ${file}, %e sec, %M KB" ~/rlbwt_iterator/build/time_lcp.out -i ${outputpath}/${file}.rlbwt -m weiner >> /mnt/server/nishimoto/result/slcp0_weiner_output.log
done

