/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kwformat_h
#define kwformat_h
#include "qrichtext_p.h"
using namespace Qt3;

class KWDocument;

/**
 * KWord's reimplementation of QTextFormat, to add setPixelSizeFloat().
 */
class KWTextFormat : public QTextFormat
{
public:
    KWTextFormat() : QTextFormat() {}
    virtual ~KWTextFormat() {}
    //KWTextFormat( const QStyleSheetItem *s );
    KWTextFormat( const QFont &f, const QColor &c, QTextFormatCollection * coll )
      : QTextFormat( f, c, coll ) { generateKey(); }
    KWTextFormat( const KWTextFormat &fm );
    //KWTextFormat& operator=( const KWTextFormat &fm );

    virtual void copyFormat( const QTextFormat & nf, int flags );

    // Return a set of flags showing the differences between this and 'format'
    int compare( const KWTextFormat & format ) const;

    // Extending QTextFormat's enum
    enum { StrikeOut = 256 };

    void setPointSizeFloat( float );
    float pointSizeFloat() const { return fn.pointSizeFloat(); }
    void setStrikeOut(bool );
protected:
    virtual void generateKey();
};

/**
 * KWord's reimplementation of QTextFormatCollection, to provide KWTextFormats.
 */
class KWTextFormatCollection : public QTextFormatCollection
{
public:
    KWTextFormatCollection( KWDocument * doc );
    virtual ~KWTextFormatCollection() {}

    virtual QTextFormat *format( const QFont &f, const QColor &c );
    virtual void remove( QTextFormat *f );

    virtual QTextFormat *createFormat( const QTextFormat &fm ) { return new KWTextFormat( static_cast<const KWTextFormat &>(fm) ); }
    virtual QTextFormat *createFormat( const QFont &f, const QColor &c ) { return new KWTextFormat( f, c, this ); }

private:
    KWTextFormat * m_cachedFormat;
    QFont m_cfont;
    QColor m_ccol;

};

#endif
