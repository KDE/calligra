#include <config-calligradb.h>

/* build Kexi scripting GUI plugin */
/* #undef KEXI_SCRIPTS_SUPPORT */

/* define if you have libreadline available */
/* TODO: detect #define HAVE_READLINE 1 */

#cmakedefine HAVE_UNAME 1

#cmakedefine KEXI_DEBUG_GUI
#if defined KEXI_DEBUG_GUI && !defined CALLIGRADB_DEBUG_GUI
# error KEXI_DEBUG_GUI requires CALLIGRADB_DEBUG_GUI to be set too (using cmake).
#endif
