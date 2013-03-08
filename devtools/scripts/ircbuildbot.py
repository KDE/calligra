#!/usr/bin/python -Qwarnall
# -*- coding: utf-8 -*-

# This is a simple irc bot that reports progress to the Calligra irc channel

import time, lxml.etree, urllib2, re, sys, socket, string

HOST='irc.freenode.org' #The server we want to connect to
PORT=6667 #The connection port which is usually 6667
NICK='buildbot_py' #The bot's nickname
IDENT='buildbot_py'
REALNAME='James Spawned'
OWNER='vandenoever' #The bot owner's nick
CHANNELINIT='#Calligra' #The default channel for the bot
readbuffer='' #Here we store all the messages from server 

feed = "http://158.36.191.251:8080/guestAuth/feed.html?buildTypeId=bt6&itemsType=builds&buildStatus=failed&userKey=guest"

s = socket.socket( ) #Create the socket
s.connect((HOST, PORT)) #Connect to server
s.send('NICK '+NICK+'\n') #Send the nick to server
s.send('USER '+IDENT+' '+HOST+' bla :'+REALNAME+'\n') #Identify to server

def getMessage():
        try:
                parser = lxml.etree.XMLParser(dtd_validation=False, load_dtd=False, resolve_entities=False, no_network=False, recover=False)
                tree = lxml.etree.parse(urllib2.urlopen(feed))
                ns = {'a':'http://www.w3.org/2005/Atom'}
                link = tree.xpath("/a:feed/a:entry[1]/a:link/@href", namespaces=ns)[0]
                title = tree.xpath("/a:feed/a:entry[1]/a:title/text()", namespaces=ns)[0]
                summary = tree.xpath("/a:feed/a:entry[1]/a:summary/text()", namespaces=ns)[0]
                s = re.search('strong>([^<]*)<', summary).group(1)
                newmessage = title + " " + s + " " + link
                try:
                        who = re.search('by\s+(\S*)', summary).group(1)
                        newmessage = who + ": " + newmessage
                except:
                        pass
        except:
                newmessage = "Error in reading RSS"
        return newmessage

joined = False
message = ""
lastchecktime = time.time() - 55
while 1:
        line = s.recv(500) #recieve server messages
        print line.rstrip() #server message is output
        if not joined:
                s.send('JOIN ' + CHANNELINIT + '\n') #Join a channel
                s.send("PRIVMSG " + CHANNELINIT + " :Spawned, James Spawned\n")
                joined = True
        if line[:4] == "PING":
                s.send("PONG\n")
        if time.time() - lastchecktime > 60:
                newmessage = getMessage()
                if newmessage != message:
                        message = newmessage
                        s.send("PRIVMSG " + CHANNELINIT + " :" + message + "\n")
                lastchecktime = time.time()
