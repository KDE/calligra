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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KoImportStyleDia__
#define __KoImportStyleDia__

#include <kdialogbase.h>
#include <qstringlist.h>
#include <kostyle.h>
class QLineEdit;
class QListBox;
class QPushButton;

class KoImportStyleDia : public KDialogBase
{
    Q_OBJECT
public:
    KoImportStyleDia( const QStringList & _list, QWidget *parent, const char *name );
    ~KoImportStyleDia();

    QPtrList<KoParagStyle> listOfStyleImported()const { return m_styleList;}
protected slots:
    virtual void slotOk();
    void slotLoadFile();
protected:
    /**  Open file dialog and load the list of styles from the selected doc.
     */
    virtual void loadFile()=0;

    void generateStyleList();
    void updateFollowingStyle(const QString & _name);

    QString generateStyleName( const QString & templateName );

    KoParagStyle *findStyle( const QString & _name);

    void initList();

    QListBox *m_listStyleName;
    QPtrList<KoParagStyle> m_styleList;

    QStringList m_list;
};

#endif
