from optparse import OptionParser
import sqlite3
import time
import string
import pickle


def parse_command_args():
	parser = OptionParser()
	
	parser.add_option("-t", "--ticker", action="append", type="string", dest="tickers")
	parser.add_option("-f", "--field", action="append", type="string", dest="fields")
	parser.add_option("--from", action="store", type="string", dest="fromdate", default="")
	parser.add_option("--to", action="store", type="string", dest="todate", default="")
	parser.add_option("--db", action="store", dest="dbfile")
	parser.add_option("--dir", action="store", dest="dir", default="./")

	(options, args) = parser.parse_args()
	options.dir=str(options.dir)+"/"
	return (options.dbfile, options.tickers, options.fromdate, options.todate, options.fields, options.dir)


def extract_vol(dbfile, ticker_id, ticker, date, spot, conn, dir):
	cursor = conn.cursor()
	cursor.execute("select data from options_data where ticker_id="+str(ticker_id)+" and date="+str(date))
	for row in cursor:
		fout = open(dir+ticker+"_"+time.strftime("%d%m%y_%H%M%S", time.gmtime(float(date)))+".csv","w")
		datadict = pickle.loads(str(row[0]))
		first = True
		for maturity,i in datadict.iteritems():
			for strike,j in i.iteritems():
				for putcall,k in j.iteritems():
					outstr = time.strftime("%Y-%m-%d", maturity) + "," + str(strike) + "," + putcall + ","
					if first == True:
						header = "Maturity,Strike,PutCall,"
					for key, item in k.iteritems():
						if first == True:
							header = header + key + ","
						outstr = outstr + item + ","
					if first == True:
						fout.write(header+"\n")
						first = False
					fout.write(outstr+"\n")
		fout.close()

def find_dates(dbfile, tickers, fromdate, todate, fields, dir):
	if fromdate != "":
		fromtime = time.mktime(time.strptime(fromdate, "%Y-%m-%d %H:%M:%S"))
	if todate != "":
		totime = time.mktime(time.strptime(todate, "%Y-%m-%d %H:%M:%S"))
	spot = 0.0

	sql_tickers = string.join(tickers, "\",\"")

	conn = sqlite3.connect(dbfile)
	c = conn.cursor()
	d = conn.cursor()
	c.execute("select ticker_id, ticker from stocks_static_data where ticker in (\""+sql_tickers+"\")")
	for ticker_id in c:
		if (fromdate == "") or (todate == ""):
			d.execute("select date, data from stocks_data where ticker_id="+str(ticker_id[0]))
		else:
			d.execute("select date, data from stocks_data where ticker_id="+str(ticker_id[0])+" and (date > "+str(fromtime)+" AND date < "+str(totime)+")")
		header = "date"
		for f in fields:
			header = header + "," + f
		fout = open(dir+ticker_id[1]+".csv","w")
		fout.write(header + "\n")
		for r in d:
			row = str(r[0])
			datadict = pickle.loads(str(r[1]))
			for f in fields:
				if f == "last":
					spot = float(datadict[f])
				row = row + "," + datadict[f]
			fout.write(row + "\n")
			extract_vol(dbfile, ticker_id[0], ticker_id[1], r[0], spot, conn, dir)
		fout.close()
	c.close()

def print_data_info(dbfile, tickers):
	sql_tickers = string.join(tickers, "\",\"")
	conn = sqlite3.connect(dbfile)
	c = conn.cursor()
	d = conn.cursor()
	c.execute("select ticker_id, ticker from stocks_static_data where ticker in (\""+sql_tickers+"\")")
	for ticker in c:
		print "Ticker="+str(ticker[1])
		d.execute("select date from stocks_data where ticker_id="+str(ticker[0]))
		for r in d:
			print time.strftime("%d-%m-%y  %H:%M:%S", time.gmtime(r[0]))

def main():
	(dbfile, tickers, fromdate, todate, fields, dir) = parse_command_args()
	if fields == None:
		print_data_info(dbfile, tickers)
	else:
		find_dates(dbfile, tickers, fromdate, todate, fields, dir)

if __name__ == "__main__":
	main()

