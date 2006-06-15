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

#ifndef kwtablestylemanager_h
#define kwtablestylemanager_h

#include "KWFrameStyle.h"
#include "KWTableStyle.h"

#include <kdialog.h>
#include <q3groupbox.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>


class Q3GridLayout;
class QLineEdit;
class Q3ListBox;
class QPushButton;
class QWidget;
class Q3Frame;
class QComboBox;

class KWDocument;
class KoParagStyle;

class KoTextDocument;

/******************************************************************/
/* Class: KWTableStylePreview                                     */
/******************************************************************/

class KWTableStylePreview : public Q3GroupBox
{
    Q_OBJECT

public:
    KWTableStylePreview(const QString &title, const QString &text, QWidget *parent, const char* name = 0);
    virtual ~KWTableStylePreview();

    void setTableStyle(KWTableStyle *_tableStyle);

protected:
    void drawContents( QPainter *p );

    KWTableStyle *tableStyle;

    KoTextDocument *m_textdoc;
    KoTextZoomHandler *m_zoomHandler;
};

class KWTableStyleListItem
{
  public:
    KWTableStyleListItem() {}
    ~KWTableStyleListItem();
    KWTableStyleListItem(KWTableStyle *orig, KWTableStyle *changed) {
        m_origTableStyle = orig;
        m_changedTableStyle = changed;
    }

    KWTableStyle *origTableStyle()const { return m_origTableStyle; }
    KWTableStyle *changedTableStyle()const { return m_changedTableStyle; }
    void setOrigTableStyle( KWTableStyle *_orig ) { m_origTableStyle = _orig; }
    void setChangedTableStyle( KWTableStyle *_changed ) { m_changedTableStyle = _changed; }
    void deleteOrigTableStyle() { delete m_changedTableStyle; }
    void deleteChangedTableStyle() { delete m_changedTableStyle; }

    void switchStyle();
    void deleteStyle( KWTableStyle *current );
    void apply();

  protected:
    KWTableStyle *m_origTableStyle;
    KWTableStyle *m_changedTableStyle;
};

/******************************************************************/
/* Class: KWTableStyleManager                                     */
/******************************************************************/
class KWTableStyleManager : public KDialog
{
    Q_OBJECT

public:
    KWTableStyleManager( QWidget *_parent, KWDocument *_doc );
    ~KWTableStyleManager();

private:
    void setupWidget();
    void addGeneralTab();
    void apply();
    void updateGUI();
    void updatePreview();
    void save();
    int tableStyleIndex( int pos );

    void addStyles(const Q3PtrList<KWTableStyle> &listStyle );

    Q3ListBox *m_stylesList;
    QLineEdit *m_nameString;
    QPushButton *m_deleteButton;
    QPushButton *m_newButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    QComboBox *m_frameStyle;
    QComboBox *m_style;
    QPushButton *m_changeFrameStyleButton;
    QPushButton *m_changeStyleButton;
    QStringList m_styleOrder;
    Q3GroupBox *previewBox;
    KWTableStylePreview *preview;

    QWidget *main;

    KWTableStyle *m_currentTableStyle;
    Q3PtrList<KWTableStyleListItem> m_tableStyles;
    int numTableStyles;
    bool noSignals;

    KWDocument *m_doc;

protected slots:
    virtual void slotOk();
    virtual void slotApply();
    void updateAllStyleCombos();
    void changeFrameStyle();
    void changeStyle();
    void selectFrameStyle(int);
    void selectStyle(int);
    void switchStyle();
    void addStyle();
    void deleteStyle();
    void moveUpStyle();
    void moveDownStyle();
    void renameStyle(const QString &);
    void setupMain();
    void importFromFile();
};

#endif
