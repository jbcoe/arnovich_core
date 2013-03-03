#/bin/bash

logdir=logs/
pidfile=${logdir}run_parallel.pids

while read line;do 
	kill ${line}
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  killed feed on ${line}"
done < $pidfile

