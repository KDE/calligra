/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Variable Dialogs                                       */
/******************************************************************/

#ifndef KWVARIABLEDLGS_H
#define KWVARIABLEDLGS_H

#include "variable.h"

#include <qdialog.h>
#include <qlist.h>
#include <qstring.h>
#include <qlistview.h>

class QComboBox;
class QVBox;
class QResizeEvent;
class QLineEdit;
class QCloseEvent;

/******************************************************************
 *
 * Class: KWVariableNameDia
 *
 ******************************************************************/

class KWVariableNameDia : public QDialog
{
    Q_OBJECT
    
public:
    KWVariableNameDia( QWidget *parent, QList<KWVariable> *vars );
    QString getName() const;
    
protected:
    void resizeEvent( QResizeEvent *e );
    
    QList<KWVariable> *variables;
    QComboBox *names;
    QVBox *back;
    
};    

/******************************************************************
 *
 * Class: KWVariableValueListItem
 *
 ******************************************************************/

class KWVariableValueListItem : public QListViewItem
{
public:
    KWVariableValueListItem( QListView *parent );
    
    void setVariable( KWCustomVariable *v );
    KWCustomVariable *getVariable() const;
    
    void setup();
    void update();
    
    void applyValue();
    
protected:
    KWCustomVariable *var;
    QLineEdit *editWidget;
    
};
    
/******************************************************************
 *
 * Class: KWVariableValueList
 *
 ******************************************************************/

class KWVariableValueList : public QListView
{
    Q_OBJECT
    
public:
    KWVariableValueList( QWidget *parent );
    
    void setValues();
    void updateItems();
    
protected slots:
    void columnSizeChange( int c, int os, int ns );
    void sectionClicked( int c );

};

/******************************************************************
 *
 * Class: KWVariableValueDia
 *
 ******************************************************************/

class KWVariableValueDia : public QDialog
{
    Q_OBJECT
    
public:
    KWVariableValueDia( QWidget *parent, QList<KWVariable> *vars );

protected:
    void resizeEvent( QResizeEvent *e );
    void closeEvent( QCloseEvent *e );
    
    QList<KWVariable> *variables;
    QVBox *back;
    KWVariableValueList *list;
    
};

#endif
