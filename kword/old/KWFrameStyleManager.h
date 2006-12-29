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

#ifndef kwframestylemanager_h
#define kwframestylemanager_h

#include "KWFrameStyle.h"

#include <kdialog.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QResizeEvent>
#include <QPaintEvent>
#include <Q3ListBox>

class Q3GroupBox;
class Q3GridLayout;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QWidget;
class KWDocument;
class KWFrameStyleManagerTab;
class KoParagLayoutWidget;
class KoParagLayout;
class KColorButton;
class KWBrushStylePreview;

/******************************************************************/
/* Class: KWFrameStylePreview                                     */
/******************************************************************/

class KWFrameStylePreview : public QWidget
{
    Q_OBJECT

public:
    KWFrameStylePreview( QWidget *parent )
        : QWidget( parent ), frameStyle( 0 ) {
            setBackgroundColor( Qt::white );
        }

    void setFrameStyle(KWFrameStyle *_frameStyle);

protected:
    void paintEvent( QPaintEvent *e );

    void setTopBorder(KoBorder _bTop) { frameStyle->setTopBorder( _bTop ); repaint( true ); }
    void setBottomBorder(KoBorder _bBottom) { frameStyle->setBottomBorder( _bBottom ); repaint( true ); }
    void setRightBorder(KoBorder _bRight) { frameStyle->setRightBorder( _bRight ); repaint( true ); }
    void setLeftBorder(KoBorder _bLeft) { frameStyle->setLeftBorder( _bLeft ); repaint( true ); }
    void setBackground( const QColor & col ) { frameStyle->setBackgroundColor( col ); repaint( true ); }

    KWFrameStyle *frameStyle;
};


class KWFrameStyleListItem
{
  public:
    KWFrameStyleListItem() {}
    ~KWFrameStyleListItem();
    KWFrameStyleListItem(KWFrameStyle *orig, KWFrameStyle *changed) {
        m_origFrameStyle = orig;
        m_changedFrameStyle = changed;
    }

    KWFrameStyle *origFrameStyle()const { return m_origFrameStyle; }
    KWFrameStyle *changedFrameStyle()const { return m_changedFrameStyle; }
    void setOrigFrameStyle( KWFrameStyle *_orig ) { m_origFrameStyle = _orig; }
    void setChangedFrameStyle( KWFrameStyle *_changed ) { m_changedFrameStyle = _changed; }
    void deleteOrigFrameStyle() { delete m_changedFrameStyle; }
    void deleteChangedFrameStyle() { delete m_changedFrameStyle; }

    void switchStyle();
    void deleteStyle( KWFrameStyle *current );
    void apply();

  protected:
    KWFrameStyle *m_origFrameStyle;
    KWFrameStyle *m_changedFrameStyle;
};

/******************************************************************/
/* Class: KWFrameStyleManager                                     */
/******************************************************************/
class KWFrameStyleManager : public KDialog
{
    Q_OBJECT

public:
    KWFrameStyleManager( QWidget *_parent, KWDocument *_doc,
                         const QString & activeStyleName );
    ~KWFrameStyleManager();

protected:
    void addStyles(const Q3PtrList<KWFrameStyle> & style );
    void setupWidget();
    void addGeneralTab();
    void apply();
    void updateGUI();
    void updatePreview();
    void save();
    int frameStyleIndex( int pos );

    QTabWidget *m_tabs;
    Q3ListBox *m_stylesList;
    QLineEdit *m_nameString;
    QPushButton *m_deleteButton;
    QPushButton *m_newButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;

    Q3GroupBox *previewBox;
    KWFrameStylePreview *preview;

    KWFrameStyle *m_currentFrameStyle;
    QStringList m_styleOrder;
    Q3PtrList<KWFrameStyleListItem> m_frameStyles;
    Q3PtrList<KWFrameStyleManagerTab> m_tabsList;
    int numFrameStyles;
    bool noSignals;

    KWDocument *m_doc;

protected slots:
    virtual void slotOk();
    virtual void slotApply();
    void switchStyle();
    void switchTabs();
    void addStyle();
    void deleteStyle();
    void moveUpStyle();
    void moveDownStyle();
    void renameStyle(const QString &);
    void importFromFile();
protected:
    void addTab( KWFrameStyleManagerTab * tab );
};

class KWFrameStyleManagerTab : public QWidget {
    Q_OBJECT
public:
    KWFrameStyleManagerTab(QWidget *parent) : QWidget(parent) {};

    /** the new style which is to be displayed */
    void setStyle(KWFrameStyle *style) { m_style = style; }
    /**  update the GUI from the current Style*/
    virtual void update() = 0;
    /**  return the (i18n-ed) name of the tab */
    virtual QString tabName() = 0;
    /** save the GUI to the style */
    virtual void save() = 0;
protected:
    KWFrameStyle *m_style;
};

class KWFrameStyleBackgroundTab : public KWFrameStyleManagerTab
{
    Q_OBJECT
public:
    KWFrameStyleBackgroundTab( QWidget * parent );

    virtual void update();
    virtual void save();
    virtual QString tabName();
protected slots:
    void updateBrushConfiguration( const QColor & );
protected:
    QBrush m_backgroundColor;
    QWidget *bgwidget;
    Q3GridLayout *grid;
    KColorButton *brushColor;
    KWBrushStylePreview *brushPreview;
};

class KWFrameStyleBordersTab : public KWFrameStyleManagerTab
{
    Q_OBJECT
public:
    KWFrameStyleBordersTab( QWidget * parent );
    ~KWFrameStyleBordersTab();
    void setWidget( KoParagLayoutWidget * widget );

    virtual void update();
    virtual void save();
    virtual QString tabName();
protected:
    virtual void resizeEvent( QResizeEvent *e );

    KoParagLayoutWidget * m_widget;
    KoParagLayout *m_borders; // Pity that I'm using such an overdosis for just borders :-(
};

#endif
