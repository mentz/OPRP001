# Script para benchmark 

DIR=saidaUDESC

rm -r ${DIR}/
mkdir ${DIR}/

for teste in $(seq 1 10)
do
    for cpus in $(seq 24 -1 1)
    do
        printf "mult com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main mult 2000 2000 ${cpus} >> ${DIR}/mult_${cpus}.txt
    done
    for cpus in $(seq 1 24)
    do
        printf "sum com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main sum 10000 10000 ${cpus} >> ${DIR}/sum_${cpus}.txt
    done
    for cpus in $(seq 1 24)
    do
        printf "sort com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        ./main sort 4000 4000 ${cpus} >> ${DIR}/sort_${cpus}.txt
    done
done
