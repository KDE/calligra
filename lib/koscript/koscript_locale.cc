#include "koscript_locale.h"

KSLocale::KSLocale() :
	KLocale("koffice")
{
    setDecimalSymbol(".");
    setThousandsSeparator("");
    setMonetaryDecimalSymbol(".");
    setMonetaryThousandsSeparator("");
    setPositiveSign("");
    setNegativeSign("-");
    setWeekStartsMonday(false);
    setTimeFormat("%I:%M:%S %p");
    setDateFormat("%A %d %B %Y");
    setDateFormatShort("%m/%d/%y");
}
