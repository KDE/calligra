#!/usr/bin/env python

"""
  This Python script demonstrates the usage of the Kross
  python-interface to access KexiDB functionality from
  within Python.
"""

# Class to test the KexiDB functionality.
class KexiDBClass:

    # Constructor.
    def __init__(self):
        # The KexiDB module spends us access to the KexiDB functionality.
        #import KexiDB
        import krosskexidb
        self.kexidbmodule = krosskexidb

        # Create and remember the drivermanager.
        self.drivermanager = self.kexidbmodule.DriverManager()

    # Print informations about the KexiDB module.
    def printKexiDB(self):
        print "KexiDB = %s %s" % (str(self.kexidbmodule),dir(self.kexidbmodule))
        # Each object has __name__ and __doc__
        print "KexiDB.__name__ = %s" % self.kexidbmodule.__name__
        print "KexiDB.__doc__ = %s" % self.kexidbmodule.__doc__
        # Print some infos about the drivermanager.
        print "drivermanager = %s %s" % str(self.drivermanager,dir(self.drivermanager))
        # The drivermanager holds a list of drivers he supports.
        print "drivermanager.driverNames() = %s" % self.driverNames()
        # Orint some infos about the connectiondatamanager.
        print "connectiondatamanager = %s %s" % str(self.connectiondatamanager,dir(self.connectiondatamanager))

    # Print informations about a driver.
    def printDriverManger(self, driver):
        print "driver = %s %s" % str(driver,dir(driver))
        # Each driver has a version to be able to determinate with what release we are working.
        print "driver.versionMajor() = %s" % driver.versionMajor()
        print "driver.versionMinor() = %s" % driver.versionMinor()
        # Show us what connections are opened right now.
        print "driver.connectionList() = %s" % str(driver.connectionList())

    # Print informations about a connection.
    def printConnection(self, connection):
        print "connection = %s %s" % (str(connection),dir(connection))
        # Print a list of all avaible databasenames this connection has.
        print "connection.databaseNames() = %s" % connection.databaseNames()

    # Return a list of drivernames.
    def driverNames(self):
        return self.drivermanager.driverNames()

    # Return the to drivername matching KexiDBDriver object.
    def driver(self, drivername):
        return self.drivermanager.driver(drivername)

    # Return a new KexiDBConnectionData object.
    def getConnectionData(self):
        return self.drivermanager.createConnectionData()

    # Open a connection to a filebased driver.
    def connectWithFile(self, driver, filename):
        # First we need a new connectiondata object.
        connectiondata = self.getConnectionData()
        # Fill the new connectiondata object with what we need to connect.
        connectiondata.setCaption("myFileConnection")
        connectiondata.setFileName(filename)
        print "connectiondata.serverInfoString = %s" % connectiondata.serverInfoString()
        # Create the connection now.
        connection = driver.createConnection(connectiondata)
        # Establish the connection.
        if not connection.connect():
            raise("ERROR in connectWithDatabase(): Failed to connect!")
        # Open database for usage. The filebased driver uses the filename as databasename.
        self.useDatabase(connection, filename)
        return connection

    # Open database for usage.
    def useDatabase(self, connection, dbname):
        if not connection.useDatabase(dbname):
            raise("ERROR in connectWithDatabase(): Failed to use database!")

    # Create a new database.
    def createDatabase(self, connection, dbname):
        #print "createDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())
        connection.createDatabase(dbname)
        #print "createDatabase databaseExists(%s) = %s" % (dbname,connection.databaseExists(dbname))
        #print "createDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())

    # Drop an existing database.
    def dropDatabase(self, connection, dbname):
        #print "dropDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())
        myfileconnection.dropDatabase(dbname)
        #print "dropDatabase databaseExists(%s) = %s" % (dbname,connection.databaseExists(dbname))
        #print "dropDatabase dbname='%s' dbnames='%s'" % (dbname,connection.databaseNames())

    # Test KexiDBParser used to parse SQL-statements.
    def testParser(self, connection, sqlstatement):
        parser = connection.parser()
        if not parser:
            raise "ERROR in testParser(): Failed to create parser!"
        print "parser.parse = %s" % parser.parse(sqlstatement)
        print "parser.statement = %s" % parser.statement()
        print "parser.operation = %s" % parser.operation()
        print "parser.table = %s" % parser.table()
        print "parser.query = %s" % parser.query()
        print "parser.connection = %s" % parser.connection()

    # Execute the sql query statement and print the single string result.
    def printQuerySingleString(self, connection, sqlstatement):
        query = myfileconnection.querySingleString("SELECT * FROM table1", 0)
        print "querySingleString = %s" % query

    # Execute the sql query statement and print the single stringlist result.
    def printQueryStringList(self, connection, sqlstatement):
        query = myfileconnection.queryStringList("SELECT * FROM table1", 0)
        print "queryStringList = %s" % query

    # Walk through the KexiDBCursor and print all item values.
    def printQueryCursor(self, cursor):
        if cursor == None:
            raise("ERROR: executeQuery failed!")
        #print "printCursor() cursor = %s %s" % (str(cursor), dir(cursor))

        # Go to the first item of the table.
        if not cursor.moveFirst():
            raise("ERROR in printCursor(): cursor.moveFirst() returned False!")

        # Walk through all items in the table.
        while(not cursor.eof()):
            # Print for each item some infos about the fields and there content.
            for i in range( cursor.fieldCount() ):
                print "Item='%s' Field='%s' Value='%s'" % (cursor.at(), i, cursor.value(i))
            # Move to the next item
            cursor.moveNext()

    # Similar to printQueryCursor
    def printQuerySchema(self, connection, queryschema):
        return self.printQueryCursor(connection.executeQuerySchema(queryschema))

    # Similar to printQueryCursor
    def printQueryString(self, connection, sqlstring):
        return self.printQueryCursor(connection.executeQueryString(sqlstring))

    # Add a field to the tableschema.
    def addField(self, tableschema, name):
        field = self.drivermanager.field()
        field.setType("Text")
        field.setName(name)
        tableschema.fieldlist().addField(field)
        print "tableschema.fieldlist().fieldCount() = %s" % tableschema.fieldlist().fieldCount()
        return field

    # Create a table.
    def createTable(self, connection, tablename):
        # First we need a new tableschema.
        tableschema = self.drivermanager.tableSchema(tablename)
        self.addField(tableschema, "myfield")
        print "connection.createTable = %s" % connection.createTable(tableschema, True)
        return tableschema

    # Drop a table.
    def dropTable(self, connection, tablename):
        connection.dropTable(tablename)

    # Alter the name of a table.
    def alterTableName(self, connection, tablename, newtablename):
        tableschema = connection.tableSchema(tablename)
        print "alterTableName from=%s to=%s tableschema=%s" % (tablename, newtablename, tableschema)
        connection.alterTableName(tableschema, newtablename)

