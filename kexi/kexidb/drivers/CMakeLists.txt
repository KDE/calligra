
# drivers will be moved out of this dir in Predicate, and then area will be redefined
#remove_definitions(-DKDE_DEFAULT_DEBUG_AREA)
#add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44001)

if (MYSQL_FOUND)
   add_subdirectory(mysql)
endif ()

if (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
   add_subdirectory(pqxx)
endif ()

if (FREETDS_FOUND)
   add_subdirectory(sybase)
endif ()

if(XBASE_FOUND)
   add_subdirectory(xbase)
endif()
