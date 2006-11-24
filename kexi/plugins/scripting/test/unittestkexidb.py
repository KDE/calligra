#!/usr/bin/env kross

"""
  This Python script implements a unittest for the KexiDB
  functionality using the Kross scripting framework.
"""

#TODO following code segfaults (please note the Qt3 vs Qt4 confusion in the backtrace)
import Kross
kexidb = Kross.module("kexidb")

#Program received signal SIGSEGV, Segmentation fault.
#0xb54d0f54 in QMutexPool::get () from /usr/lib/libqt-mt.so.3
#(gdb) bt
##0  0xb54d0f54 in QMutexPool::get () from /usr/lib/libqt-mt.so.3
##1  0xb51b8358 in QMetaObjectCleanUp::QMetaObjectCleanUp () from /usr/lib/libqt-mt.so.3
##2  0xb60c308a in KParts::BrowserHostExtension::~BrowserHostExtension () from /usr/lib/libkparts.so.2
##3  0xb60e00c5 in QValueListPrivate<KParts::Plugin::PluginInfo>::~QValueListPrivate () from /usr/lib/libkparts.so.2
##4  0xb60bfe71 in _init () from /usr/lib/libkparts.so.2
##5  0xb7fd37f5 in _dl_rtld_di_serinfo () from /lib/ld-linux.so.2
##6  0xb7fd3921 in _dl_rtld_di_serinfo () from /lib/ld-linux.so.2
##7  0xb6a31eb7 in _dl_open () from /lib/tls/libc.so.6
##8  0xb7fd344f in _dl_rtld_di_serinfo () from /lib/ld-linux.so.2
##9  0xb6a3154f in _dl_open () from /lib/tls/libc.so.6
##10 0xb689bd8e in dlopen () from /lib/tls/libdl.so.2
##11 0xb7fd344f in _dl_rtld_di_serinfo () from /lib/ld-linux.so.2
##12 0xb689c42d in dlerror () from /lib/tls/libdl.so.2
##13 0xb689bd21 in dlopen () from /lib/tls/libdl.so.2
##14 0xb7f52482 in QLibraryPrivate::load_sys (this=0x813e118) at plugin/qlibrary_unix.cpp:177
##15 0xb7f4e42a in QLibraryPrivate::load (this=0xb60c22c9) at plugin/qlibrary.cpp:418
##16 0xb7f4e46e in QLibrary::load (this=0xb60c22c9) at plugin/qlibrary.cpp:641
##17 0xb7351132 in KLibLoader::library (this=0x80c0bb0, _name=0x80f1fb8 "krossmodulekexidb", hint=@0xbfafe46c)
    #at /home/kde4/kdelibs/kdelibs/kdecore/klibloader.cpp:467
##18 0xb734f444 in KLibLoader::globalLibrary (this=0x80c0bb0, _name=0x80f1fb8 "krossmodulekexidb")
    #at /home/kde4/kdelibs/kdelibs/kdecore/klibloader.cpp:422
##19 0xb71f4585 in Kross::Manager::module (this=0x80be830, modulename=@0x811911c)
    #at /home/kde4/koffice/libs/kross/core/manager.cpp:259

CurrentPath = self.currentPath()

import unittest

class TestKexiDB(unittest.TestCase):

	def setUp(self):
		import Kross
		self.kexidb = Kross.module("kexidb")

	def testGeneral(self):
		self.assert_( self.kexidb != None )
		self.assert_( self.kexidb.version() >= 1 )

	def testKexiFile(self):
		import os
		global CurrentPath
		file = os.path.join(CurrentPath,'testcase.kexi')
		self.assert_( os.path.isfile(file) )

		connectiondata = self.kexidb.createConnectionDataByFile(file)
		self.assert_( connectiondata != None )

suite = unittest.makeSuite(TestKexiDB)
unittest.TextTestRunner(verbosity=2).run(suite)

