import urllib
from optparse import OptionParser
import sys
import time
import pickle
import sqlite3
import string
import time
import sgmllib
import arnovich.core as core
import base64

# TODO implement function in extract_api which returns dict[ticker][date] = lists of tenors&strikes
# TODO to be used by other python apps
# TODO do script for historical stock data

class OptionTickerParser(sgmllib.SGMLParser):

	def __init__(self, ticker, verbose=0):
		sgmllib.SGMLParser.__init__(self, verbose)
		self.in_table = False
		self.table_count = 0
		self.in_key = False
		self.in_item = False
		self.key = ""
		self.data = dict()

	def start_table(self, attributes):
		for name, value in attributes:
			if (name == "id") and ((value == "table1") or (value == "table2")):
				self.in_table = True
		if self.in_table == True:
			self.table_count = self.table_count + 1

	def end_table(self):
		if self.in_table == True:
			self.table_count = self.table_count - 1
		if self.table_count == 0:
			self.in_table = False

	def start_th(self, attributes):
		if self.in_table == True:
			self.in_key = True

	def end_th(self):
		if self.in_table == True:
			self.in_key = False

	def start_td(self, attributes):
		if self.in_table == True:
			for name, value in attributes:
				if (name == "class") and (value == "yfnc_tabledata1"):
					self.in_item = True

	def end_td(self):
		if self.in_table == True:
			self.in_item = False

	def handle_data(self, data):
		if self.in_key == True:
			i = string.find(data,":")
			self.key = string.replace(string.replace(data[:i], " ", ""), "'", "")
		if self.in_item == True:
			self.data[self.key] = data

class OptionTickerParser2(sgmllib.SGMLParser):

	def __init__(self, ticker, verbose=0):
		sgmllib.SGMLParser.__init__(self, verbose)
		self.ticker = ticker
		self.tenors = list()
		self.data = dict()
		self.in_key = False
		self.key = ""
		self.option_date = 0.0
		self.option_putcall = ""
		self.option_strike = 0.0
		self.in_option = False
		self.open_interest = False

	def end_tr(self, attributes):
		self.in_option = False
		self.open_interest = False

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
						if len(value) == 15:
							date = value[:6]
							putcall = value[6]
							strike = value[7:]
							self.option_date = time.strptime(date,"%y%m%d")
							self.option_putcall = str(putcall)
							self.option_strike = float(strike)/1000.0
							self.in_option = True

	def start_td(self, attributes):
		if self.open_interest == True:
			self.key = "OpenInterest"
			self.in_key = True
		self.open_interest = False

	def start_span(self, attributes):
		if self.in_option == True:
			for name, value in attributes:
				if name == "id":
					if value[:8] == "yfs_l10_":
						self.key = "Last"
						self.in_key = True
					elif value[:8] == "yfs_c10_":
						self.key = "Change"
						self.in_key = True
					elif value[:8] == "yfs_b00_":
						self.key = "Bid"
						self.in_key = True
					elif value[:8] == "yfs_a00_":
						self.key = "Ask"
						self.in_key = True
					elif value[:8] == "yfs_v00_":
						self.key = "Volume"
						self.in_key = True
						self.open_interest = True

	def end_span(self):
		self.in_key = False

	def end_td(self):
		self.in_key = False

	def handle_data(self, data):
		if self.in_key == True:
			self.data.setdefault(self.option_date,dict()).setdefault(self.option_strike,dict()).setdefault(self.option_putcall,dict())[self.key] = data

	def close(self):
		sgmllib.SGMLParser.close(self)
		self.tenors = list()
		self.in_key = False

