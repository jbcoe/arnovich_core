#/bin/bash

today=`date +%d%m%Y`

dbfile=yahoo_tickdb_$today.db
if [ -z $1 ]
then
	echo "ERROR: pass ticker as first argument"
	exit 1
fi
tickers='-t '${1}
mins=5 

echo `date +'%Y%m%d %H:%M:%S'`"  ####  running setup"
python py/setup.py ${tickers} --db data/${dbfile}
echo `date +'%Y%m%d %H:%M:%S'`"  ####  setup done"

heartbeat=$((60*$mins))
tags='--last --bid --ask --div --low --high --cap --open --shares --shorts --volume --yield'
while [ 1 = 1 ]
do
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  fetching data"
	python py/fetch.py ${tickers} --db data/${dbfile} ${tags}
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  fetching done"
	sleep ${heartbeat}
done

