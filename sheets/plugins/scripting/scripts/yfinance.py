#!/usr/bin/env kross

"""
Sheets python script that provides the Yahoo! Finance formula function.

Yahoo! Terms of Use
The feeds are provided free of charge for use by individuals and non-profit
organizations for personal, non-commercial uses. We ask that you provide
attribution to Yahoo! Weather in connection with your use of the feeds.
If you provide this attribution in text, please use: "Yahoo! Weather." If
you provide this attribution with a graphic, please use the Yahoo! Weather
logo that we have included in the feed itself.
We reserve all rights in and to the Yahoo! Weather logo, and your right
to use the Yahoo! Weather logo is limited to providing attribution in
connection with these RSS feeds. 
Yahoo! also reserves the right to require you to cease distributing these
feeds at any time for any reason.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
This script is licensed under the BSD license.
"""

import re, urllib
import Kross, KSpread

T = Kross.module("kdetranslation")

class Yfinance:
    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()

        func = KSpread.function("YFINANCE")
        func.minparam = 3
        func.maxparam = 3
        func.comment = (
            T.i18n("The YFINANCE() function uses the Yahoo! Finance Web Service to display stock values of a defined ticker symbol.")
        )
        func.syntax = T.i18n("YFINANCE(string,string,string)")
        func.addParameter("String", T.i18n("The ticker symbol."))
        func.addParameter("String", T.i18n("The date."))
        func.addParameter("String", T.i18n("The type: Date, Open, High, Low, Close, Volume or AdjClose."))
        func.addExample(T.i18n("YFINANCE(\"YHOO\";\"20060119\";\"Open\")"))
        func.addExample(T.i18n("YFINANCE(\"=C14\";\"=C15\";\"=C16\")"))

        def update(argument):
            print "Yfinance.update !"
            ticker = argument[0] #e.g. "yhoo" or "goog"
            if ticker.startswith('='):
                ticker = KSpread.currentSheet().text(ticker[1:])

            todate = argument[1] #e.g. "20060119"
            if todate.startswith('='):
                todate = KSpread.currentSheet().text(todate[1:])
            fromdate=todate

            typename = argument[2] #e.g. "Open"
            if typename.startswith('='):
                typename = KSpread.currentSheet().text(typename[1:])
            typename = typename.lower()

            if not re.compile('^[a-zA-Z0-9]+$').match(ticker):
                func.error = T.i18n("Invalid symbol")
                return

            if len(todate) != 8 or not re.compile('^[0-9]+$').match(todate):
                func.error = T.i18n("Invalid date")
                return

            typenr = None
            if typename == "date": typenr = 0
            elif typename == "open": typenr = 1
            elif typename == "high": typenr = 2
            elif typename == "low": typenr = 3
            elif typename == "close": typenr = 4
            elif typename == "volume": typenr = 5
            elif typename == "adjclose": typenr = 6
            else:
                func.error = T.i18n("Invalid type")
                return

            quote = dict()
            quote['s'] = ticker
            quote['d'] = str(int(todate[4:6]) - 1)
            quote['e'] = str(int(todate[6:8]))
            quote['f'] = str(int(todate[0:4]))
            quote['g'] = "d"
            quote['a'] = str(int(fromdate[4:6]) - 1)
            quote['b'] = str(int(fromdate[6:8]))
            quote['c'] = str(int(fromdate[0:4]))
            params = urllib.urlencode(quote)
            params += "&ignore=.csv"
            url = "http://ichart.yahoo.com/table.csv?%s" % params
            try:
                f = urllib.urlopen(url)
            except:
                func.error = T.i18n("Web services request failed")
                return
            result = f.read().split("\n")
            resultlist = []
            rx = re.compile('^[0-9]+')
            for i in range(0,len(result)):
                if rx.match(result[i]):
                    resultlist = result[i].split(',')
                    break

            if len(resultlist) < 1:
                func.error = T.i18n("No stock")
                return
            if len(resultlist) < 7:
                func.error = T.i18n("Invalid stock")
                return

            v = resultlist[typenr]
            print "Y! Finance: %s %s" % (v,resultlist)
            func.result = v

        func.connect("called(QVariantList)", update)
        func.registerFunction()

print "Started Y! Finance"
Yfinance( self )
print "Finished Y! Finance"
