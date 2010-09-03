#include "ImportUtils.h"

#include <NumberFormatParser.h>

namespace XlsUtils {

QString removeEscaped(const QString &text, bool removeOnlyEscapeChar)
{
    QString s(text);
    int pos = 0;
    while (true) {
        pos = s.indexOf('\\', pos);
        if (pos < 0)
            break;
        if (removeOnlyEscapeChar) {
            s = s.left(pos) + s.mid(pos + 1);
            pos++;
        } else {
            s = s.left(pos) + s.mid(pos + 2);
        }
    }
    return s;
}

QString extractLocale(QString &time)
{
    QString locale;
    if (time.startsWith("[$-")) {
        int pos = time.indexOf(']');
        if (pos > 3) {
            locale = time.mid(3, pos - 3);
            time = time.mid(pos + 1);
            pos = time.lastIndexOf(';');
            if (pos >= 0) {
                time = time.left(pos);
            }
        }
    }
    return locale;
}

bool isPercentageFormat(const QString& valueFormat)
{
    if (valueFormat.isEmpty()) return false;
    if (valueFormat.length() < 1) return false;
    return valueFormat[valueFormat.length()-1] == QChar('%');
}

bool isTimeFormat(const Swinder::Value &value, const QString& valueFormat)
{
    if (value.type() != Swinder::Value::Float)
        return false;

    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);
    vf = removeEscaped(vf);

    // if there is still a time formatting picture item that was not escaped
    // and therefore removed above, then we have a time format here.
    QRegExp ex("(h|H|m|s)");
    return (ex.indexIn(vf) >= 0) && value.asFloat() < 1.0;
}

bool isFractionFormat(const QString& valueFormat)
{
    QRegExp ex("^#[?]+/[0-9?]+$");
    QString vf = removeEscaped(valueFormat);
    return ex.indexIn(vf) >= 0;
}

bool isDateFormat(const Swinder::Value &value, const QString& valueFormat)
{
    if (value.type() != Swinder::Value::Float)
        return false;

    return NumberFormatParser::isDateFormat(valueFormat);
}

}