class testClass:
    i = 123;
    s = "string-attribute from testClass"
    def __init__(self):
        print "testKexiDBClass Constructor."
    def testClassFunction1(args):
        print "testClassFunction1() called in pythonscript."
        return "testClassFunction1() returnvalue"
    def testClassFunction2(args):
        print "testClassFunction2() called in pythonscript."
        return "testClassFunction2() returnvalue"

def testKexiDB():
    global KexiDBClass
    mykexidbclass = KexiDBClass()
    #mykexidbclass.printKexiDB()

    mydriver = mykexidbclass.driver("SQLite3")
    #mykexidbclass.printDriverManger(mydriver)

    myfileconnection = mykexidbclass.connectWithFile(mydriver, "/home/snoopy/test.kexi")
    #mykexidbclass.printConnection(myfileconnection)
    #mykexidbclass.testParser(myfileconnection, "SELECT * from table1")

    #mykexidbclass.printQuerySingleString(myfileconnection, "SELECT * FROM dept")
    #mykexidbclass.printQueryStringList(myfileconnection, "SELECT * FROM dept")
    mykexidbclass.printQueryString(myfileconnection, "SELECT * FROM dept")

    #myqueryschema = mykexidbclass.drivermanager.querySchema()
    #myqueryschema.setName("myqueryname")
    #myqueryschema.setCaption("myquerycaption")
    #myqueryschema.setStatement("SELECT * FROM table2")
    #print "myqueryschema = %s" % myqueryschema.statement()
    #mykexidbclass.printQuerySchema(myfileconnection, myqueryschema)

    #mykexidbclass.createTable(myfileconnection, "mytable123")
    #mykexidbclass.dropTable(myfileconnection, "mytable123")
    #mykexidbclass.alterTableName(myfileconnection, "table1", "table111")

    #TODO: new table isn't usuable!!!
    #ts1 = myfileconnection.tableSchema("table2")
    #ts2 = mykexidbclass.drivermanager.tableSchema("table4")
    #mykexidbclass.addField(ts2, "MyField 111111111")
    #print "myfileconnection.alterTable = %s" % myfileconnection.alterTable(ts1, ts2)
    #TEST
    #bool Connection::insertRecord(TableSchema &tableSchema, QValueList<QVariant>& values)
    #myfileconnection.insertRecord(KexiDBField, ("field1", "field2"))
    #del(mycursor)
    #del(myfileconnection)
    #del(mydriver)
    #del(mykexidbclass)

