# This Python script demonstrates the usage of
# the Kross::PythonKexiDB module to access the
# KexiDB functionality from within Python.

#TODO: why the hell the script-execution aborts sometimes with following message;
#KexiDB: WARNING: DriverManagerInternal::lookupDrivers(): problem with detecting 'sqlite' driver's version -- skipping it!
#Alarm clock

# Import the KexiDB module.
import KexiDB

# Let's use a class for testing cause it just
# enables the Python way to do it :) For sure
# it's not needed to have a class to work with
# KexiDB.
class Test:
    # Constructor.
    #def __init__(self):

    # Prints just some various infos about the
    # KexiDB module.
    def printInfos(self):
        print "KexiDB = %s %s" % (str(KexiDB), dir(KexiDB))
        #print "KexiDB.__doc__ = %s" % KexiDB.__doc__
        print "KexiDB.driverNames() = %s" % str(KexiDB.driverNames())

    # Returns a KexiDBDriver object.
    def getDriver(self, drivername):
        print "111"
        driver = KexiDB.driver(drivername)
        print "222"
        print "KexiDB.driver = %s %s" % (str(driver), dir(driver))
        return driver

    # Returns a KexiDBConnectionData object.
    def getConnectionData(self, name, file):
        connectiondata = KexiDB.createConnectionData()
        connectiondata.connName = name
        #connectiondata.hostName = "127.0.0.1"
        #connectiondata.port = 1234
        #connectiondata.password = "myPassword"
        #connectiondata.userName = "myUsername"
        connectiondata.fileName = file
        print "KexiDB.createConnectionData = %s" % str(connectiondata)
        return connectiondata

    # Returns a KexiDBConnection object.
    def getConnection(self, driver, connectiondata):
        connection = driver.createConnection(connectiondata)
        print "driver.createConnection(connectiondata) = %s %s" % (str(connection), dir(connection))
        return connection

    # Prints various information about a KexiDBConnection object.
    def printConnectionInfos(self, connection):
        if not connection.connect():
            print "ERROR in printConnectionInfos(): connection.connect() returned with FALSE !!!"
            return
        print "connection.databaseNames() = %s" % connection.databaseNames()
        print "connection.currentDatabase() = %s" % connection.currentDatabase()
        print "connection.tableNames() = %s" % connection.tableNames()
        if not connection.disconnect():
            print "ERROR in printConnectionInfos(): connection.disconnect() returned with FALSE !!!"

    # Execute a SQL-statement.
    def executeSQL(self, connection, dbname, sqlstatement):
        if not connection.connect():
            print "ERROR in executeSQL(): connection.connect() returned with FALSE !!!"
            return
        if not connection.isDatabase(dbname):
            print "ERROR in executeSQL(): database %s does not exist !!!" % dbname
            return

        print "connection.useDatabase() = %s" % connection.useDatabase(dbname)
        print "connection.currentDatabase() = %s" % connection.currentDatabase()

        cursor = connection.executeQuery(sqlstatement)
        print "connection.executeQuery() cursor = %s %s" % (str(cursor), dir(cursor))

        cursor.moveFirst()
        while(not cursor.eof()):
            for i in range(cursor.fieldCount()):
                print "Item=%s Field=%s Value=%s" % (cursor.at(), i, cursor.value(i))
            cursor.moveNext()

        if not connection.disconnect():
            print "ERROR in executeSQL(): connection.disconnect() returned with FALSE !!!"

        return cursor

if __name__ == '__main__':
    t = Test()
    t.printInfos()
    driver = t.getDriver("SQLite3")
    connectiondata = t.getConnectionData("myConnection", "/home/snoopy/New_database.kexi")
    connection = t.getConnection(driver, connectiondata)
    #t.printConnectionInfos(connection)
    cursor = t.executeSQL(connection, "/home/snoopy/New_database.kexi", "SELECT * FROM table1")

    #TODO access to cursor without having connection.connect() called crashes!!!
