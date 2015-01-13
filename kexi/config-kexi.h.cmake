#include <config-calligradb.h>

/*! Compile a mobile version of Kexi. */
#cmakedefine KEXI_MOBILE

/* define if you have libreadline available */
/* TODO: detect #define HAVE_READLINE 1 */

#cmakedefine HAVE_UNAME 1

/*! Debugging GUI for Kexi. */
#cmakedefine KEXI_DEBUG_GUI
#if defined KEXI_DEBUG_GUI && !defined CALLIGRADB_DEBUG_GUI
# error KEXI_DEBUG_GUI requires CALLIGRADB_DEBUG_GUI to be set too (using cmake).
#endif

/*! Build Kexi scripting GUI plugin. */
#cmakedefine KEXI_SCRIPTS_SUPPORT

/*! Show unfinished features in Kexi. Thus is useful for testing but may confuse end-user. */
#cmakedefine KEXI_SHOW_UNFINISHED

/* -- Experimental -- */
/*! Enable scripting in Kexi. */
#cmakedefine KEXI_SCRIPTS_SUPPORT

/*! Enable support for project templates in Kexi. */
#cmakedefine KEXI_PROJECT_TEMPLATES

/*! Autorise the main tabbed toolbar in Kexi */
#cmakedefine KEXI_AUTORISE_TABBED_TOOLBAR
