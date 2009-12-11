/* This file is part of the KDE project
   Copyright 2000-2006 The KSpread Team <koffice-devel@kde.org>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Locale
#include "Currency.h"

#include <klocale.h>

using namespace KSpread;

namespace Currency_LNS
{
typedef struct {
    char const * code;
    char const * country;
    char const * name;
    char const * display;
} Money;

// codes and names as defined in ISO 3166-1
// first  column: saved code
// second column: country name (localized)
// third column:  currency name (localized)
// fourth column: displayed currency code (localized but maybe only in
//                the country language it belongs to)
// WARNING: change the "29" in chooseString if you change this array
static const Money lMoney[] = {
    { "", "", "", ""}, // auto
    { "", "", "", ""}, // extension (codes imported)
    { "$", "", "Dollar", "$" }, // unspecified
    { "$", I18N_NOOP("Australia"), I18N_NOOP("Dollar"), "$" },
    { "$", I18N_NOOP("Canada"), I18N_NOOP("Dollar"), "$" },
    { "$", I18N_NOOP("Caribbean"), I18N_NOOP("Dollar"), "$" },
    { "$", I18N_NOOP("New Zealand"), I18N_NOOP("Dollar"), "$" },
    { "$", I18N_NOOP("United States"), I18N_NOOP("Dollar"), "$" },

    // € == Euro sign in utf8
    { "€", "", "€", "€" }, // unspecified
    { "€", I18N_NOOP("Austria"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Belgium"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Cyprus"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Finland"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("France"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Germany"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Greece"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Ireland"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Italy"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Luxembourg"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Malta"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Monaco"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Netherlands"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Portugal"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("San Marino"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Slovakia"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Slovenia"), I18N_NOOP("Euro"), "€" },
    { "€", I18N_NOOP("Spain"), I18N_NOOP("Euro"), "€" },

    { "£", I18N_NOOP("United Kingdom"), I18N_NOOP("Pound"), "£" },

    { "¥", I18N_NOOP("Japan"), I18N_NOOP("Yen"), "¥" },

    { "AFA", I18N_NOOP("Afghanistan"), I18N_NOOP("Afghani (old)"), I18N_NOOP("AFA") },
    { "AFN", I18N_NOOP("Afghanistan"), I18N_NOOP("Afghani"), I18N_NOOP("AFN") },
    { "ALL", I18N_NOOP("Albania"), I18N_NOOP("Lek"), I18N_NOOP("Lek") },
    { "DZD", I18N_NOOP("Algeria"), I18N_NOOP("Algerian Dinar"), I18N_NOOP("DZD") },
    { "USD", I18N_NOOP("American Samoa"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "EUR", I18N_NOOP("Andorra"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "ADP", I18N_NOOP("Andorra"), I18N_NOOP("Andorran Peseta"), I18N_NOOP("ADP") },
    { "AOA", I18N_NOOP("Angola"), I18N_NOOP("Kwanza"), I18N_NOOP("AOA") },
    { "XCD", I18N_NOOP("Anguilla"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "XCD", I18N_NOOP("Antigua And Barbuda"), I18N_NOOP("East Carribean Dollar"), I18N_NOOP("XCD") },
    { "ARS", I18N_NOOP("Argentina"), I18N_NOOP("Argentine Peso"), I18N_NOOP("ARS") },
    { "AMD", I18N_NOOP("Armenia"), I18N_NOOP("Armenian Dram"), I18N_NOOP("AMD") },
    { "AWG", I18N_NOOP("Aruba"), I18N_NOOP("Aruban Florin"), I18N_NOOP("AWG") },
    { "AUD", I18N_NOOP("Australia"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "EUR", I18N_NOOP("Austria"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "ATS", I18N_NOOP("Austria"), I18N_NOOP("Schilling"), I18N_NOOP("S") },
    { "AZM", I18N_NOOP("Azerbaijan"), I18N_NOOP("Azerbaijanian Manat"), I18N_NOOP("AZM") },
    { "BSD", I18N_NOOP("Bahamas"), I18N_NOOP("Bahamian Dollar"), I18N_NOOP("BSD") },
    { "BHD", I18N_NOOP("Bahrain"), I18N_NOOP("Bahraini Dinar"), I18N_NOOP("BHD") },
    { "BDT", I18N_NOOP("Bangladesh"), I18N_NOOP("Taka"), I18N_NOOP("BDT") },
    { "BBD", I18N_NOOP("Barbados"), I18N_NOOP("Barbados Dollar"), I18N_NOOP("BBD") },
    { "BYR", I18N_NOOP("Belarus"), I18N_NOOP("Belarusian Ruble"), I18N_NOOP("p.") },
    { "EUR", I18N_NOOP("Belgium"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "BEF", I18N_NOOP("Belgium"), I18N_NOOP("Franc"), I18N_NOOP("BF") },
    { "BZD", I18N_NOOP("Belize"), I18N_NOOP("Belize Dollar"), I18N_NOOP("BZ$") },
    { "XOF", I18N_NOOP("Benin"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "BMD", I18N_NOOP("Bermuda"), I18N_NOOP("Bermudian Dollar"), I18N_NOOP("BMD") },
    { "INR", I18N_NOOP("Bhutan"), I18N_NOOP("Indian Rupee"), I18N_NOOP("INR") },
    { "BTN", I18N_NOOP("Bhutan"), I18N_NOOP("Ngultrum"), I18N_NOOP("BTN") },
    { "BOB", I18N_NOOP("Bolivia"), I18N_NOOP("Boliviano"), I18N_NOOP("Bs") },
    { "BOV", I18N_NOOP("Bolivia"), I18N_NOOP("Mvdol"), I18N_NOOP("BOV") },
    { "BAM", I18N_NOOP("Bosnia And Herzegovina"), I18N_NOOP("Convertible Marks"), I18N_NOOP("BAM") },
    { "BWP", I18N_NOOP("Botswana"), I18N_NOOP("Pula"), I18N_NOOP("BWP") },
    { "NOK", I18N_NOOP("Bouvet Island"), I18N_NOOP("Norwegian Krone"), I18N_NOOP("NOK") },
    { "BRL", I18N_NOOP("Brazil"), I18N_NOOP("Brazilian Real"), I18N_NOOP("R$") },
    { "USD", I18N_NOOP("British Indian Ocean Territory"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "BND", I18N_NOOP("Brunei Darussalam"), I18N_NOOP("Brunei Dollar"), I18N_NOOP("BND") },
    { "BGL", I18N_NOOP("Bulgaria"), I18N_NOOP("Lev"), I18N_NOOP("BGL") },
    { "BGN", I18N_NOOP("Bulgaria"), I18N_NOOP("Bulgarian Lev"), I18N_NOOP("BGN") },
    { "XOF", I18N_NOOP("Burkina Faso"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "BIF", I18N_NOOP("Burundi"), I18N_NOOP("Burundi Franc"), I18N_NOOP("BIF") },
    { "KHR", I18N_NOOP("Cambodia"), I18N_NOOP("Riel"), I18N_NOOP("KHR") },
    { "XAF", I18N_NOOP("Cameroon"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "CAD", I18N_NOOP("Canada"), I18N_NOOP("Canadian Dollar"), I18N_NOOP("CAD") },
    { "CVE", I18N_NOOP("Cape Verde"), I18N_NOOP("Cape Verde Escudo"), I18N_NOOP("CVE") },
    { "KYD", I18N_NOOP("Cayman Islands"), I18N_NOOP("Cayman Islands Dollar"), I18N_NOOP("KYD") },
    { "XAF", I18N_NOOP("Central African Republic"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "XAF", I18N_NOOP("Chad"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "CLP", I18N_NOOP("Chile"), I18N_NOOP("Chilean Peso"), I18N_NOOP("Ch$") },
    { "CLF", I18N_NOOP("Chile"), I18N_NOOP("Unidades de fomento"), I18N_NOOP("CLF") },
    { "CNY", I18N_NOOP("China"), I18N_NOOP("Yuan Renminbi"), I18N_NOOP("CNY") },
    { "AUD", I18N_NOOP("Christmas Island"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "AUD", I18N_NOOP("Cocos (Keeling) Islands"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "COP", I18N_NOOP("Colombia"), I18N_NOOP("Colombian Peso"), I18N_NOOP("C$") },
    { "KMF", I18N_NOOP("Comoros"), I18N_NOOP("Comoro Franc"), I18N_NOOP("KMF") },
    { "XAF", I18N_NOOP("Congo"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "CDF", I18N_NOOP("Congo, The Democratic Republic Of"), I18N_NOOP("Franc Congolais"), I18N_NOOP("CDF") },
    { "NZD", I18N_NOOP("Cook Islands"), I18N_NOOP("New Zealand Dollar"), I18N_NOOP("NZD") },
    { "CRC", I18N_NOOP("Costa Rica"), I18N_NOOP("Costa Rican Colon"), I18N_NOOP("C") },
    { "XOF", I18N_NOOP("Cote D'Ivoire"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "HRK", I18N_NOOP("Croatia"), I18N_NOOP("Croatian kuna"), I18N_NOOP("kn") },
    { "CUP", I18N_NOOP("Cuba"), I18N_NOOP("Cuban Peso"), I18N_NOOP("CUP") },
    { "EUR", I18N_NOOP("Cyprus"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "CYP", I18N_NOOP("Cyprus"), I18N_NOOP("Cyprus Pound"), I18N_NOOP("CYP") },
    { "CZK", I18N_NOOP("Czech Republic"), I18N_NOOP("Czech Koruna"), I18N_NOOP("Kc") },
    { "DKK", I18N_NOOP("Denmark"), I18N_NOOP("Danish Krone"), I18N_NOOP("kr") },
    { "DJF", I18N_NOOP("Djibouti"), I18N_NOOP("Djibouti Franc"), I18N_NOOP("DJF") },
    { "XCD", I18N_NOOP("Dominica"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("RD$") },
    { "DOP", I18N_NOOP("Dominican Republic"), I18N_NOOP("Dominican Peso"), I18N_NOOP("DOP") },
    { "TPE", I18N_NOOP("East Timor"), I18N_NOOP("Timor Escudo"), I18N_NOOP("TPE") },
    { "USD", I18N_NOOP("East Timor"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "USD", I18N_NOOP("Ecuador"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "EGP", I18N_NOOP("Egypt"), I18N_NOOP("Egyptian Pound"), I18N_NOOP("EGP") },
    { "SVC", I18N_NOOP("El Salvador"), I18N_NOOP("El Salvador Colon"), I18N_NOOP("C") },
    { "USD", I18N_NOOP("El Salvador"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "XAF", I18N_NOOP("Equatorial Guinea"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "ERN", I18N_NOOP("Eritrea"), I18N_NOOP("Nakfa"), I18N_NOOP("ERN") },
    { "EEK", I18N_NOOP("Estonia"), I18N_NOOP("Kroon"), I18N_NOOP("kr") },
    { "ETB", I18N_NOOP("Ethiopia"), I18N_NOOP("Ethiopian Birr"), I18N_NOOP("ETB") },
    { "FKP", I18N_NOOP("Falkland Island (Malvinas)"), I18N_NOOP("Falkland Islands Pound"), I18N_NOOP("FKP") },
    { "DKK", I18N_NOOP("Faeroe Islands"), I18N_NOOP("Danish Krone"), I18N_NOOP("kr") },
    { "FJD", I18N_NOOP("Fiji"), I18N_NOOP("Fiji Dollar"), I18N_NOOP("FJD") },
    { "EUR", I18N_NOOP("Finland"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "FIM", I18N_NOOP("Finland"), I18N_NOOP("Markka"), I18N_NOOP("mk") },
    { "EUR", I18N_NOOP("France"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "FRF", I18N_NOOP("France"), I18N_NOOP("Franc"), I18N_NOOP("F") },
    { "EUR", I18N_NOOP("French Guiana"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "XPF", I18N_NOOP("French Polynesia"), I18N_NOOP("CFP Franc"), I18N_NOOP("XPF") },
    { "EUR", I18N_NOOP("Franc Southern Territories"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "XAF", I18N_NOOP("Gabon"), I18N_NOOP("CFA Franc BEAC"), I18N_NOOP("XAF") },
    { "GMD", I18N_NOOP("Gambia"), I18N_NOOP("Dalasi"), I18N_NOOP("GMD") },
    { "GEL", I18N_NOOP("Georgia"), I18N_NOOP("Lari"), I18N_NOOP("GEL") },
    { "EUR", I18N_NOOP("Germany"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "DEM", I18N_NOOP("Germany"), I18N_NOOP("German Mark"), I18N_NOOP("DM") },
    { "GHC", I18N_NOOP("Ghana"), I18N_NOOP("Cedi"), I18N_NOOP("GHC") },
    { "GIP", I18N_NOOP("Gibraltar"), I18N_NOOP("Gibraltar Pound"), I18N_NOOP("GIP") },
    { "EUR", I18N_NOOP("Greece"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "GRD", I18N_NOOP("Greece"), I18N_NOOP("Drachma"), I18N_NOOP("GRD") },
    { "DKK", I18N_NOOP("Greenland"), I18N_NOOP("Danish Krone"), I18N_NOOP("DKK") },
    { "XCD", I18N_NOOP("Grenada"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "EUR", I18N_NOOP("Guadeloupe"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "USD", I18N_NOOP("Guam"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "GTQ", I18N_NOOP("Guatemala"), I18N_NOOP("Quetzal"), I18N_NOOP("Q") },
    { "GNF", I18N_NOOP("Guinea"), I18N_NOOP("Guinea Franc"), I18N_NOOP("GNF") },
    { "GWP", I18N_NOOP("Guinea-Bissau"), I18N_NOOP("Guinea-Bissau Peso"), I18N_NOOP("GWP") },
    { "XOF", I18N_NOOP("Guinea-Bissau"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "GYD", I18N_NOOP("Guyana"), I18N_NOOP("Guyana Dollar"), I18N_NOOP("GYD") },
    { "HTG", I18N_NOOP("Haiti"), I18N_NOOP("Gourde"), I18N_NOOP("HTG") },
    { "USD", I18N_NOOP("Haiti"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "AUD", I18N_NOOP("Heard Island And McDonald Islands"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "EUR", I18N_NOOP("Holy See (Vatican City State)"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "HNL", I18N_NOOP("Honduras"), I18N_NOOP("Lempira"), I18N_NOOP("L") },
    { "HKD", I18N_NOOP("Hong Kong"), I18N_NOOP("Hong Kong Dollar"), I18N_NOOP("HKD") },
    { "HUF", I18N_NOOP("Hungary"), I18N_NOOP("Forint"), I18N_NOOP("Ft") },
    { "ISK", I18N_NOOP("Iceland"), I18N_NOOP("Iceland Krona"), I18N_NOOP("kr.") },
    { "INR", I18N_NOOP("India"), I18N_NOOP("Indian Rupee"), I18N_NOOP("INR") },
    { "IDR", I18N_NOOP("Indonesia"), I18N_NOOP("Rupiah"), I18N_NOOP("Rp") },
    { "IRR", I18N_NOOP("Iran, Islamic Republic Of"), I18N_NOOP("Iranian Rial"), I18N_NOOP("IRR") },
    { "IQD", I18N_NOOP("Iraq"), I18N_NOOP("Iraqi Dinar"), I18N_NOOP("IQD") },
    { "EUR", I18N_NOOP("Ireland"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "IEP", I18N_NOOP("Ireland"), I18N_NOOP("Punt"), I18N_NOOP("IR----") },
    { "IEX", I18N_NOOP("Ireland"), I18N_NOOP("Pence"), I18N_NOOP("IEX") },
    { "ILS", I18N_NOOP("Israel"), I18N_NOOP("New Israeli Sheqel"), I18N_NOOP("ILS") },
    { "EUR", I18N_NOOP("Italy"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "ITL", I18N_NOOP("Italy"), I18N_NOOP("Lira"), I18N_NOOP("L.") },
    { "JMD", I18N_NOOP("Jamaica"), I18N_NOOP("Jamaican Dollar"), I18N_NOOP("J$") },
    { "JPY", I18N_NOOP("Japan"), I18N_NOOP("Yen"), I18N_NOOP("JPY") },
    { "JOD", I18N_NOOP("Jordan"), I18N_NOOP("Jordanian Dinar"), I18N_NOOP("JOD") },
    { "KZT", I18N_NOOP("Kazakhstan"), I18N_NOOP("Tenge"), I18N_NOOP("KZT") },
    { "KES", I18N_NOOP("Kenya"), I18N_NOOP("Kenyan Shilling"), I18N_NOOP("KES") },
    { "AUD", I18N_NOOP("Kiribati"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "KPW", I18N_NOOP("Korea, Democratic People's Republic Of"), I18N_NOOP("North Korean Won"), I18N_NOOP("KPW") },
    { "KRW", I18N_NOOP("Korea, Republic Of"), I18N_NOOP("Won"), I18N_NOOP("KRW") },
    { "KWD", I18N_NOOP("Kuwait"), I18N_NOOP("Kuwaiti Dinar"), I18N_NOOP("KWD") },
    { "KGS", I18N_NOOP("Kyrgyzstan"), I18N_NOOP("Som"), I18N_NOOP("KGS") },
    { "LAK", I18N_NOOP("Lao People's Democratic Republic"), I18N_NOOP("Kip"), I18N_NOOP("LAK") },
    { "LVL", I18N_NOOP("Latvia"), I18N_NOOP("Latvian Lats"), I18N_NOOP("Ls") },
    { "LBP", I18N_NOOP("Lebanon"), I18N_NOOP("Lebanese Pound"), I18N_NOOP("LBP") },
    { "ZAR", I18N_NOOP("Lesotho"), I18N_NOOP("Rand"), I18N_NOOP("ZAR") },
    { "LSL", I18N_NOOP("Lesotho"), I18N_NOOP("Loti"), I18N_NOOP("LSL") },
    { "LRD", I18N_NOOP("Liberia"), I18N_NOOP("Liberian Dollar"), I18N_NOOP("LRD") },
    { "LYD", I18N_NOOP("Libyan Arab Jamahiriya"), I18N_NOOP("Lybian Dinar"), I18N_NOOP("LYD") },
    { "CHF", I18N_NOOP("Liechtenstein"), I18N_NOOP("Swiss Franc"), I18N_NOOP("CHF") },
    { "LTL", I18N_NOOP("Lithuania"), I18N_NOOP("Lithuanian Litus"), I18N_NOOP("Lt") },
    { "EUR", I18N_NOOP("Luxembourg"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "LUF", I18N_NOOP("Luxembourg"), I18N_NOOP("Franc"), I18N_NOOP("F") },
    { "MOP", I18N_NOOP("Macao"), I18N_NOOP("Pataca"), I18N_NOOP("MOP") },
    { "MKD", I18N_NOOP("Macedonia, The Former Yugoslav Republic Of"), I18N_NOOP("Denar"), I18N_NOOP("MKD") },
    { "MGF", I18N_NOOP("Madagascar"), I18N_NOOP("Malagasy Franc"), I18N_NOOP("MGF") },
    { "MWK", I18N_NOOP("Malawi"), I18N_NOOP("Kwacha"), I18N_NOOP("MWK") },
    { "MYR", I18N_NOOP("Malaysia"), I18N_NOOP("Malaysian Ringgit"), I18N_NOOP("MYR") },
    { "MVR", I18N_NOOP("Maldives"), I18N_NOOP("Rufiyaa"), I18N_NOOP("MVR") },
    { "XOF", I18N_NOOP("Mali"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "EUR", I18N_NOOP("Malta"), I18N_NOOP("EURO"), I18N_NOOP("EUR") },
    { "MTL", I18N_NOOP("Malta"), I18N_NOOP("Maltese Lira"), I18N_NOOP("MTL") },
    { "USD", I18N_NOOP("Marshall Islands"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "EUR", I18N_NOOP("Martinique"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "MRO", I18N_NOOP("Mauritania"), I18N_NOOP("Ouguiya"), I18N_NOOP("MRO") },
    { "MUR", I18N_NOOP("Mauritius"), I18N_NOOP("Mauritius Rupee"), I18N_NOOP("MUR") },
    { "EUR", I18N_NOOP("Mayotte"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "MXN", I18N_NOOP("Mexico"), I18N_NOOP("Mexican Peso"), I18N_NOOP("MXN") },
    { "MXV", I18N_NOOP("Mexico"), I18N_NOOP("Mexican Unidad de Inversion (UDI)"), I18N_NOOP("MXV") },
    { "USD", I18N_NOOP("Micronesia, Federated States Of"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "MDL", I18N_NOOP("Moldova, Republic Of"), I18N_NOOP("Moldovan Leu"), I18N_NOOP("MDL") },
    { "EUR", I18N_NOOP("Monaco"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "MNT", I18N_NOOP("Mongolia"), I18N_NOOP("Tugrik"), I18N_NOOP("MNT") },
    { "XCD", I18N_NOOP("Montserrat"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "MAD", I18N_NOOP("Morocco"), I18N_NOOP("Moroccan Dirham"), I18N_NOOP("MAD") },
    { "MZM", I18N_NOOP("Mozambique"), I18N_NOOP("Metical"), I18N_NOOP("MZM") },
    { "MMK", I18N_NOOP("Myanmar"), I18N_NOOP("Kyat"), I18N_NOOP("MMK") },
    { "ZAR", I18N_NOOP("Namibia"), I18N_NOOP("Rand"), I18N_NOOP("ZAR") },
    { "NAD", I18N_NOOP("Namibia"), I18N_NOOP("Namibia Dollar"), I18N_NOOP("NAD") },
    { "AUD", I18N_NOOP("Nauru"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "NPR", I18N_NOOP("Nepal"), I18N_NOOP("Nepalese Rupee"), I18N_NOOP("NPR") },
    { "EUR", I18N_NOOP("Netherlands"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "NLG", I18N_NOOP("Netherlands"), I18N_NOOP("Guilder"), I18N_NOOP("fl") },
    { "ANG", I18N_NOOP("Netherlands Antilles"), I18N_NOOP("Netherlands Antillean Guilder"), I18N_NOOP("ANG") },
    { "XPF", I18N_NOOP("New Caledonia"), I18N_NOOP("CFP Franc"), I18N_NOOP("XPF") },
    { "NZD", I18N_NOOP("New Zealand"), I18N_NOOP("New Zealand Dollar"), I18N_NOOP("NZD") },
    { "NIO", I18N_NOOP("Nicaragua"), I18N_NOOP("Cordoba Oro"), I18N_NOOP("NIO") },
    { "XOF", I18N_NOOP("Niger"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "NGN", I18N_NOOP("Nigeria"), I18N_NOOP("Naira"), I18N_NOOP("NGN") },
    { "NZD", I18N_NOOP("Niue"), I18N_NOOP("New Zealand Dollar"), I18N_NOOP("NZD") },
    { "AUD", I18N_NOOP("Norfolk Islands"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "USD", I18N_NOOP("Northern Mariana Islands"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "NOK", I18N_NOOP("Norway"), I18N_NOOP("Norwegian Krone"), I18N_NOOP("kr") },
    { "OMR", I18N_NOOP("Oman"), I18N_NOOP("Rial Omani"), I18N_NOOP("OMR") },
    { "PKR", I18N_NOOP("Pakistan"), I18N_NOOP("Pakistan Rupee"), I18N_NOOP("PKR") },
    { "USD", I18N_NOOP("Palau"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "PAB", I18N_NOOP("Panama"), I18N_NOOP("Balboa"), I18N_NOOP("PAB") },
    { "USD", I18N_NOOP("Panama"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "PGK", I18N_NOOP("Papua New Guinea"), I18N_NOOP("Kina"), I18N_NOOP("PGK") },
    { "PYG", I18N_NOOP("Paraguay"), I18N_NOOP("Guarani"), I18N_NOOP("G") },
    { "PEN", I18N_NOOP("Peru"), I18N_NOOP("Nuevo Sol"), I18N_NOOP("PEN") },
    { "PHP", I18N_NOOP("Philippines"), I18N_NOOP("Philippine Peso"), I18N_NOOP("PHP") },
    { "NZD", I18N_NOOP("Pitcairn"), I18N_NOOP("New Zealand Dollar"), I18N_NOOP("NZD") },
    { "PLN", I18N_NOOP("Poland"), I18N_NOOP("Zloty"), I18N_NOOP("zt") },
    { "EUR", I18N_NOOP("Portugal"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "PTE", I18N_NOOP("Portugal"), I18N_NOOP("Escudo"), I18N_NOOP("Esc.") },
    { "USD", I18N_NOOP("Puerto Rico"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "QAR", I18N_NOOP("Qatar"), I18N_NOOP("Qatari Rial"), I18N_NOOP("QAR") },
    { "ROL", I18N_NOOP("Romania"), I18N_NOOP("Leu"), I18N_NOOP("LEI") },
    { "RUR", I18N_NOOP("Russian Federation"), I18N_NOOP("Russian Ruble"), I18N_NOOP("RUR") },
    { "RUB", I18N_NOOP("Russian Federation"), I18N_NOOP("Russian Ruble"), I18N_NOOP("RUB") },
    { "RWF", I18N_NOOP("Rwanda"), I18N_NOOP("Rwanda Franc"), I18N_NOOP("RWF") },
    { "SHP", I18N_NOOP("Saint Helena"), I18N_NOOP("Saint Helena Pound"), I18N_NOOP("SHP") },
    { "XCD", I18N_NOOP("Saint Kitts And Nevis"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "XCD", I18N_NOOP("Saint Lucia"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "EUR", I18N_NOOP("Saint Pierre And Miquelon"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "XCD", I18N_NOOP("Saint Vincent And The Grenadines"), I18N_NOOP("East Caribbean Dollar"), I18N_NOOP("XCD") },
    { "WST", I18N_NOOP("Samoa"), I18N_NOOP("Tala"), I18N_NOOP("WST") },
    { "EUR", I18N_NOOP("San Marino"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "STD", I18N_NOOP("Sao Tome And Principe"), I18N_NOOP("Dobra"), I18N_NOOP("STD") },
    { "SAR", I18N_NOOP("Saudi Arabia"), I18N_NOOP("Saudi Riyal"), I18N_NOOP("SAR") },
    { "XOF", I18N_NOOP("Senegal"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "SCR", I18N_NOOP("Seychelles"), I18N_NOOP("Seychelles Rupee"), I18N_NOOP("SCR") },
    { "SLL", I18N_NOOP("Sierra Leone"), I18N_NOOP("Leone"), I18N_NOOP("SLL") },
    { "SGD", I18N_NOOP("Singapore"), I18N_NOOP("Singapore Dollar"), I18N_NOOP("SGD") },
    { "EUR", I18N_NOOP("Slovakia"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "SKK", I18N_NOOP("Slovakia"), I18N_NOOP("Slovak Koruna"), I18N_NOOP("Sk") },
    { "EUR", I18N_NOOP("Slovenia"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "SIT", I18N_NOOP("Slovenia"), I18N_NOOP("Tolar"), I18N_NOOP("SIT") },
    { "SBD", I18N_NOOP("Solomon Islands"), I18N_NOOP("Solomon Islands Dollar"), I18N_NOOP("SBD") },
    { "SOS", I18N_NOOP("Somalia"), I18N_NOOP("Somali Shilling"), I18N_NOOP("SOS") },
    { "ZAR", I18N_NOOP("South Africa"), I18N_NOOP("Rand"), I18N_NOOP("R") },
    { "EUR", I18N_NOOP("Spain"), I18N_NOOP("Euro"), I18N_NOOP("EUR") },
    { "ESP", I18N_NOOP("Spain"), I18N_NOOP("Peseta"), I18N_NOOP("Pts") },
    { "LKR", I18N_NOOP("Sri Lanka"), I18N_NOOP("Sri Lanka Rupee"), I18N_NOOP("LKR") },
    { "SDD", I18N_NOOP("Sudan"), I18N_NOOP("Sudanese Dinar"), I18N_NOOP("SDD") },
    { "SRG", I18N_NOOP("Suriname"), I18N_NOOP("Suriname Guilder"), I18N_NOOP("SRG") },
    { "NOK", I18N_NOOP("Svalbard And Jan Mayen"), I18N_NOOP("Norwegian Krone"), I18N_NOOP("NOK") },
    { "SZL", I18N_NOOP("Swaziland"), I18N_NOOP("Lilangeni"), I18N_NOOP("SZL") },
    { "SEK", I18N_NOOP("Sweden"), I18N_NOOP("Swedish Krona"), I18N_NOOP("kr") },
    { "CHF", I18N_NOOP("Switzerland"), I18N_NOOP("Swiss Franc"), I18N_NOOP("SFr.") },
    { "SYP", I18N_NOOP("Syrian Arab Republic"), I18N_NOOP("Syrian Pound"), I18N_NOOP("SYP") },
    { "TWD", I18N_NOOP("Taiwan, Province Of China"), I18N_NOOP("New Taiwan Dollar"), I18N_NOOP("TWD") },
    { "TJS", I18N_NOOP("Tajikistan"), I18N_NOOP("Somoni"), I18N_NOOP("TJS") },
    { "TZS", I18N_NOOP("Tanzania, United Republic Of"), I18N_NOOP("Tanzanian Shilling"), I18N_NOOP("TZS") },
    { "THB", I18N_NOOP("Thailand"), I18N_NOOP("Baht"), I18N_NOOP("THB") },
    { "XOF", I18N_NOOP("Togo"), I18N_NOOP("CFA Franc BCEAO"), I18N_NOOP("XOF") },
    { "NZD", I18N_NOOP("Tokelau"), I18N_NOOP("New Zealand Dollar"), I18N_NOOP("NZD") },
    { "TOP", I18N_NOOP("Tonga"), I18N_NOOP("Pa'anga"), I18N_NOOP("TOP") },
    { "TTD", I18N_NOOP("Trinidad And Tobago"), I18N_NOOP("Trinidad and Tobago Dollar"), I18N_NOOP("TT$") },
    { "TND", I18N_NOOP("Tunisia"), I18N_NOOP("Tunisian Dinar"), I18N_NOOP("TND") },
    { "TRL", I18N_NOOP("Turkey"), I18N_NOOP("Turkish Lira"), I18N_NOOP("TL") },
    { "TMM", I18N_NOOP("Turkmenistan"), I18N_NOOP("Manat"), I18N_NOOP("TMM") },
    { "USD", I18N_NOOP("Turks And Caicos Islands"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "AUD", I18N_NOOP("Tuvalu"), I18N_NOOP("Australian Dollar"), I18N_NOOP("AUD") },
    { "UGX", I18N_NOOP("Uganda"), I18N_NOOP("Uganda Shilling"), I18N_NOOP("UGX") },
    { "UAH", I18N_NOOP("Ukraine"), I18N_NOOP("Hryvnia"), I18N_NOOP("UAH") },
    { "AED", I18N_NOOP("United Arab Emirates"), I18N_NOOP("UAE Dirham"), I18N_NOOP("AED") },
    { "GBP", I18N_NOOP("United Kingdom"), I18N_NOOP("Pound Sterling"), I18N_NOOP("GBP") },
    { "USD", I18N_NOOP("United States"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "USN", I18N_NOOP("United States"), I18N_NOOP("US Dollar (Next day)"), I18N_NOOP("USN") },
    { "USS", I18N_NOOP("United States"), I18N_NOOP("US Dollar (Same day)"), I18N_NOOP("USS") },
    { "UYU", I18N_NOOP("Uruguay"), I18N_NOOP("Peso Uruguayo"), I18N_NOOP("NU$") },
    { "UZS", I18N_NOOP("Uzbekistan"), I18N_NOOP("Uzbekistan Sum"), I18N_NOOP("UZS") },
    { "VUV", I18N_NOOP("Vanuatu"), I18N_NOOP("Vatu"), I18N_NOOP("VUV") },
    { "VEB", I18N_NOOP("Venezuela"), I18N_NOOP("Bolivar"), I18N_NOOP("Bs") },
    { "VND", I18N_NOOP("Viet Nam"), I18N_NOOP("Dong"), I18N_NOOP("VND") },
    { "USD", I18N_NOOP("Virgin Islands"), I18N_NOOP("US Dollar"), I18N_NOOP("USD") },
    { "XPF", I18N_NOOP("Wallis And Futuna"), I18N_NOOP("CFP Franc"), I18N_NOOP("XPF") },
    { "MAD", I18N_NOOP("Western Sahara"), I18N_NOOP("Moroccan Dirham"), I18N_NOOP("MAD") },
    { "YER", I18N_NOOP("Yemen"), I18N_NOOP("Yemeni Rial"), I18N_NOOP("YER") },
    { "YUM", I18N_NOOP("Yugoslavia"), I18N_NOOP("Yugoslavian Dinar"), I18N_NOOP("YUM") },
    { "ZMK", I18N_NOOP("Zambia"), I18N_NOOP("Kwacha"), I18N_NOOP("ZMK") },
    { "ZWD", I18N_NOOP("Zimbabwe"), I18N_NOOP("Zimbabwe Dollar"), I18N_NOOP("ZWD") },
    { 0, 0, 0, 0}, // Last must be empty!
};


class CurrencyMap
{
public:
    CurrencyMap() : m_List(lMoney) {}

    // Those return the _untranslated_ strings from the above array
    QString code(int t) const {
        return QString::fromUtf8(m_List[t].code);
    }

    QString country(int t) const {
        return QString::fromUtf8(m_List[t].country);
    }

    QString name(int t) const {
        return QString::fromUtf8(m_List[t].name);
    }

    QString symbol(int t) const {
        return QString::fromUtf8(m_List[t].display);
    }

    int index(const QString& code) const {
        int index = 0;
        while (m_List[index].code != 0 && m_List[index].code != code)
            ++index;
        return (m_List[index].code != 0) ? index : 1 /*undef*/;
    }