def parse_command_args():
	parser = OptionParser()
	parser.add_option("-t", "--ticker", action="append", type="string", dest="ticker")

	#TAG OPTIONS
	parser.add_option("--ask", action="store_true", dest="do_ask", default=False)
	parser.add_option("--asksize", action="store_true", dest="do_asksize", default=False)
	parser.add_option("--bid", action="store_true", dest="do_bid", default=False)
	parser.add_option("--bidsize", action="store_true", dest="do_bidsize", default=False)
	parser.add_option("--div", action="store_true", dest="do_div", default=False)
	parser.add_option("--last", action="store_true", dest="do_last", default=False)
	parser.add_option("--lastdate", action="store_true", dest="do_lastdate", default=False)
	parser.add_option("--lasttime", action="store_true", dest="do_lasttime", default=False)
	parser.add_option("--lastsize", action="store_true", dest="do_lastsize", default=False)
	parser.add_option("--low", action="store_true", dest="do_low", default=False)
	parser.add_option("--high", action="store_true", dest="do_high", default=False)
	parser.add_option("--cap", action="store_true", dest="do_cap", default=False)
	parser.add_option("--open", action="store_true", dest="do_open", default=False)
	parser.add_option("--shares", action="store_true", dest="do_shares", default=False)
	parser.add_option("--shorts", action="store_true", dest="do_shorts", default=False)
	parser.add_option("--volume", action="store_true", dest="do_volume", default=False)
	parser.add_option("--yield", action="store_true", dest="do_yield", default=False)
	parser.add_option("--outof", action="store_true", dest="do_outof", default=False)

	#DB OPTION
	parser.add_option("--db", action="store", dest="dbfile")
	parser.add_option("--srv", action="store_true", dest="do_srv", default=False)

	(options, args) = parser.parse_args()

	tags = ""
	keys = list()
	if options.do_ask == True:
		tags = tags + "a3"
		keys.append("ask")
	if options.do_asksize == True:
		tags = tags + "a5"
		keys.append("asksize")
	if options.do_bid == True:
		tags = tags + "b3"
		keys.append("bid")
	if options.do_bidsize == True:
		tags = tags + "b6"
		keys.append("bidsize")
	if options.do_div == True:
		tags = tags + "d"
		keys.append("div")
	if options.do_last == True:
		tags = tags + "l1"
		keys.append("last")
	if options.do_lastdate == True:
		tags = tags + "d1"
		keys.append("lastdate")
	if options.do_lasttime == True:
		tags = tags + "t1"
		keys.append("lasttime")
	if options.do_lastsize == True:
		tags = tags + "k3"
		keys.append("lastsize")
	if options.do_low == True:
		tags = tags + "g"
		keys.append("low")
	if options.do_high == True:
		tags = tags + "h"
		keys.append("high")
	if options.do_cap == True:
		tags = tags + "j3"
		keys.append("cap")
	if options.do_open == True:
		tags = tags + "o"
		keys.append("open")
	if options.do_shares == True:
		tags = tags + "s1"
		keys.append("shares")
	if options.do_shorts == True:
		tags = tags + "s7"
		keys.append("shorts")
	if options.do_volume == True:
		tags = tags + "v"
		keys.append("volume")
	if options.do_yield == True:
		tags = tags + "y"
		keys.append("yield")
	if options.do_outof == True:
		tags = tags + "c8"
		keys.append("after")
	tickers = options.ticker
	return (tickers, tags, keys, options.dbfile, options.do_srv)


def get_data(tickers, tags):
	if len(tags) == 0:
		print "no tags given"
		sys.exit(0)
	if len(tickers) == 0:
		print "no tickers given"
		sys.exit(0)
	yahoo_url = "http://download.finance.yahoo.com/d/quotes.csv?s="
	ticker = ""
	for t in tickers:
		if len(ticker) > 0:
			ticker = ticker + ","
		ticker = ticker + t
	url = yahoo_url+ticker+"&f="+tags
	u = urllib.urlopen(url)
	record = dict()
	for t in tickers:
		s = u.readline()
		record[t] = s
	u.close()
	return record

def get_data_html(ticker):
	yahoo_url = "http://finance.yahoo.com/q?s="
	url = yahoo_url+ticker
	u = urllib.urlopen(url)
	s = u.read()
	optionparser = OptionTickerParser(ticker)
	optionparser.feed(s)
	optionparser.close()
	u.close()
	return optionparser.data

