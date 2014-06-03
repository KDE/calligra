#!/usr/bin/env kross

"""
Sheets python script that provides the Yahoo! Weather formula function.

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
from xml.dom import minidom
import Kross, KSpread

T = Kross.module("kdetranslation")

class Yweather:
    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()

        func = KSpread.function("YWEATHER")
        func.minparam = 1
        func.maxparam = 2
        func.comment = ( T.i18n("The YWEATHER() function uses the Yahoo! Weather Web Service to display the weather of a location.")
        )
        func.syntax = T.i18n("YWEATHER(string;string)")
        func.addParameter("String", T.i18n("The US zip code, Location ID or cell that contains them."))
        func.addParameter("String", T.i18n("Units for temperature. f=Fahrenheit and c=Celsius"))
        func.addExample(T.i18n("YWEATHER(\"=A1\")"))
        func.addExample(T.i18n("YWEATHER(\"GMXX0151\";\"c\")"))

        def update(argument):
            print "Yweather.update !"
            location = argument[0] #e.g. "GMXX0151"
            if location.startswith('='):
                sheet = KSpread.currentSheet()
                location = sheet.text(location[1:])

            if location == None or not re.compile('^[a-zA-Z0-9]+$').match(location):
                func.error = T.i18n("Invalid location")
                return

            url = "http://weather.yahooapis.com/forecastrss?p=%s" % location
            if len(argument) >= 2:
                url += "&u=%s" % urllib.quote_plus(argument[1])

            print "url=%s" % url
            namespace = 'http://xml.weather.yahoo.com/ns/rss/1.0'
            dom = None
            try:
                dom = minidom.parse(urllib.urlopen(url))
            except:
                func.error = T.i18n("Web services request failed")
                return

            forecasts = []
            for node in dom.getElementsByTagNameNS(namespace, 'forecast'):
                forecasts.append({
                    'date': node.getAttribute('date'),
                    'low': node.getAttribute('low'),
                    'high': node.getAttribute('high'),
                    'condition': node.getAttribute('text')
                })

            try:
                ycondition = dom.getElementsByTagNameNS(namespace, 'condition')[0]
            except IndexError:
                func.error = T.i18n("Invalid condition")
                return
            #my_current_condition = ycondition.getAttribute('text')
            #my_current_temp = ycondition.getAttribute('temp')
            #my_forecasts = forecasts
            #my_title = dom.getElementsByTagName('title')[0].firstChild.data

            temp = ycondition.getAttribute('temp')
            print "Y! Weather Temperature: %s" % temp
            func.result = temp

        func.connect("called(QVariantList)", update)
        func.registerFunction()

print "Started Y! Weather"
Yweather( self )
print "Finished Y! Weather"