#class KexiDBClass:
    ## Constructor.
    #def __init__(self):
        ## The KexiDB module spends us access to the KexiDB functionality.
        ##import KexiDB
        #import krosskexidb
        #self.kexidbmodule = krosskexidb
        #print "KrossKexiDB version=%s" % self.kexidbmodule.version()

        ## Create and remember the drivermanager.
        #self.drivermanager = self.kexidbmodule.DriverManager()

    ## Print informations about the KexiDB module.
    #def printKexiDB(self):
        #print "KexiDB = %s %s" % (str(self.kexidbmodule),dir(self.kexidbmodule))
        ## Each object has __name__ and __doc__
        ##print "KexiDB.__name__ = %s" % self.kexidbmodule.__name__
        ##print "KexiDB.__doc__ = %s" % self.kexidbmodule.__doc__
        ## Print some infos about the drivermanager.
        #print "drivermanager = %s %s" % (self.drivermanager,dir(self.drivermanager))
        ## The drivermanager holds a list of drivers he supports.
        #print "drivermanager.driverNames() = %s" % self.driverNames()

    ## Print informations about a driver.
    #def printDriverManger(self, driver):
        #print "driver = %s %s" % (driver,dir(driver))
        ## Each driver has a version to be able to determinate with what release we are working.
        #print "driver.versionMajor() = %s" % driver.versionMajor()
        #print "driver.versionMinor() = %s" % driver.versionMinor()
        ## Show us what connections are opened right now.
        #print "driver.connectionsList() = %s" % str(driver.connectionsList())

    ## Print informations about a connection.
    #def printConnection(self, connection):
        #print "connection = %s %s" % (str(connection),dir(connection))
        ## Print a list of all avaible databasenames this connection has.
        #print "connection.databaseNames() = %s" % connection.databaseNames()

    ## Return a list of drivernames.
    #def driverNames(self):
        #return self.drivermanager.driverNames()

    ## Return the to drivername matching KexiDBDriver object.
    #def driver(self, drivername):
        #return self.drivermanager.driver(drivername)

    ## Return a new KexiDBConnectionData object.
    #def getConnectionData(self):
        #return self.drivermanager.createConnectionData()

    ## Open a connection to a filebased driver.
    #def connectWithFile(self, driver, filename):
        ## First we need a new connectiondata object.
        #connectiondata = self.getConnectionData()
        ## Fill the new connectiondata object with what we need to connect.
        #connectiondata.setCaption("myFileConnection")
        #connectiondata.setFileName(filename)
        #print "connectiondata.serverInfoString = %s" % connectiondata.serverInfoString()
        ## Create the connection now.
        #connection = driver.createConnection(connectiondata)
        ## Establish the connection.
        #if not connection.connect():
            #raise("ERROR in connectWithDatabase(): Failed to connect!")
        ## Open database for usage. The filebased driver uses the filename as databasename.
        #self.useDatabase(connection, filename)
        #return connection

    ## Open database for usage.
    #def useDatabase(self, connection, dbname):
        #if not connection.useDatabase(dbname):
            #raise("ERROR in connectWithDatabase(): Failed to use database!")

    ## Create a new database.
    #def createDatabase(self, connection, dbname):
        ##print "createDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())
        #connection.createDatabase(dbname)
        ##print "createDatabase databaseExists(%s) = %s" % (dbname,connection.databaseExists(dbname))
        ##print "createDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())

    ## Drop an existing database.
    #def dropDatabase(self, connection, dbname):
        ##print "dropDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())
        #myfileconnection.dropDatabase(dbname)
        ##print "dropDatabase databaseExists(%s) = %s" % (dbname,connection.databaseExists(dbname))
        ##print "dropDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())

    ## Test KexiDBParser used to parse SQL-statements.
    #def testParser(self, connection, sqlstatement):
        #parser = connection.parser()
        #if not parser:
            #raise "ERROR in testParser(): Failed to create parser!"
        #print "parser.parse = %s" % parser.parse(sqlstatement)
        #print "parser.statement = %s" % parser.statement()
        #print "parser.operation = %s" % parser.operation()
        #print "parser.table = %s" % parser.table()
        #print "parser.query = %s" % parser.query()
        #print "parser.connection = %s" % parser.connection()

    ## Execute the sql query statement and print the single string result.
    #def printQuerySingleString(self, connection, sqlstatement):
        #query = myfileconnection.querySingleString("SELECT * FROM table1", 0)
        #print "querySingleString = %s" % query

    ## Execute the sql query statement and print the single stringlist result.
    #def printQueryStringList(self, connection, sqlstatement):
        #query = myfileconnection.queryStringList("SELECT * FROM table1", 0)
        #print "queryStringList = %s" % query

    ## Walk through the KexiDBCursor and print all item values.
    #def printQueryCursor(self, cursor):
        #if cursor == None:
            #raise("ERROR: executeQuery failed!")
        ##print "printCursor() cursor = %s %s" % (str(cursor), dir(cursor))

        ## Go to the first item of the table.
        #if not cursor.moveFirst():
            #raise("ERROR in printCursor(): cursor.moveFirst() returned False!")

        ## Walk through all items in the table.
        #while(not cursor.eof()):
            ## Print for each item some infos about the fields and there content.
            #for i in range( cursor.fieldCount() ):
                #print "Item='%s' Field='%s' Value='%s'" % (cursor.at(), i, cursor.value(i))
            ## Move to the next item
            #cursor.moveNext()

    ## Similar to printQueryCursor
    #def printQuerySchema(self, connection, queryschema):
        #return self.printQueryCursor(connection.executeQuerySchema(queryschema))

    ## Similar to printQueryCursor
    #def printQueryString(self, connection, sqlstring):
        #return self.printQueryCursor(connection.executeQueryString(sqlstring))

    ## Add a field to the tableschema.
    #def addField(self, tableschema, name):
        #field = self.drivermanager.field()
        #field.setType("Text")
        #field.setName(name)
        #tableschema.fieldlist().addField(field)
        #print "tableschema.fieldlist().fieldCount() = %s" % tableschema.fieldlist().fieldCount()
        #return field

    ## Create a table.
    #def createTable(self, connection, tablename):
        ## First we need a new tableschema.
        #tableschema = self.drivermanager.tableSchema(tablename)
        #self.addField(tableschema, "myfield")
        #print "connection.createTable = %s" % connection.createTable(tableschema, True)
        #return tableschema

    ## Drop a table.
    #def dropTable(self, connection, tablename):
        #connection.dropTable(tablename)

    ## Alter the name of a table.
    #def alterTableName(self, connection, tablename, newtablename):
        #tableschema = connection.tableSchema(tablename)
        #print "alterTableName from=%s to=%s tableschema=%s" % (tablename, newtablename, tableschema)
        #connection.alterTableName(tableschema, newtablename)