def testfunc(msg):
    global globalvar
    globalvar = globalvar + 1
    print "testfunc() returnvalue msg='%s' globalvar='%s'" % (msg,globalvar)
    return "this is the __main__.testfunc() returnvalue!"

def testobjectCallback():
    print "function testobjectCallback() called !"
    return "this is the __main__.testobjectCallback() returnvalue!"

def testobjectCallbackWithParams(argument):
    print "testobjectCallbackWithParams() argument = %s" % str(argument)
    return "this is the __main__.testobjectCallbackWithParams() returnvalue!"

def testQtObject(self):

    # Get the QtObject instance to access the QObject.
    #testobject = get("TestObject")
    testobject = self.get("TestObject")
    if testobject == None:
        raise "Object 'TestObject' undefined !!!"

    print "testobject = %s %s" % (str(testobject),dir(testobject))
    #print "propertyNames = %s" % testobject.propertyNames()
    #print "slotNames = %s" % testobject.slotNames()
    #print "signalNames = %s" % testobject.signalNames()

    # We could just call a slot or a signal.
    print "################################### 1"
    print testobject.call("testSlot2()");
    print testobject.call("testSignal()");
    #print testobject.call() #KrossTest: List::item index=0 is out of bounds. Raising TypeException.

    # Each slot a QObject spends is a object itself.
    print "################################### 2"
    myslot = testobject.get("testSlot()")
    print "myslotevent = %s" % str(myslot)
    print "################################### 3"
    print myslot.call()

    print "################################### 4"
    print "__name__ = %s" % __name__
    print "__dir__ = %s" % dir()
    #print "__builtin__ = %s" % __builtin__
    print "self = %s %s" % (str(self),dir(self))
    #print "TestCase = %s" % str(TestCase)

    print "################################### 5"
    print "self.list = %s" % self.list()
    print "self.dict = %s" % self.dict()
    print "################################### 6"
    testobject = self.get("TestObject")
    print "testobject = %s" % testobject

    print "################################### 7"
    if not testobject.connect("testSignal()",testobject,"testSlot2()"):
        raise "Failed to connect testSignal() with testSlot2() at object 'TestObject'."
    testobject.signal("testSignal()")
    #testobject.testSlot()
    testobject.slot("testSlot()")
    testobject.disconnect("testSignal()")

def testActionEvent(self):

    #action1 = get("Action1")
    action1 = self.get("Action1")
    if action1 == None:
        raise "Object 'Action1' undefined !!!"

    print "action1 = %s %s" % (str(action1),dir(action1))

    print "################################### 1"
    #action1.call()
    action1.activate()

def testPluginModule():
    import krosstestpluginmodule
    print "krosstestpluginmodule => %s %s" % (krosstestpluginmodule,dir(krosstestpluginmodule))

    testobject1 = krosstestpluginmodule.testpluginobject1()
    print "testpluginobject1 => %s %s" % (testobject1,dir(testobject1))

    print testobject1.func()
    print "-------------------------------------"
    print testobject1.overloadedFunc()
    print "-------------------------------------"
    print testobject1.overloadedFunc("somestring")
    print "-------------------------------------"
    print testobject1.overloadedFunc(22)
    print "-------------------------------------"
    print testobject1.overloadedFunc("somestring1","somestring2")
    print "-------------------------------------"
    print testobject1.overloadedFunc("somestring",22)

print "########## BEGIN TEST: testpluginmodule ##########"
testPluginModule()
print "########## BEGIN TEST: testpluginmodule ##########"

print "########## BEGIN TEST: KexiDB ##########"
testKexiDB()
print "########## END TEST: KexiDB ##########"

#print "########## BEGIN TEST: QObject ##########"
#testQtObject(self)
#print "########## END TEST: QObject ##########"

#print "########## BEGIN TEST: ActionEvent ##########"
#testActionEvent(self)
#print "########## END TEST: ActionEvent ##########"

#testfunc("from __main__")
#maintestfunc()
#print __name__
