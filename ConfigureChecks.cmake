include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckPrototypeExists)
include(CheckTypeSize)
include(MacroBoolTo01)
# The FindKDE4.cmake module sets _KDE4_PLATFORM_DEFINITIONS with
# definitions like _GNU_SOURCE that are needed on each platform.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})

#check for libz using the cmake supplied FindZLIB.cmake
macro_bool_to_01(OPENEXR_FOUND HAVE_OPENEXR)
macro_bool_to_01(PYTHON_INCLUDE_PATH HAVE_PYTHON)
if(BZIP2_FOUND)
   set(HAVE_BZIP2_SUPPORT 1)
   if(BZIP2_NEED_PREFIX)
      set(NEED_BZ2_PREFIX 1)
   endif(BZIP2_NEED_PREFIX)
else(BZIP2_FOUND)
   set(HAVE_BZIP2_SUPPORT 0)
endif(BZIP2_FOUND)

#now check for dlfcn.h using the cmake supplied CHECK_include_FILE() macro
# If definitions like -D_GNU_SOURCE are needed for these checks they
# should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
# defined outside this file.  Here we include these definitions in
# CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
# checks below.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
if (WIN32)
   set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES} )
   set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES} )
endif (WIN32)

check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(ieeefp.h HAVE_IEEEFP_H)
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(sys/dir.h HAVE_SYS_NDIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_function_exists(initgroups HAVE_INITGROUPS)
check_function_exists(setenv     HAVE_SETENV)
check_function_exists(unsetenv   HAVE_UNSETENV)

check_prototype_exists(initgroups "unistd.h;sys/types.h;unistd.h;grp.h" HAVE_INITGROUPS_PROTO)
check_prototype_exists(setenv stdlib.h HAVE_SETENV_PROTO)
check_prototype_exists(unsetenv stdlib.h HAVE_UNSETENV_PROTO)

check_type_size("int" SIZEOF_INT)
check_type_size("char *"  SIZEOF_CHAR_P)
check_type_size("long" SIZEOF_LONG)
check_type_size("short" SIZEOF_SHORT)

