/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#ifndef __WPIMPORT_H
#define __WPIMPORT_H

#include <qstring.h>
#include <qdatastream.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qcstring.h>

#include <qdom.h>

#include <koFilter.h>
#include <koStore.h>

class WPParagraphLayout
{
  public:
    int justification;
    int linespace;
};

typedef struct 
{

  typedef struct
  {
    int leftmargin, rightmargin;
    int topmargin, bottommargin;
    int columns, columnspacing;
  } PageSettings;

  typedef struct
  {
    QString desc_name;
    QString desc_type;
    QString subject;
    QString author;
    QString typist;
    QString abstract;
  } Summary;
  
  typedef struct
  { 
    int type;
    long size, pos;
    QArray<int> data;
  } Packet;

  typedef struct
  {
    int type, pos, len;
  } TextFormat;

  typedef struct   
  {
    int pos, len;
    int red, green, blue;
  } FontColor;

  typedef struct
  {
    int major, minor;
  } Version;
  
  QPtrList<Packet> packets;
  QPtrList<TextFormat> formats;
  QPtrList<FontColor> colors;

  Summary summary;
  PageSettings pagesettings;
  Version version;

} WPDocument;

class WPImport : public KoFilter 
{

    Q_OBJECT


  protected:

    long m_docstart;

    QDataStream stream;
    QString m_output;

    QString m_text;

    WPParagraphLayout m_layout;

    WPDocument document;
    QDomDocument root;
    QDomElement frameset;

  public:

    WPImport(KoFilter *parent, const char *name);
    WPImport();

    virtual ~WPImport() {}

    virtual bool filter(const QString &fileIn, const QString &fileOut,
                        const QString &from, const QString &to,
                        const QString &config=QString::null);

    int readByte();

    bool readHeader();
    bool parseDocument();

    void handleFunction( int function, int subfunction, QArray<int>& data );
    void handlePacket( int type, QArray<int>& data );

    void resetTextFormat();
    void resetParagraphLayout();

    void writeParagraph();

};
#endif // __WPIMPORT_H



