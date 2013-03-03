#/bin/bash

ticker_file=$1
if [ -z $1 ]
then
	ticker_file=tickers.txt
fi

logdir=logs/
pidfile=${logdir}run_parallel.pids
rm -f ${pidfile}

while read line;do 
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  starting feed for ${line}"
	./run_ticker.sh ${line} >> ${logdir}${line}.log &
	echo "$!" >> ${pidfile}
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  started feed on $!"
	sleep 5
done < $ticker_file

wait

