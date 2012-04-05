
##TODO revision?
find_library(LIBKCAL_LIBRARY libkcal.so)
if(LIBKCAL_LIBRARY)
    set(HAVE_LIBKCAL 1)
endif(LIBKCAL_LIBRARY)
macro_log_feature(HAVE_LIBKCAL "KDE PIM iCalendar" "KDE Personal Information Management iCalendar Libraries" "http://www.kde.org/" FALSE "" "Required by Plan ICalendar export filter (available as a module in KDE)")


if(HAVE_LIBKCAL)
    add_subdirectory( export )
    #add_subdirectory( import )
endif(HAVE_LIBKCAL)


