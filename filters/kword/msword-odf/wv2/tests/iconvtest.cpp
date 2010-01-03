/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <test.h>

#include "textconverter.h"
#include "ustring.h"

using namespace wvWare;

int cp_test( const std::string& codepage );

// A small testcase for the text converter (iconv interface)
int main( int, char** )
{
    int status = 0;

    std::cout << "===================================" << std::endl;
    std::cout << "textconverter: (1) codepage support" << std::endl;
    std::cout << "===================================" << std::endl;

    status += cp_test( "CP874" );
    status += cp_test( "CP936" );
    status += cp_test( "CP932" );
    status += cp_test( "CP949" );
    status += cp_test( "CP950" );
    status += cp_test( "CP1250" );
    status += cp_test( "CP1251" );
    status += cp_test( "CP1252" );
    status += cp_test( "CP1253" );
    status += cp_test( "CP1254" );
    status += cp_test( "CP1255" );
    status += cp_test( "CP1256" );
    status += cp_test( "CP1257" );
    status += cp_test( "UCS-2" );
    status += cp_test( "UCS-2-INTERNAL" );
    status += cp_test( "UNICODEBIG" );
    status += cp_test( "UNICODELITTLE" );
    status += cp_test( "UTF-8" );
    status += cp_test( "koi8-r" );
    status += cp_test( "tis-620" );
    status += cp_test( "iso-8859-15" );

    std::cout << "==========================================" << std::endl;
    std::cout << "textconverter: (2) conversion completeness" << std::endl;
    std::cout << "==========================================" << std::endl;

    TextConverter converter(0x0407);

    std::string t( "The quick brown fox jumped over the lazy dog." );

    UString result = converter.convert( t );

    std::cout << "String: " << t << std::endl;
    std::cout << "Result: " << result.ascii() << std::endl;

    test( strcmp( t.c_str(), result.ascii() ) == 0, "The strings aren't matching!" );

    /*
    converter.setFromCode( "CP1255" ); // Hebrew
    t = "אירופה, תוכנה והאינטרנט: Unicode יוצא לשוק העולמי הירשמו כעת לכנס Unicode הבינלאומי העשירי, שייערך בין התאריכים 12־10 במרץ 1997, במיינץ שבגרמניה. בכנס ישתתפו מומחים מכל ענפי התעשייה בנושא האינטרנט העולמי וה־Unicode, בהתאמה לשוק הבינלאומי והמקומי, ביישום Unicode במערכות הפעלה וביישומים, בגופנים, בפריסת טקסט ובמחשוב רב־לשוני. some english inbetween כאשר העולם רוצה לדבר, הוא מדבר ב־Unicode";

    result = converter.convert( t );
    std::cout << "String: " << t << std::endl;
    std::cout << "Result: " << result.ascii() << std::endl;
    */
    std::cout << "===================" << std::endl;
    std::cout << "textconverter: done" << std::endl;
    std::cout << "failures=" << status << std::endl;
    std::cout << "===================" << std::endl;

    return status;
}

int cp_test( const std::string& codepage )
{
    int status = 0;

    TextConverter tc( codepage );
    if ( !tc.isOk() )
        status=1;
    std::cout << codepage << ": " << ( status==0 ? "yes" : "no" ) << std::endl;

    return status;
}
