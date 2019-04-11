# Script para benchmark 

DIR=saida

rm -r ${DIR}/
mkdir ${DIR}/

for teste in $(seq 1 10)
do
    for cpus in $(seq 24 -1 1)
    do
        printf "mult com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        (OMP_NUM_THREADS=${cpus} ./main mult 2000 2000 ${cpus} | sed -e "s/\./,/g") >> ${DIR}/mult_${cpus}.txt
        printf "sum com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        (OMP_NUM_THREADS=${cpus} ./main sum 20000 20000 ${cpus} | sed -e "s/\./,/g") >> ${DIR}/sum_${cpus}.txt
        printf "sort com %02d cpus, iteracao %d\n" ${cpus} ${teste}
        (OMP_NUM_THREADS=${cpus} ./main sort 20000 20000 ${cpus} | sed -e "s/\./,/g") >> ${DIR}/sort_${cpus}.txt
    done
done
