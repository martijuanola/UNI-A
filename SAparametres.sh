EXE_FILE=./bin/local_search
OUTPUT_FILE=src/local_search/resultatsKL.txt

N=5

ALGORISME=2
OPERADORS=3
SI=2
HEURISTIC=3


DEPTH=2

T=10000
ITR=15

make local_search

echo -e "K\tL\tMIN\tAVG\tVSD\tTIME\tTSD"> $OUTPUT_FILE

for INPUT_FILE in data/soc-gplus.txt data/graph_actors_dat.txt data/socfb-Brandeis99.txt data/graph_football.txt
do
	echo -e $INPUT_FILE >> $OUTPUT_FILE
	for K in 2 6 10 14
	do
		for L in 0.1 0.075 0.05 0.025
		do
			echo -n -e $K "\t" >> $OUTPUT_FILE
			echo -n -e $L "\t" >> $OUTPUT_FILE
			$EXE_FILE -i $INPUT_FILE -n $N -p	-a $ALGORISME -o $OPERADORS -si $SI -h $HEURISTIC -d $DEPTH >> $OUTPUT_FILE
		done
		echo "" >> $OUTPUT_FILE
	done
	echo -e "\n" >> $OUTPUT_FILE
done
echo "END" >> $OUTPUT_FILE


# 23628 data/soc-gplus.txt
# 23133 data/graph_CA-CondMat.txt
# 18772 data/graph_CA-AstroPh.txt
# 12008 data/graph_CA-HepPh.txt
# 10042	data/graph_actors_dat.txt
# 4039 	data/ego-facebook.txt
# 3898	data/socfb-Brandeis99.txt
# 3748	data/socfb-Mich67.txt
# 198	data/graph_jazz.txt
# 115	data/graph_football.txt




