make
./share_quebra.sh
time mpirun -N 1 -machinefile ips.txt ~/quebra 3 < input
