for ip in `cat ips.txt`;
do
	echo $ip
	scp quebra $ip:~/.
done
