# Script para benchmark 

DIR=saida

rm -r ${DIR}/
mkdir ${DIR}/

for teste in $(seq 1 10)
do
    printf "mult com 1 cpus, iteracao %d\n" ${teste}
    (./main mult 2000 2000 1 | sed -e "s/\./,/g") >> ${DIR}/mult_1.txt
    printf "sum com 1 cpus, iteracao %d\n" ${teste}
    (./main sum 20000 20000 1 | sed -e "s/\./,/g") >> ${DIR}/sum_1.txt
    printf "sort com 1 cpus, iteracao %d\n" ${teste}
    (./main sort 20000 20000 1 | sed -e "s/\./,/g") >> ${DIR}/sort_1.txt
done
