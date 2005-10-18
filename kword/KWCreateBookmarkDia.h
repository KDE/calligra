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

#ifndef __KWCreateBookmarkDia__
#define __KWCreateBookmarkDia__

#include <kdialogbase.h>
#include <qstringlist.h>
class QLineEdit;
class QListBox;
class KWDocument;
class QPushButton;

class KWCreateBookmarkDia : public KDialogBase
{
    Q_OBJECT
public:
    KWCreateBookmarkDia( const QStringList & _list, QWidget *parent, const char *name );
    KWCreateBookmarkDia( const QStringList & _list, const QString & _name, QWidget *parent, const char *name );
    QString bookmarkName()const;

protected slots:
    virtual void slotOk();
    void nameChanged( const QString &text);

protected:
    void init();
    QStringList listBookMark;
    QLineEdit *m_bookmarkName;
};

class KWSelectBookmarkDia : public KDialogBase
{
    Q_OBJECT
public:
    KWSelectBookmarkDia( const QStringList & _list, KWDocument *_doc, QWidget *parent, const char *name );
    QString bookmarkSelected()const;
protected slots:
    void slotSelectionChanged();
    void slotRenameBookmark();
    void slotDeleteBookmark();
private:
    QListBox *m_bookmarkList;
    KWDocument *m_doc;
    QPushButton *m_pbDelete;
    QPushButton *m_pbRename;
};

#endif
