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

#include <koOasisSettings.h>
#include <kodom.h>
#include <kdebug.h>
#include <assert.h>

void testSelectItemSet( KoOasisSettings& settings )
{
    bool ok = settings.selectItemSet( "notexist" );
    assert( !ok );
    ok = settings.selectItemSet( "view-settings" );
    assert( ok );
}

void testParseConfigItemString( KoOasisSettings& settings )
{
    const QString unit = settings.parseConfigItemString( "unit" );
    qDebug( "%s", unit.latin1() );
    assert( unit == "mm" );
}

void testSelectItemMap( KoOasisSettings& settings )
{
    bool ok = settings.selectItemSet( "view-settings" );
    assert( ok );
    ok = settings.selectItemMap( "Views" );
    assert( ok );
}

int main( int, char** ) {

    const QCString xml = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
<office:document-settings xmlns:office=\"urn:oasis:names:tc:openoffice:xmlns:office:1.0\" xmlns:config=\"urn:oasis:names:tc:openoffice:xmlns:config:1.0\"> \
 <office:settings> \
  <config:config-item-set config:name=\"view-settings\"> \
    <config:config-item config:name=\"unit\" config:type=\"string\">mm</config:config-item> \
    <config:config-item-map-indexed config:name=\"Views\"> \
      <config:config-item-map-entry> \
        <config:config-item config:name=\"SnapLinesDrawing\" config:type=\"string\">value</config:config-item> \
      </config:config-item-map-entry> \
    </config:config-item-map-indexed> \
  </config:config-item-set> \
 </office:settings> \
</office:document-settings> \
";

    QDomDocument doc;
    bool ok = doc.setContent( xml, true /* namespace processing */ );
    assert( ok );

    KoOasisSettings settings( doc );

    testSelectItemSet( settings );
    testParseConfigItemString( settings );
    testSelectItemMap( settings );
    return 0;
}
