kde4_header()

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KDE33MIMEDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KDE351MIMEDIR)")



########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#if need_kde33_compatibility
#  KDE33MIMEDIR = kde33
#endif
#
#if need_kde351_compatibility
#  KDE351MIMEDIR = kde351
#endif
#
#SUBDIRS = $(KDE33MIMEDIR) $(KDE351MIMEDIR)
