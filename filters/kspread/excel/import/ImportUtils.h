#ifndef IMPORTUTILS_H
#define IMPORTUTILS_H

#include <QString>
#include "../sidewinder/value.h"

namespace XlsUtils {

/// Remove via the "\" char escaped characters from the string.
QString removeEscaped(const QString &text, bool removeOnlyEscapeChar = false);
/// extract and return locale and remove locale from time string.
QString extractLocale(QString &time);

bool isPercentageFormat(const QString& valueFormat);
bool isTimeFormat(const Swinder::Value &value, const QString& valueFormat);
bool isFractionFormat(const QString& valueFormat);
bool isDateFormat(const Swinder::Value &value, const QString& valueFormat);

}

#endif // IMPORTUTILS_H
