# Script para benchmark 

# rm -r saida/
# mkdir saida/

for teste in $(seq 1 10)
do
    # for cpus in $(seq 12 -1 1)
    # do
    #     printf "sum com %dcpus, iteracao %d\n" ${cpus} ${teste}
    #     ./main sum 10000 10000 ${cpus} >> saida/sum_${cpus}.txt
    # done
    # for cpus in $(seq 12 -1 1)
    # do
    #     printf "sort com %dcpus, iteracao %d\n" ${cpus} ${teste}
    #     ./main sort 6000 6000 ${cpus} >> saida/sort_${cpus}.txt
    # done
    for cpus in $(seq 12 -1 1)
    do
        printf "mult com %dcpus, iteracao %d\n" ${cpus} ${teste}
        ./main mult 2000 2000 ${cpus} >> saida/mult_${cpus}.txt
    done
done