private:
    const Money * m_List;
};

const CurrencyMap gCurrencyMap;
const Money * gMoneyList(lMoney);
}

using namespace Currency_LNS;

Currency::Currency(int index)
        : m_index(index)
        , m_code(gCurrencyMap.code(index))
{
}

Currency::Currency(QString const & code, Format format)
        : m_index(1)   // unspecified first, searched at the end of this ctor
        , m_code(code)
{
    if (format == Gnumeric) {
        // I use QChar(c,r) here so that this file can be opened in any encoding...
        if (code.indexOf(QChar(172, 32)) != -1)            // Euro sign
            m_code = QChar(172, 32);
        else if (code.indexOf(QChar(163, 0)) != -1)        // Pound sign
            m_code = QChar(163, 0);
        else if (code.indexOf(QChar(165, 0)) != -1)        // Yen sign
            m_code = QChar(165, 0);
        else if (code[0] == '[' && code[1] == '$') {
            int n = code.indexOf(']');
            if (n != -1)
                m_code = code.mid(2, n - 2);
            else
                m_index = 0;
        } else if (code.indexOf('$') != -1)
            m_code = '$';
    } // end gnumeric
    // search the corresponding index
    m_index = gCurrencyMap.index(m_code);
}

Currency::~Currency()
{
}