#def testKexiDB():
    #global KexiDBClass
    #mykexidbclass = KexiDBClass()
    #mykexidbclass.printKexiDB()

    #mydriver = mykexidbclass.driver("SQLite3")
    #mykexidbclass.printDriverManger(mydriver)

    #myfileconnection = mykexidbclass.connectWithFile(mydriver, "/home/snoopy/test.kexi")
    #mykexidbclass.printConnection(myfileconnection)
    ##mykexidbclass.testParser(myfileconnection, "SELECT * from table1")

    ##mykexidbclass.printQuerySingleString(myfileconnection, "SELECT * FROM dept")
    ##mykexidbclass.printQueryStringList(myfileconnection, "SELECT * FROM dept")
    #mykexidbclass.printQueryString(myfileconnection, "SELECT * FROM dept")

    ##myqueryschema = mykexidbclass.drivermanager.querySchema()
    ##myqueryschema.setName("myqueryname")
    ##myqueryschema.setCaption("myquerycaption")
    ##myqueryschema.setStatement("SELECT * FROM table2")
    ##print "myqueryschema = %s" % myqueryschema.statement()
    ##mykexidbclass.printQuerySchema(myfileconnection, myqueryschema)

    ##mykexidbclass.createTable(myfileconnection, "mytable123")
    ##mykexidbclass.dropTable(myfileconnection, "mytable123")
    ##mykexidbclass.alterTableName(myfileconnection, "table1", "table111")

    ##TODO: new table isn't usuable!!!
    ##ts1 = myfileconnection.tableSchema("table2")
    ##ts2 = mykexidbclass.drivermanager.tableSchema("table4")
    ##mykexidbclass.addField(ts2, "MyField 111111111")
    ##print "myfileconnection.alterTable = %s" % myfileconnection.alterTable(ts1, ts2)
    ##TEST
    ##bool Connection::insertRecord(TableSchema &tableSchema, QValueList<QVariant>& values)
    ##myfileconnection.insertRecord(KexiDBField, ("field1", "field2"))
    ##del(mycursor)
    ##del(myfileconnection)
    ##del(mydriver)
    ##del(mykexidbclass)

#print "########## BEGIN TEST: KexiDB ##########"
#testKexiDB()
#print "########## END TEST: KexiDB ##########"
