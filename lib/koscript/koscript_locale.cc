#include "koscript_locale.h"

KSLocale::KSLocale()
{
    _decimalSymbol = ".";
    _thousandsSeparator = "";
    _monetaryDecimalSymbol = ".";
    _monetaryThousandsSeparator = "";
    _positiveSign = "";
    _negativeSign = "-";
    m_weekStartsMonday = FALSE;
    _timefmt = "%I:%M:%S %p";
    _datefmt = "%A %d %B %Y";
    _datefmtshort = "%m/%d/%y";
}