bool Currency::operator==(Currency const & cur) const
{
    if (m_index != cur.m_index)
        return false;
    if (m_code != cur.m_code)
        return false;
    return true;
}

QString Currency::code(Format format) const
{
    if (format == Gnumeric) {
        if (m_code.length() == 1)   // symbol
            return m_code;
        return QString("[$" + m_code + ']');
    }
    return m_code;
}

QString Currency::country() const
{
    return gCurrencyMap.country(m_index);
}

QString Currency::name() const
{
    return gCurrencyMap.name(m_index);
}

QString Currency::symbol() const
{
    if (m_index == 1)   // undefined
        return m_code;
    return gMoneyList[m_index].display;
}

int Currency::index() const
{
    return m_index;
}

QString Currency::chooseString(int type, bool & ok)
{
    if (!gMoneyList[type].country) {
        ok = false;
        return QString();
    }
    if (type < 29) {
        QString ret(i18n(gMoneyList[type].name));
        if (gMoneyList[type].country[0]) {
            ret += " (";
            ret += i18n(gMoneyList[type].country);
            ret += ')';
        }
        return ret;
    } else {
        QString ret(i18n(gMoneyList[type].country));
        if (gMoneyList[type].name[0]) {
            ret += " (";
            ret += i18n(gMoneyList[type].name);
            ret += ')';
        }
        return ret;
    }
}
