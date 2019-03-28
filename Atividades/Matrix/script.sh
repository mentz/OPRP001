# Script para benchmark 

rm -r saida/
mkdir saida/

for  in $(seq 1 12)
do
    for i in $(seq 1 10)
    do
        ./main mult 5000 5000 $i >> mult_${i}.txt
        ./main sum  5000 5000 $i >> sum_${i}.txt
        ./main sort 5000 5000 $i >> sort_${i}