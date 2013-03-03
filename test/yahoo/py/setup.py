
from optparse import OptionParser
import sqlite3
import string
import time
import urllib
import sgmllib
import os.path
import arnovich.core as core

class OptionTickerParser(sgmllib.SGMLParser):

	def __init__(self, ticker, verbose=0):
		sgmllib.SGMLParser.__init__(self, verbose)
		self.ticker = ticker
		self.tenors = list()
		self.tickers = list()

	def start_a(self, attributes):
		for name, value in attributes:
			if name == "href":
				id = "/q/op?s="+self.ticker+"&m="
				i = string.find(value, id)
				if i != -1:
					value = value[len(id):]
					if len(value) > 0:
						self.tenors.append(value)
				else:
					id = "/q?s="+self.ticker
					i = string.find(value, id)
					if (i != -1):
						value = value[len(id):]
						if len(value) > 0:
							self.tickers.append(value)
	def close(self):
		sgmllib.SGMLParser.close(self)
		self.tenors = list()
		self.tickers = list()

def parse_command_args():
	parser = OptionParser()

	parser.add_option("-t", "--ticker", action="append", type="string", dest="tickers")
	parser.add_option("--clean", action="store_true", dest="do_clean", default=False)
	parser.add_option("--db", action="store", dest="dbfile")
	parser.add_option("--srv", action="store_true", dest="do_srv", default=False)

	(options, args) = parser.parse_args()

	return (options.tickers, options.do_clean, options.dbfile, options.do_srv)


def insert_stock(dbfile, ticker, datetime, do_srv):
	conn = sqlite3.connect(dbfile, 5)
	cursor = conn.cursor()
	cursor.execute("SELECT * FROM stocks_static_data WHERE ticker=\""+ticker+"\"")
	if cursor.fetchone() == None:
		cursor.execute("INSERT INTO stocks_static_data VALUES(\""+ticker+"\",NULL)")
		print "inserted ticker "+ticker
	conn.commit()
	cursor.close()
	conn.close()
	if do_srv == True:
		c = core.connection()
		c.add_ticker(ticker)

def insert_options(dbfile, ticker, datetime, do_srv):

	conn = sqlite3.connect(dbfile, 5)
	cursor = conn.cursor()
	cursor.execute("SELECT ticker_id FROM stocks_static_data WHERE ticker=\""+ticker+"\"")
	tickerid = cursor.fetchone()[0]

	f = urllib.urlopen("http://finance.yahoo.com/q/op?s="+ticker+"+Options")
	s = f.read()
	optionparser = OptionTickerParser(ticker)
	optionparser.feed(s)

	tickers = optionparser.tickers
	tenors = optionparser.tenors
	optionparser.close()
	for t in tenors:
		f = urllib.urlopen("http://finance.yahoo.com/q/op?s="+ticker+"&m="+t)
		s = f.read()
		optionparser.feed(s)
		tickers.extend(optionparser.tickers)
		optionparser.close()

	n = 0
	for t in tickers:
		if len(t) == 15:
			date = t[:6]
			datet = time.mktime(time.strptime(date,"%y%m%d"))
			putcall = t[6]
			strike = float(t[7:])/1000
			cursor.execute("INSERT INTO options_static_data VALUES("+str(tickerid)+","+str(datet)+","+str(strike)+",\""+putcall+"\")")
			n = n + 1
	print "inserted "+str(n)+" options over "+str(len(tenors))+" tenors for ticker "+ticker
	conn.commit()
	cursor.close()
	conn.close()
	if do_srv == True:
		c = core.connection()
		c.add_ticker(ticker+"_options")

def setup_tables(tickers, dbfile, do_clean, do_srv):
	exists = os.path.isfile(dbfile)
	conn = sqlite3.connect(dbfile, 5)
	c = conn.cursor()
	if exists == True:
		c.execute("DROP TABLE options_static_data")
		if do_clean == True:
			c.execute("DROP TABLE stocks_static_data")
			c.execute("DROP TABLE options_data")
			c.execute("DROP TABLE stocks_data")
	if (do_clean == True) or (exists == False):
		c.execute("CREATE TABLE options_data (ticker_id INTEGER, date FLOAT, data BLOB)")
		c.execute("CREATE TABLE stocks_static_data (ticker TEXT, ticker_id INTEGER PRIMARY KEY AUTOINCREMENT)")
		c.execute("CREATE TABLE stocks_data (ticker_id INTEGER, date FLOAT, data BLOB)")
	c.execute("CREATE TABLE options_static_data (ticker_id INTEGER, maturity FLOAT, strike FLOAT, putcall TEXT)")

	c.close()
	conn.close()

	datetime = time.time()
	for t in tickers:
		insert_stock(dbfile, t, datetime, do_srv)
		insert_options(dbfile, t, datetime, do_srv)


def main():

	(tickers, do_clean, dbfile, do_srv) = parse_command_args()
	setup_tables(tickers, dbfile, do_clean, do_srv)


if __name__ == "__main__":
	main()
