KEXI_SQL_KEYWORDS=../../kexidb/keywords.cpp
MYSQL_KEYWORDS=../../kexidb/drivers/mySQL/mysqlkeywords.cpp
SQLITE3_KEYWORDS=../../kexidb/drivers/sqlite/sqlitekeywords.cpp
PQXX_KEYWORDS=../../kexidb/drivers/pqxx/pqxxkeywords.cpp

all: build

build:
	./sql_keywords.sh

install: ${KEXI_SQL_KEYWORDS} ${SQLITE3_KEYWORDS} \
         ${MYSQL_KEYWORDS} ${PQXX_KEYWORDS}

${MYSQL_KEYWORDS}: mysqlkeywords.cpp
	cp mysqlkeywords.cpp ${MYSQL_KEYWORDS}


${KEXI_SQL_KEYWORDS}: keywords.cpp
	cp keywords.cpp ${KEXI_SQL_KEYWORDS}


${SQLITE3_KEYWORDS}: sqlitekeywords.cpp
	cp sqlitekeywords.cpp ${SQLITE3_KEYWORDS}

${PQXX_KEYWORDS}: pqxxkeywords.cpp
	cp pqxxkeywords.cpp ${PQXX_KEYWORDS}

clean:
	rm -f *.new *.all *.cpp *~
.PHONY: clean kexi
