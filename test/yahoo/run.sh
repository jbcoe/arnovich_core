#/bin/bash

today=`date +%d%m%Y`

dbfile=yahoo_tickdb_$today.db
ticker_file=$1
if [ -z $1 ]
then
	ticker_file=tickers.txt
fi
secs=10 


tickers=''
while read line;do 
	tickers=${tickers}'-t '${line}' ';
	export tickers
done < $ticker_file

echo `date +'%Y%m%d %H:%M:%S'`"  ####  running setup"
python py/setup.py ${tickers} --db data/${dbfile} --srv
echo `date +'%Y%m%d %H:%M:%S'`"  ####  setup done"

heartbeat=$secs
tags='--last --bid --ask --div --low --high --cap --open --shares --shorts --volume --yield'
while [ 1 = 1 ]
do
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  fetching data"
	python py/fetch.py ${tickers} --db data/${dbfile} ${tags} --srv
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  fetching done"
	sleep ${heartbeat}
done

