/******************************************************************/
/* KSearchDialogs - (c) by Reginald Stadlbauer 1998               */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelect is under GNU GPL                                   */
/******************************************************************/
/* Module: Search Dialogs (header)                                */
/******************************************************************/

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
