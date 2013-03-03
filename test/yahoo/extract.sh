#/bin/bash

today=`date +%d%m%Y`

dbfile=yahoo_tickdb_$today.db
ticker_file=tickers.txt
mins=2 

tags='-f last -f bid -f ask -f div -f low -f high -f cap -f open -f shares -f shorts -f volume -f yield'

tickers=''
echo `date +'%Y%m%d %H:%M:%S'`"  ####  extracting data from ${dbfile}"
while read line;do 
	tickers=${tickers}'-t '${line}' ';
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  extracting all data for ${line}"
	python py/extract.py -t ${line} --dir exports --db data/${dbfile} ${tags}
	echo `date +'%Y%m%d %H:%M:%S'`"  ####  extraction done"
done < $ticker_file

