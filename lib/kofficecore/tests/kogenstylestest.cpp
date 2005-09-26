/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koGenStyles.h>
#include <koxmlwriter.h>
#include "../../store/tests/xmlwritertest.h"
#include <kdebug.h>
#include <assert.h>

int main( int, char** ) {

    KoGenStyles coll;

    QMap<QString, QString> map1;
    map1.insert( "map1key", "map1value" );
    QMap<QString, QString> map2;
    map2.insert( "map2key1", "map2value1" );
    map2.insert( "map2key2", "map2value2" );

    KoGenStyle first( KoGenStyle::STYLE_AUTO, "paragraph" );
    first.addAttribute( "style:master-page-name", "Standard" );
    first.addProperty( "style:page-number", "0" );
    first.addProperty( "style:foobar", "2", KoGenStyle::TextType );
    first.addStyleMap( map1 );
    first.addStyleMap( map2 );

    QString firstName = coll.lookup( first );
    kdDebug() << "The first style got assigned the name " << firstName << endl;
    assert( firstName == "A1" ); // it's fine if it's something else, but the koxmlwriter tests require a known name
    assert( first.type() == KoGenStyle::STYLE_AUTO );

    KoGenStyle second( KoGenStyle::STYLE_AUTO, "paragraph" );
    second.addAttribute( "style:master-page-name", "Standard" );
    second.addProperty( "style:page-number", "0" );
    second.addProperty( "style:foobar", "2", KoGenStyle::TextType );
    second.addStyleMap( map1 );
    second.addStyleMap( map2 );

    QString secondName = coll.lookup( second );
    kdDebug() << "The second style got assigned the name " << secondName << endl;

    assert( firstName == secondName ); // check that sharing works
    assert( first == second ); // check that operator== works :)

    const KoGenStyle* s = coll.style( firstName ); // check lookup of existing style
    assert( s );
    assert( *s == first );
    s = coll.style( "foobarblah" ); // check lookup of non-existing style
    assert( !s );

    KoGenStyle third( KoGenStyle::STYLE_AUTO, "paragraph", secondName ); // inherited style
    third.addProperty( "style:margin-left", "1.249cm" );
    third.addProperty( "style:page-number", "0" ); // same as parent
    third.addProperty( "style:foobar", "3", KoGenStyle::TextType ); // different from parent
    assert( third.parentName() == secondName );

    QString thirdName = coll.lookup( third, "P" );
    kdDebug() << "The third style got assigned the name " << thirdName << endl;
    assert( thirdName == "P1" );

    KoGenStyle user( KoGenStyle::STYLE_USER ); // differs from third since it doesn't inherit second, and has a different type
    user.addProperty( "style:margin-left", "1.249cm" );

    QString userStyleName = coll.lookup( user, "User", false );
    kdDebug() << "The user style got assigned the name " << userStyleName << endl;
    assert( userStyleName == "User" );

    KoGenStyle sameAsParent( KoGenStyle::STYLE_AUTO, "paragraph", secondName ); // inherited style
    sameAsParent.addAttribute( "style:master-page-name", "Standard" );
    sameAsParent.addProperty( "style:page-number", "0" );
    sameAsParent.addProperty( "style:foobar", "2", KoGenStyle::TextType );
    sameAsParent.addStyleMap( map1 );
    sameAsParent.addStyleMap( map2 );
    QString sapName = coll.lookup( sameAsParent, "foobar" );
    kdDebug() << "The 'same as parent' style got assigned the name " << sapName << endl;

    assert( sapName == secondName );

    // OK, now add a style marked as for styles.xml
    KoGenStyle headerStyle( KoGenStyle::STYLE_AUTO, "paragraph" );
    headerStyle.addAttribute( "style:master-page-name", "Standard" );
    headerStyle.addProperty( "style:page-number", "0" );
    headerStyle.addStyleMap( map1 );
    headerStyle.addStyleMap( map2 );
    QString headerStyleName = coll.lookup( headerStyle, "hs" );
    coll.markStyleForStylesXml( headerStyleName );

    assert( coll.styles().count() == 4 );
    assert( coll.styles( KoGenStyle::STYLE_AUTO ).count() == 2 );
    assert( coll.styles( KoGenStyle::STYLE_USER ).count() == 1 );

    QValueList<KoGenStyles::NamedStyle> stylesXmlStyles = coll.styles( KoGenStyle::STYLE_AUTO, true );
    assert( stylesXmlStyles.count() == 1 );
    KoGenStyles::NamedStyle firstStyle = stylesXmlStyles.first();
    assert( firstStyle.name == headerStyleName );

    TEST_BEGIN( 0, 0 );
    first.writeStyle( &writer, coll, "style:style", firstName, "style:paragraph-properties" );
    TEST_END( "XML for first/second style", "<r>\n <style:style style:name=\"A1\" style:family=\"paragraph\" style:master-page-name=\"Standard\">\n  <style:paragraph-properties style:page-number=\"0\"/>\n  <style:text-properties style:foobar=\"2\"/>\n  <style:map map1key=\"map1value\"/>\n  <style:map map2key1=\"map2value1\" map2key2=\"map2value2\"/>\n </style:style>\n</r>\n" );

    TEST_BEGIN( 0, 0 );
    third.writeStyle( &writer, coll, "style:style", thirdName, "style:paragraph-properties" );
    TEST_END( "XML for third style", "<r>\n <style:style style:name=\"P1\" style:parent-style-name=\"A1\" style:family=\"paragraph\">\n  <style:paragraph-properties style:margin-left=\"1.249cm\"/>\n  <style:text-properties style:foobar=\"3\"/>\n </style:style>\n</r>\n" );


    fprintf( stderr, "OK\n" );

    return 0;
}
