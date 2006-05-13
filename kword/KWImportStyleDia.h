/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KWImportStyleDia__
#define __KWImportStyleDia__

#include <kdialogbase.h>
#include <KoImportStyleDia.h>
#include <QStringList>
//Added by qt3to4:
#include <Q3PtrList>

#include "KWFrameStyle.h"
#include "KWTableStyle.h"

class QLineEdit;
class Q3ListBox;
class KWDocument;
class QPushButton;
class KoParagStyle;

class KWImportStyleDia : public KoImportStyleDia
{
    Q_OBJECT
public:
    KWImportStyleDia( KWDocument *_doc, KoStyleCollection* currentCollection, QWidget *parent, const char *name = 0 );
    ~KWImportStyleDia();

protected:
    virtual void loadFile();

private:
    KWDocument *m_doc;
};


class KWImportFrameTableStyleDia : public KDialogBase
{
    Q_OBJECT
public:
    enum StyleType { frameStyle, TableStyle};
    KWImportFrameTableStyleDia( KWDocument *_doc, const QStringList & _list, StyleType _type , QWidget *parent, const char *name );
    ~KWImportFrameTableStyleDia();
    Q3PtrList<KWFrameStyle> listOfFrameStyleImported() const { return m_frameStyleList;}
    Q3PtrList<KWTableStyle> listOfTableStyleImported() const { return m_tableStyleList;}

protected slots:
    virtual void slotOk();

protected:
    QString generateStyleName( const QString & templateName );
    void loadFile();
    void initList();
    Q3ListBox *m_listStyleName;
    KWDocument *m_doc;
    Q3PtrList<KWFrameStyle> m_frameStyleList;
    Q3PtrList<KWTableStyle> m_tableStyleList;
    QStringList m_list;
    StyleType m_typeStyle;
};

#endif