def get_data_html2(ticker):
	f = urllib.urlopen("http://finance.yahoo.com/q/op?s="+ticker+"+Options")
	s = f.read()
	data = dict()
	optionparser = OptionTickerParser2(ticker, data)
	optionparser.feed(s)

	tenors = optionparser.tenors
	optionparser.close()
	for t in tenors:
		f = urllib.urlopen("http://finance.yahoo.com/q/op?s="+ticker+"&m="+t)
		s = f.read()
		optionparser.feed(s)
		optionparser.close()

	return optionparser.data

def data_to_dict(data, keys):
	tokens = data.split(',')
	i = 0
	values = dict()
	for t in tokens:
		values[keys[i]] = t.strip('\n\r')
		i = i+1
	return values

def get_ticker_id(dbfile, ticker):
	conn = sqlite3.connect(dbfile, 5)
	c = conn.cursor()
	c.execute("select ticker_id from stocks_static_data where ticker=\""+ticker+"\"")
	id = c.fetchone()[0]
	c.close()
	conn.close()
	return id

def save_price_to_db(record, dbfile, ticker, date, ticker_id):
	conn = sqlite3.connect(dbfile, 5)
	c = conn.cursor()
	s = pickle.dumps(record)
	c.execute("insert into stocks_data values ("+str(ticker_id)+","+str(date)+",\""+s+"\")")
	conn.commit()
	c.close()
	conn.close()

def save_option_to_db(record, dbfile, ticker_id, date):
	conn = sqlite3.connect(dbfile, 5)
	c = conn.cursor()
	s = pickle.dumps(record)
	c.execute("insert into options_data values ("+str(ticker_id)+","+str(date)+",\""+s+"\")")
	conn.commit()
	c.close()
	conn.close()

def push_data_to_data_srv(record, connection, ticker_id):
	s = pickle.dumps(record)
	connection.push_ticker(ticker_id, s)
	#connection.push_ticker(ticker_id, base64.b64encode(s))

def get_options(dbfile, ticker):
  #look up options for tickers in db
	conn = sqlite3.connect(dbfile, 5)
	c = conn.cursor()
	c.execute("select ticker_id from stocks_static_data where ticker=\""+ticker+"\"")
	for ticker_id in c:
		d = conn.cursor()
		d.execute("select maturity, strike, putcall from options_static_data where ticker_id="+str(ticker_id[0]))
		tickers = list()
		for o in d:
			option_ticker = ticker
			mat = time.gmtime(float(o[0]))
			option_ticker = option_ticker + time.strftime("%y%m%d", mat)
			option_ticker = option_ticker + str(o[2])
			strike = str(int(float(o[1])*1000)).rjust(8,'0')
			option_ticker = option_ticker + strike
			tickers.append(option_ticker)
		d.close()
	c.close()
	conn.close()
	return tickers

def main():
	tickers = ""
	tags = ""
	keys = list()

	c = -1
	c_id = -1
	c_oid = -1

	(tickers, tags, keys, dbfile, do_srv) = parse_command_args()

	data = get_data(tickers, tags)
	if do_srv == True:
		c = core.connection()
	for k, i in data.iteritems():
		try:
			values = data_to_dict(i, keys)
			date = time.time()
			id = get_ticker_id(dbfile, k)
			save_price_to_db(values, dbfile, k, date, id)
			if do_srv == True:
				c_id = c.get_ticker(k)
				push_data_to_data_srv(values, c, c_id)
			print "fetched ticker "+k
			option_tickers = get_options(dbfile, k)
			oi = 0
			option_data = get_data_html2(k)
			save_option_to_db(option_data, dbfile, id, date)
			if do_srv == True:
				co_id = c.get_ticker(k+"_options")
				print str(co_id)
				push_data_to_data_srv(option_data, c, co_id)
			print "fetched "+str(len(option_tickers))+" options for ticker "+k
		except sqlite3.OperationalError as exp:
			print "### SQL Exception caught: "+str(exp)
			sys.stderr.write("SQL Exception caught for ticker "+k+"\n")
			raise

if __name__ == "__main__":
	main()
