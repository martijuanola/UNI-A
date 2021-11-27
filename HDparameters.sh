EXE_FILE=./bin/local_search
#INPUT_FILE=data/graph_jazz.txt
OUTPUT_FILE=src/local_search/resultatsHD.txt

N=5

ALGORISME=2
OPERADORS=3
SI=2

make local_search

echo -e "H\tD\tMIN\tAVG\tVSD\tTIME\tTSD"> $OUTPUT_FILE

for INPUT_FILE in data/soc-gplus.txt data/graph_actors_dat.txt data/socfb-Brandeis99.txt data/graph_football.txt
do
	echo -e $INPUT_FILE >> $OUTPUT_FILE
	for HEURISTIC in 1 2
	do
		for DEPTH in 3 5 7 9
		do
			echo -n -e $HEURISTIC "\t" >> $OUTPUT_FILE
			echo -n -e $DEPTH "\t" >> $OUTPUT_FILE
			$EXE_FILE -i $INPUT_FILE -n $N -p	-a $ALGORISME -o $OPERADORS -si $SI -h $HEURISTIC -d $DEPTH >> $OUTPUT_FILE
		done
		echo "" >> $OUTPUT_FILE
	done
	echo -e "\n" >> $OUTPUT_FILE
done
echo "END" >> $OUTPUT_FILE