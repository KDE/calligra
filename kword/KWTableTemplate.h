/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwtabletemplate_h
#define kwtabletemplate_h

#include "KWTableStyle.h"

#include <qdom.h>
#include <qptrlist.h>
#include <qbrush.h>

class KWTableTemplate;
class KWDocument;

/******************************************************************/
/* Class: KWTableTemplateCollection                               */
/******************************************************************/

class KWTableTemplateCollection
{
public:
    KWTableTemplateCollection();
    ~KWTableTemplateCollection();
    const QPtrList<KWTableTemplate> & tableTemplateList() const { return m_templateList; }
    void clear();

    KWTableTemplate* findTableTemplate( const QString & name );
    /**
     * Return style number @p i.
     */
    KWTableTemplate* tableTemplateAt( int i ) { return m_templateList.at(i); }

    KWTableTemplate* addTableTemplate( KWTableTemplate *tt );

    void removeTableTemplate ( KWTableTemplate *tt );

private:
    QPtrList<KWTableTemplate> m_templateList;
    QPtrList<KWTableTemplate> m_deletedTemplates;
    KWTableTemplate *m_lastTemplate;
};

/******************************************************************/
/* Class: KWTableTemplate                                            */
/******************************************************************/

class KWTableTemplate
{
public:
    KWTableTemplate( const QString & name, KWTableStyle * _firstRow=0L, KWTableStyle * _firstCol=0L,
                     KWTableStyle *_lastRow=0L, KWTableStyle *_lastCol=0L, KWTableStyle *_bodyCell=0L,
                     KWTableStyle *_topLeftCorner=0L, KWTableStyle *_topRightCorner=0L,
                     KWTableStyle *_bottomLeftCorner=0L, KWTableStyle *_bottomRightCorner=0L );

    KWTableTemplate( const KWTableTemplate & rhs ) { *this = rhs; }

    KWTableTemplate( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

    ~KWTableTemplate() {}

    void operator=( const KWTableTemplate & );

    /** The internal name (untranslated if a standard style) */
    QString name() const { return m_name; }
    void setName( const QString & name ) { m_name = name; }
    /** The translated name */
    QString displayName() const;

    // ATTRIBUTES
    KWTableStyle firstRow() const { return *m_firstRow; }
    KWTableStyle* pFirstRow() const { return m_firstRow; }
    void setFirstRow( KWTableStyle *_tableStyle ) { m_firstRow = _tableStyle; }

    KWTableStyle lastRow() const { return *m_lastRow; }
    KWTableStyle* pLastRow() const { return m_lastRow; }
    void setLastRow( KWTableStyle *_tableStyle ) { m_lastRow = _tableStyle; }

    KWTableStyle firstCol() const { return *m_firstCol; }
    KWTableStyle* pFirstCol() const { return m_firstCol; }
    void setFirstCol( KWTableStyle *_tableStyle ) { m_firstCol = _tableStyle; }

    KWTableStyle lastCol() const { return *m_lastCol; }
    KWTableStyle* pLastCol() const { return m_lastCol; }
    void setLastCol( KWTableStyle *_tableStyle ) { m_lastCol = _tableStyle; }

    KWTableStyle bodyCell() const { return *m_bodyCell; }
    KWTableStyle* pBodyCell() const { return m_bodyCell; }
    void setBodyCell( KWTableStyle *_tableStyle ) { m_bodyCell = _tableStyle; }

    KWTableStyle topLeftCorner() const { return *m_topLeftCorner; }
    KWTableStyle* pTopLeftCorner() const { return m_topLeftCorner; }
    void setTopLeftCorner( KWTableStyle *_tableStyle ) { m_topLeftCorner = _tableStyle; }

    KWTableStyle topRightCorner() const { return *m_topRightCorner; }
    KWTableStyle* pTopRightCorner() const { return m_topRightCorner; }
    void setTopRightCorner( KWTableStyle *_tableStyle ) { m_topRightCorner = _tableStyle; }

    KWTableStyle bottomRightCorner() const { return *m_bottomRightCorner; }
    KWTableStyle* pBottomRightCorner() const { return m_bottomRightCorner; }
    void setBottomRightCorner( KWTableStyle *_tableStyle ) { m_bottomRightCorner = _tableStyle; }

    KWTableStyle bottomLeftCorner() const { return *m_bottomLeftCorner; }
    KWTableStyle* pBottomLeftCorner() const { return m_bottomLeftCorner; }
    void setBottomLeftCorner( KWTableStyle *_tableStyle ) { m_bottomLeftCorner = _tableStyle; }

    // SAVING METHODS
    void save( QDomElement parentElem, KoTextZoomHandler* zh );
    void saveTableTemplate( QDomElement & parentElem );

    // STATIC METHODS
    static KWTableTemplate *loadTemplate( QDomElement & parentElem, KWDocument *_doc, int docVersion=2 );

private:
    QString m_name;
    KWTableStyle *m_firstRow;
    KWTableStyle *m_lastRow;
    KWTableStyle *m_firstCol;
    KWTableStyle *m_lastCol;
    KWTableStyle *m_bodyCell;

    KWTableStyle *m_topLeftCorner;
    KWTableStyle *m_topRightCorner;
    KWTableStyle *m_bottomRightCorner;
    KWTableStyle *m_bottomLeftCorner;

};

#endif
