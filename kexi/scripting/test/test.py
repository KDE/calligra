# This Python script demonstrates the usage of
# the Kross python-interface to access KexiDB
# functionality from within Python.

#import sys

# Import the KexiDB module.
#from Kross import KexiDB
import KexiDB

# Let's use a class for testing cause it just
# enables the Python way to do it :)
class KexiDBClass:

    # Constructor.
    def __init__(self):
        # Create and remember some global needed classes.
        self.drivermanager = KexiDB.get("DriverManager")
        self.connectiondatamanager = KexiDB.get("ConnectionDataManager")

    # Print informations about the KexiDB module.
    def printKexiDB(self):
        print "KexiDB = %s %s" % (str(KexiDB),dir(KexiDB))
        # Each object has __name__ and __doc__
        print "KexiDB.__name__ = %s" % KexiDB.__name__
        print "KexiDB.__doc__ = %s" % KexiDB.__doc__
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

    # Open a connection to a filebased driver.
    def connectWithFile(self, driver, filename):
        # First we need a new connectiondata object.
        connectiondata = self.connectiondatamanager.createConnectionData()
        # Fill the new connectiondata object with what we need to connect.
        connectiondata.setConnName("myFileConnection")
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

    # Execute the sql query statement and iterate through all returned
    # cursor-elements and print there values.
    def printQueryCursor(self, connection, sqlstatement):
        # Execute the SQL query statement.
        cursor = connection.executeQuery(sqlstatement)
        if cursor == None:
            raise("ERROR: executeQuery failed!")
        print "printCursor() cursor = %s %s" % (str(cursor), dir(cursor))

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

if __name__ == '__main__':

    print "BEGIN KROSS::KEXIDB TEST ###############################################"

    mykexidbclass = KexiDBClass()
    #mykexidbclass.printKexiDB()

    mydriver = mykexidbclass.driver("SQLite3")
    #mykexidbclass.printDriverManger(mydriver)

    myfileconnection = mykexidbclass.connectWithFile(mydriver, "/home/snoopy/New_database.kexi")
    #mykexidbclass.printConnection(myfileconnection)
    #mykexidbclass.testParser(myfileconnection, "SELECT * from table1")

    mykexidbclass.printQuerySingleString(myfileconnection, "SELECT * FROM table1")
    mykexidbclass.printQueryStringList(myfileconnection, "SELECT * FROM table1")
    mykexidbclass.printQueryCursor(myfileconnection, "SELECT * FROM table1")

    #myfileconnection.createTable(KexiDBTableSchema)
    #myfileconnection.dropTable(KexiDBTableSchema)
    #myfileconnection.alterTable(KexiDBTableSchema, NewKexiDBTableSchema)
    #myfileconnection.alterTableName(KexiDBTableSchema, "MyNewTableName")
    #myfileconnection.tableSchema()
    #myfileconnection.isEmptyTable()
    #myfileconnection.dropDatabase("mydatabase")

    #del(mycursor)
    #del(myfileconnection)
    #del(mydriver)
    #del(mykexidbclass)

    print "END KROSS::KEXIDB TEST ###############################################"

