
#include "kexi_global.h"

//! displays information that feature "feature_name" is not availabe in the current application version
#define KEXI_UNFINISHED(feature_name) KMessageBox::sorry(0, i18n("\"%1\" function is not available for version %2 of %3 application.").arg(feature_name).arg(KEXI_VERSION).arg(KEXI_APP_NAME))

