/* Copied from sqlite/sqlitekeywords.h */
#include <xbasedriver.h>

namespace KexiDB {
  const char* xBaseDriver::keywords[] = {
		"ABORT",
		"ATTACH",
		"CLUSTER",
		"CONFLICT",
		"DEFERRED",
		"DEFERRABLE",
		"DETACH",
		"EACH",
		"EXCEPT",
		"FAIL",
		"GLOB",
		"IMMEDIATE",
		"INITIALLY",
		"INSTEAD",
		"INTERSECT",
		"ISNULL",
		"NOTNULL",
		"OF",
		"PRAGMA",
		"RAISE",
		"STATEMENT",
		"TEMP",
		"TRIGGER",
		"VACUUM",
		"VIEW",
		0
  };
}
