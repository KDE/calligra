# Python script for testing Kross

class myClass:
    def __init__(self, whatever):
        print "MyClass.__init__ self=%s whatever=%s" % (str(self), str(whatever))
    def __getattr__(self, name):
        print "MyClass.__getattr__ self=%s name=%s" % (str(self), str(name))
        return "MyClass.__getattr__ RESULT"
    def __setattr__(self, name, value):
        print "MyClass.__setattr__ self=%s name=%s value=%s" % (str(self), str(name), str(value))
        return "MyClass.__setattr__ RESULT"

import KexiDB
#from qt import *

def myfunction():
    print "This is myfunction..."
    return "This is the myfunction returnvalue..."

print "KexiDB = %s %s" % ( str(KexiDB), dir(KexiDB) )
print "KexiDB.__doc__ = %s " % KexiDB.__doc__
#print "KexiDB.value1 = %s" % KexiDB.value1

#driverlist = KexiDB.getDriverList()
#print "KexiDB.getDriverList() = %s" % driverlist

driver = KexiDB.getDriver("SQLite3")
if not driver or driver == None:
    print "driver IS Py_None !!!!!!!!!!!!!!!"
print "KexiDB.getDriver() = %s %s" % (str(driver), dir(driver))

#connectionlist1 = driver.getConnectionList()
#print "driver.getConnectionList() 1 = %s" % str(connectionlist1)

connectiondata = {
    "connName" : "myConnection",
    #"hostName" : "localhost",
    #"port" : 0,
    #"userName" : "",
    #"password" : "mypwd",
    "fileName" : "/home/snoopy/New_database.kexi"
}
connection = driver.createConnection(connectiondata)
print "driver.createConnection(name) = %s %s" % (str(connection), dir(connection))
print "connection.connect() = %s" % connection.connect()

dbnames = connection.getDatabaseNames()
print "connection.getDatabaseNames() = %s" % dbnames

print "connection.getTableNames() 1 = %s" % connection.getTableNames()
print "connection.getCurrentDatabase() 1 = %s" % connection.getCurrentDatabase()

dbname = "/home/snoopy/New_database.kexi" #"New_database.kexi"
print "connection.isDatabase() = %s" % connection.isDatabase(dbname)

print "connection.useDatabase() 1= %s" % connection.useDatabase(dbname)
print "connection.getTableNames() 2 = %s" % connection.getTableNames()
print "connection.getCurrentDatabase() 2 = %s" % connection.getCurrentDatabase()

#connectionlist2 = driver.getConnectionList()
#print "driver.getConnectionList() 2 = %s" % str(connectionlist2)

cursor = connection.executeQuery("SELECT * FROM table1")
print "cursor = %s" % cursor

fieldcount = cursor.fieldCount()
cursor.moveFirst()
while(not cursor.eof()):
    print "cursor ITEM"
    for i in range(fieldcount):
        print "  %s %s" % (str(i), cursor.value(i))
    cursor.moveNext()

#print "testmethodfunction2() = %s" % KexiDB.testmethodfunction2("param2")
#print "myfunction() = %s" % myfunction()
#f = myClass()
#print "class = %s %s" % (str(f),dir(f))
#print str( myClass.myfunc1(f,"hallo1") )
#print str( myClass.myfunc2(f,"hallo2") )

