/* This file is part of the KDE project
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

#ifndef KSEARCHDIALOGS_H
#define KSEARCHDIALOGS_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>

class QPushButton;
class QCheckBox;
class QGroupBox;
class QLineEdit;
class QLabel;

/******************************************************************/
/* Class: KSearchDialog                                           */
/******************************************************************/

class KSearchDialog : public QDialog
{
    Q_OBJECT

public:
    KSearchDialog ( QWidget *parent=0, const char *name=0 );

    void setText( QString text ) {value->setText( text ); }
    void setCaseSensitive( bool s ) {sensitive->setChecked( s ); }
    void setDirection( bool d ) {direction->setChecked( d ); }

protected:
    QPushButton *ok, *cancel;
    QCheckBox *sensitive;
    QCheckBox *direction;
    QGroupBox *frame;
    QLineEdit *value;

protected slots:
    void search() {emit doSearch( value->text(), sensitive->isChecked(), direction->isChecked() ); }

signals:
    void doSearch( QString, bool, bool );

};


/******************************************************************/
/* Class: KSearchReplaceDialog                                    */
/******************************************************************/
class KSearchReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    KSearchReplaceDialog( QWidget *parent=0, const char *name=0 );

    void setText( QString ssearch ) {search->setText( ssearch ); }
    void setReplaceText( QString sreplace ) {replace->setText( sreplace ); }
    void setCaseSensitive( bool s ) {sensitive->setChecked( s ); }
    void setDirection( bool d ) {direction->setChecked( d ); }

protected:
    QPushButton *ok, *cancel, *replace_all;
    QCheckBox *sensitive;
    QCheckBox *direction;
    QGroupBox *frame;
    QLineEdit *search, *replace;
    QLabel *label;

protected slots:
    void searchReplace() {emit doSearchReplace( search->text(), replace->text(), sensitive->isChecked(), direction->isChecked() ); }
    void searchReplaceAll() {emit doSearchReplaceAll( search->text(), replace->text(), sensitive->isChecked() ); }
    void gotoReplace() {replace->setFocus(); }

signals:
    void doSearchReplace( QString, QString, bool, bool );
    void doSearchReplaceAll( QString, QString, bool );

};

#endif
