/*
   This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef koTemplateCreateDia_h
#define koTemplateCreateDia_h

#include <kdialogbase.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qpoint.h>

class QListViewItem;
class QListView;
class QLineEdit;

/****************************************************************************
 *
 * Class: koTemplateCreateDia
 *
 ****************************************************************************/

class KoTemplateCreateDia : public KDialogBase
{
    Q_OBJECT
    
public:
    KoTemplateCreateDia( QWidget *parent, const QString &file_, const QPixmap &pix,
			 const QStringList &templateRoots_, const QString extension_ );

    static void createTemplate( QWidget *parent, const QString &file_, const QPixmap &pix,
				const QStringList &templateRoots_, const QString extension_ );
    
private slots:
    void createFolder( QListViewItem *item, const QPoint &, int );
    void doubleClicked( QListViewItem *item );
    void selectionChanged( QListViewItem *item );
    void nameChanged( const QString &name );
    void useGeneratedPixmap();
    void useChosenPixmap();
    void iconChanged( const QString &name );
    void ok();
    void slotOk() {
	emit okClicked();
    }
    
private:
    QPixmap pixmap;
    QString extension, file;
    QString currName;
    QPixmap currPixmap;
    QListView *folderList;
    QLineEdit *lined;
    
};
    
#endif
