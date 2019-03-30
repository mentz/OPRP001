# Script para benchmark 

rm -r saida/
mkdir saida/

for teste in $(seq 1 10)
do
    for cpus in $(seq 24 -1 1)
    do
        printf "mult com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main mult 2000 2000 ${cpus} >> saida/mult_${cpus}.txt
    done
    for cpus in $(seq 1 24)
    do
        printf "sum com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main sum 20000 20000 ${cpus} >> saida/sum_${cpus}.txt
    done
    for cpus in $(seq 1 24)
    do
        printf "sort com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main sort 20000 20000 ${cpus} >> saida/sort_${cpus}.txt
    done
done
