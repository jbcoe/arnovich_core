
# small script for

from optparse import OptionParser
import sqlite3
import time
import string
import arnovich.core as core

def parse_command_args():
	parser = OptionParser()

	parser.add_option("-t", "--ticker", action="append", type="string", dest="tickers")
	parser.add_option("--from", action="store", type="string", dest="fromdate", default="")
	parser.add_option("--to", action="store", type="string", dest="todate", default="")
	parser.add_option("--db", action="store", dest="dbfile")
	parser.add_option("--wait", action="store", type="int", dest="wait", default=1)

	(options, args) = parser.parse_args()
	return (options.dbfile, options.tickers, options.fromdate, options.todate, options.wait)

def find_dates(dbfile, tickers, fromdate, todate, wait):
	if fromdate != "":
		fromtime = time.mktime(time.strptime(fromdate, "%Y-%m-%d %H:%M:%S"))
	if todate != "":
		totime = time.mktime(time.strptime(todate, "%Y-%m-%d %H:%M:%S"))

	connection = core.connection()
	sql_tickers = string.join(tickers, "\",\"")

	conn = sqlite3.connect(dbfile)
	c = conn.cursor()
	d = conn.cursor()
	c.execute("select ticker_id, ticker from stocks_static_data where ticker in (\""+sql_tickers+"\")")
	prevtime = 0
	for ticker_id in c:
		#should check if it already exists using get_ticker
		srv_id = connection.add_ticker(str(ticker_id[1]))
		srv_id_opt = connection.add_ticker(str(ticker_id[1])+"_options")
		if (fromdate == "") or (todate == ""):
			d.execute("select date, data from stocks_data where ticker_id="+str(ticker_id[0])+" ORDER BY date")
		else:
			d.execute("select date, data from stocks_data where ticker_id="+str(ticker_id[0])+" and (date > "+str(fromtime)+" AND date < "+str(totime)+")")
		for r in d:
			rowdate = str(r[0])
			rowdata = str(r[1])
			rowtime = float(r[0])
			if prevtime == 0:
				prevtime = rowtime
			connection.push_ticker(srv_id, rowdata)
			vcursor = conn.cursor()
			vcursor.execute("select data from options_data where ticker_id="+str(ticker_id[0])+" and date="+rowdate)
			for row in vcursor:
				connection.push_ticker(srv_id_opt, str(row[0]))
			#TODO make this better: take exec time into consideration
			time.sleep((rowtime-prevtime)/wait)
			prevtime = rowtime
	c.close()

def main():
	(dbfile, tickers, fromdate, todate, wait) = parse_command_args()
	find_dates(dbfile, tickers, fromdate, todate, wait)

if __name__ == "__main__":
	main()

