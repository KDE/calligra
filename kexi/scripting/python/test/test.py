# This Python script demonstrates the usage of
# the Kross::PythonKexiDB module to access the
# KexiDB functionality from within Python.

#TODO: why the hell the script-execution aborts sometimes with following message;
#KexiDB: WARNING: DriverManagerInternal::lookupDrivers(): problem with detecting 'sqlite' driver's version -- skipping it!
#Alarm clock
#Skipping the 'sqlite' driver folliwng way works as expected, so what's going on there?
#if(srv_name.lower() == "sqlite") { delete ptr; continue; }

# Import the KexiDB module.
import KexiDB

# Let's use a class for testing cause it just
# enables the Python way to do it :)
class Test:
    # Constructor.
    #def __init__(self):

    # Prints just some various informations about the
    # KexiDB module.
    def printInfos(self):
        print "KexiDB = %s %s" % (str(KexiDB), dir(KexiDB))
        #print "KexiDB.__doc__ = %s" % KexiDB.__doc__
        print "KexiDB.driverNames() = %s" % str(KexiDB.driverNames())

    # Returns a KexiDBDriver object.
    def getDriver(self, drivername):
        driver = KexiDB.driver(drivername)
        print "KexiDB.driver = %s %s" % (str(driver), dir(driver))
        return driver

    # Returns a KexiDBConnectionData object.
    def getFileConnectionData(self, name, file):
        connectiondata = KexiDB.getConnectionData()
        connectiondata.connName = name
        connectiondata.fileName = file
        print "KexiDB.getConnectionData = %s" % str(connectiondata)
        return connectiondata

    # Returns a KexiDBConnectionData object.
    def getSocketConnectionData(self, name, host, port, password, username):
        connectiondata = KexiDB.getConnectionData()
        connectiondata.connName = name
        connectiondata.hostName = host
        connectiondata.port = port
        connectiondata.password = password
        connectiondata.userName = username
        print "KexiDB.getConnectionData = %s" % str(connectiondata)
        return connectiondata

    # Returns a KexiDBConnection object.
    def getConnection(self, driver, connectiondata):
        connection = driver.getConnection(connectiondata)
        print "driver.getConnection(connectiondata) = %s %s" % (str(connection), dir(connection))
        return connection

    # Connect with KexiDBConnection object.
    def connect(self, connection):
        if connection.isConnected():
            print "ERROR in connect(): Already connected!"
            return False

        if not connection.connect():
            print "ERROR in connect(): connect failed!"
            return False

        return True

    # Disconnect from KexiDBConnection object.
    def disconnect(self, connection):
        if not connection.isConnected():
            print "ERROR in disconnect(): Not connected!"
            return False

        if not connection.disconnect():
            print "ERROR in disconnect(): disconnect failed!"
            return None

        return True

    # Prints various information about a KexiDBConnection object.
    def printConnectionInfos(self, connection):
        print "connection.databaseNames() = %s" % connection.databaseNames()
        print "connection.currentDatabase() = %s" % connection.currentDatabase()
        print "connection.tableNames() = %s" % connection.tableNames()

    # Execute a SQL-query and print the result.
    def executeSQLQuery(self, connection, dbname, sqlquerystatement):

        if not connection.isDatabase(dbname):
            print "ERROR in executeSQLQuery(): database %s does not exist!" % dbname
            return None

        if not connection.useDatabase(dbname):
            print "ERROR in executeSQLQuery(): useDatabase %s failed!" % dbname
            return None
        #print "connection.currentDatabase() = %s" % connection.currentDatabase()

        cursor = connection.executeQuery(sqlquerystatement)
        if cursor == None:
            print "ERROR in executeSQLQuery(): connection.executeQuery() returned a None!"
            return None
        print "connection.executeQuery() cursor = %s %s" % (str(cursor), dir(cursor))

        cursor.moveFirst()
        while(not cursor.eof()):
            for i in range(cursor.fieldCount()):
                print "Item=%s Field=%s Value=%s" % (cursor.at(), i, cursor.value(i))
            cursor.moveNext()

        return cursor

if __name__ == '__main__':
    t = Test()
    t.printInfos()
    driver = t.getDriver("SQLite3")

    connectiondata = t.getFileConnectionData(
        "mySQLiteConnection", # Connectionname
        "/home/snoopy/New_database.kexi" # File
    )
    #connectiondata = t.getSocketConnectionData(
    #    "mySocketConnection", # Connectionname
    #    "127.0.0.1", # Hostaddress
    #    1234, # Port
    #    "myUsername", # Username
    #    "myPassword" # Password
    #)

    connection = t.getConnection(driver, connectiondata)

    t.connect(connection)

    t.printConnectionInfos(connection)

    cursor = t.executeSQLQuery(
        connection, # KexiDBConnection object.
        "/home/snoopy/New_database.kexi", # databasename
        "SELECT * FROM table1" # sql query statement
    )

    tableschema = KexiDB.getTableSchema("myTableSchema4")
    print "tableschema = %s %s" % (tableschema, dir(tableschema))
    print "tableschema.name = %s" % tableschema.name
    print "tableschema.caption = %s" % tableschema.caption
    print "tableschema.description = %s" % tableschema.description

    #TypeError: 'KexiDBTableSchema' object has only read-only attributes (assign to .name)
    #tableschema.name = "TheName"
    #tableschema.caption = "TheCaption"
    #tableschema.description = "TheDescription"

    ###################################################
    # "Failed loading object's definition."
    ###################################################
    # KexiDB: ******** CREATE TABLE myTableSchema3 (myField1 Text(200))
    # KexiDB: ######## NEW obj_id == 9
    # KexiDB: val0: 9
    # KexiDB: val1: 11
    # KexiDB: val2: 'myField1'
    # KexiDB: val3: 200
    # KexiDB: val4: 0
    # KexiDB: val5: 0
    # KexiDB: val6: 0
    # KexiDB: val7: NULL
    # KexiDB: val8: 0
    # KexiDB: val9: NULL
    # KexiDB: val10: NULL
    ###################################################

    field = KexiDB.getField("myField1")
    #field.name = "myField1"
    field.Text = True
    tableschema.fieldlist.addField(field)

    # WORKS
    #print "connection.createTable() = %s" % connection.createTable(tableschema)

    t.disconnect(connection)

    #TODO access to cursor without having connection.connect() called crashes. We
    #need to add a lot more of checks. Maybe add policies like in boost?
