####### General stuff

INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) -version-info 1:0:0

####### Files

lib_LTLIBRARIES = libktl.la
 
libktl_la_SOURCES = ktlCondition.cc ktlExceptions.cc ktlFastMutex.cc \
                    ktlMutex.cc ktlMutexGuard.cc ktlRecursiveMutex.cc \
                    ktlThread.cc ktlThreadManager.cc

include_HEADERS = ktlCondition.h ktlExceptions.h ktlFastMutex.h \
                  ktlMutex.h ktlMutexGuard.h ktlRecursiveMutex.h \
                  ktlThread.h ktlThreadManager.h

METASOURCES = ktlThread.moc