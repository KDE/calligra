/* This file is part of the KDE project
   Copyright (C) 1998, 1999 David Faure <faure@kde.org>
 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#include "koUIUtils.h"

OpenPartsUI::Pixmap* KOUIUtils::colorPixmap( QColor c, PType _type )
{
    int r, g, b;
    QString pix;
    QString line;
 
    c.rgb( &r, &g, &b );
 
    pix = "/* XPM */\n";
 
    pix += "static char * text_xpm[] = {\n";
 
    switch ( _type )
    {
      case TXT_COLOR:
      {
        pix += "\"20 20 11 1\",\n";
        pix += "\"h c #c0c000\",\n";
        pix += "\"g c #808000\",\n";
        pix += "\"f c #c0c0ff\",\n";
        pix += "\"a c #000000\",\n";
        pix += "\"d c #ff8000\",\n";
        pix += "\". c none\",\n";
        pix += "\"e c #0000c0\",\n";
        pix += "\"i c #ffff00\",\n";
        line.sprintf( "\"# c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();
        pix += "\"b c #c00000\",\n";
        pix += "\"c c #ff0000\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"........#...........\",\n";
        pix += "\"........#a..........\",\n";
        pix += "\".......###..........\",\n";
        pix += "\".......###a.........\",\n";
        pix += "\"......##aa#.........\",\n";
        pix += "\"......##a.#a........\",\n";
        pix += "\".....##a...#........\",\n";
        pix += "\".....#######a.......\",\n";
        pix += "\"....##aaaaaa#.......\",\n";
        pix += "\"....##a.....aaaaaaaa\",\n";
        pix += "\"...####....#abbccdda\",\n";
        pix += "\"....aaaa....abbccdda\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............aaaaaaaa\"};\n"; 
      } break;
      case FRAME_COLOR:
      {
	pix += "\" 20 20 3 1 \",\n";

	pix += "\"  c none \",\n";
	pix += "\"+ c white \",\n";
	line.sprintf( "\". c #%02X%02X%02X \",\n", r, g, b );
	pix += line.copy();

	pix += "\"                     \",\n";
	pix += "\"                     \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"                     \",\n";
	pix += "\"                     \";\n";
      } break;
      case BACK_COLOR:
      {
        pix += "\" 20 20 3 1 \",\n";
 
        pix += "\"  c none \",\n";
        pix += "\". c red \",\n";
        line.sprintf( "\"+ c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();
 
        pix += "\"                     \",\n";
        pix += "\"                     \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"                     \",\n";
        pix += "\"                     \";\n";
      } break;
    }
 
  OpenPartsUI::Pixmap* p = new OpenPartsUI::Pixmap;
  p->data = CORBA::string_dup( pix );
  p->onlyFilename = false;
  return p;
}
