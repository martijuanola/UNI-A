EXE_FILE=./bin/local_search
#INPUT_FILE=data/graph_jazz.txt
OUTPUT_FILE=src/local_search/resultatsLS.txt

N=5

HEURISTIC=3
ALGORISME=2
SI=2

#s'ha de canviar després de l'experiment
T=10000
ITR=15
K=10
L=0.075

make local_search

echo -e "A\tH\tMIN\tAVG\tVSD\tTIME\tTSD"> $OUTPUT_FILE

for INPUT_FILE in data/soc-gplus.txt data/graph_CA-CondMat.txt data/graph_CA-AstroPh.txt data/graph_CA-HepPh.txt data/graph_actors_dat.txt data/ego-facebook.txt data/socfb-Brandeis99.txt data/socfb-Mich67.txt data/graph_jazz.txt data/graph_football.txt
do
	echo -e $INPUT_FILE >> $OUTPUT_FILE
	$EXE_FILE -i $INPUT_FILE -n $N -p	-a $ALGORISME -o $OPERADORS -si $SI -h $HEURISTIC -d $DEPTH >> $OUTPUT_FILE
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