/*
   This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

#ifndef koTemplateChooseDia_h
#define koTemplateChooseDia_h

#include <kdialog.h>
#include <kicondialog.h>


class MyIconCanvas : public KIconCanvas
{
    Q_OBJECT

public:
    MyIconCanvas( QWidget *parent = 0, const QString &name = QString::null )
	: KIconCanvas( parent, name ) {};

    bool isCurrentValid() { return currentItem(); }
    void loadDir( const QString &dirname, const QString &filter );

protected:
    void viewportMousePressEvent( QMouseEvent *e ) {
	KIconCanvas::viewportMousePressEvent( e );
	if ( isCurrentValid() ) {
	    QString s = getCurrent();
	    emit currentChanged( s );
	} else {
	    QString s = "";
	    emit currentChanged( s );
	}
    }

signals:
    void currentChanged( const QString & );
};


class KInstance;
class KoTemplateChooseDiaPrivate;

/**
 *  class KoTemplateChooseDia
 */
class KoTemplateChooseDia : public KDialog
{
    Q_OBJECT

public:
    enum ReturnType {Cancel, Template, File, Empty};
    enum DialogType {Everything, OnlyTemplates, NoTemplates};

    KoTemplateChooseDia(QWidget *parent, const char *name, const QString& templateType,
			KInstance* global, const QString &importFilter, const QString &mimeType,
			bool hasCancel=true, const DialogType &dialogType=Everything);
    ~KoTemplateChooseDia();

    static ReturnType choose(const QString& templateType, KInstance* global, QString &file,
			     const DialogType &dialogType=Everything, bool hasCancel=true,
			     const QString &importFilter=QString::null,
			     const QString &mimeType=QString::null);

    QString getTemplate();
    QString getFullTemplate();
    ReturnType getReturnType();
    DialogType getDialogType();

private:
    KoTemplateChooseDiaPrivate *d;

    void getGroups();
    void setupTabs();

private slots:
    void chosen();
    void currentChanged( const QString & );

    void openTemplate();
    void openFile();
    void openEmpty();
    void chooseFile();
    void tabsChanged( const QString & );

signals:
    void templateChosen( const QString & );
};
#endif
