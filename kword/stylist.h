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

#ifndef stylist_h
#define stylist_h

#include <kdialogbase.h>
#include <qstringlist.h>

#include "paragdia.h"

class KWDocument;
class KWStyleEditor;
class QWidget;
class QGridLayout;
class QListBox;
class QPushButton;
class KButtonBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QCheckBox;
class KWStyle;
class QTabWidget;
class KWStyleManagerTab;
class KWStylePreview;

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

class KWStyleManager : public QDialog
{
    Q_OBJECT

public:
    KWStyleManager( QWidget *_parent, KWDocument *_doc );

protected:
    KWDocument *m_doc;

    void setupWidget();
    void addGeneralTab();
    void apply();
    void updateGUI();
    void save();
    int getStyleByName(const QString name);

    QTabWidget *m_tabs;
    QListBox *m_stylesList;
    QLineEdit *m_nameString;
    QComboBox *m_styleCombo;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_deleteButton;
    QPushButton *m_newButton;
    KWStylePreview *preview;

    KWStyle *m_currentStyle;
    QList<KWStyle> m_origStyles;      // internal list of orig styles we have modified
    QList<KWStyle> m_changedStyles;   // internal list of changed styles.
    QList<KWStyleManagerTab> m_tabsList;
    int numStyles;
    bool noSignals;

protected slots:
    virtual void slotOk();
    virtual void slotCancel();
    void switchStyle();
    void addStyle();
    void deleteStyle();
    void renameStyle(const QString &);
    void addTab( KWStyleManagerTab * tab );
};

/******************************************************************/
/* Class: KWStylePreview                                         */
/******************************************************************/

class KWStylePreview : public QGroupBox
{
    Q_OBJECT

public:
    KWStylePreview( const QString &title, QWidget *parent, KWStyle *_style ) : QGroupBox( title, parent, "" )
    { style = _style; }

    void setStyle(KWStyle *_style) { style= _style; }

protected:
    void drawContents( QPainter *painter );

    KWStyle *style;
};

class KWStyleManagerTab : public QWidget {
    Q_OBJECT
public:
    KWStyleManagerTab(QWidget *parent) : QWidget(parent) {};

    /** the new style which is to be displayed */
    void setStyle(KWStyle *style) { m_style = style; }
    /**  update the GUI from the current Style*/
    virtual void update() = 0;
    /**  return the (i18n-ed) name of the tab */
    virtual QString tabName() = 0;
    /** save the GUI to the style */
    virtual void save() = 0;
protected:
    KWStyle *m_style;
};

// A tab to edit parts of the parag-layout of the style
// Acts as a wrapper around KWParagLayoutWidget [which doesn't know about styles].
class KWStyleParagTab : public KWStyleManagerTab
{
    Q_OBJECT
public:
    KWStyleParagTab( QWidget * parent )
        : KWStyleManagerTab( parent ) { m_widget = 0L; }

    // not a constructor parameter since 'this' is the parent of the widget
    void setWidget( KWParagLayoutWidget * widget );

    virtual void update() { m_widget->display( m_style->paragLayout() ); }
    virtual void save() { m_widget->save( m_style->paragLayout() ); }
    virtual QString tabName() { return m_widget->tabName(); }
protected:
    virtual void resizeEvent( QResizeEvent *e );
private:
    KWParagLayoutWidget * m_widget;
};

/*
class KWStyleFontTab : public KWStyleManagerTab {
    virtual void update();
    virtual QString tabName(); }
    virtual void save();
}*/

/*
Font            simple font dia
Color           simple color dia
Spacing and Indents     paragraph spacing dia (KWParagDia)
alignments      KWParagDia alignment tab
borders         KWParagDia  borders tab
numbering       KWParagDia  tab numbering
tabulators      KWParagDia  tab tabs */

#endif
