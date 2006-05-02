/*
  This file is part of the KDE KOffice project.

  Copyright (C) 2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2005 Fredrik Edemar <f_edemar@linux.se>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KOFFICE_ICONSIDEPANEBASE_H
#define KOFFICE_ICONSIDEPANEBASE_H

#include <kactioncollection.h>
#include <klistbox.h>

#include <q3buttongroup.h> 
#include <qlayout.h>
#include <qtooltip.h>

#include <QStackedWidget>
//Added by qt3to4:
#include <QPixmap>
#include <QEvent>
#include <Q3ValueList>
#include <QResizeEvent>
#include <QMouseEvent>
#include <kvbox.h>

#include "koshellsettings.h"


class Navigator;
class IconSidePane;

enum IconViewMode { LargeIcons = 48, NormalIcons = 32, SmallIcons = 22, ShowText = 3, ShowIcons = 5 };

/**
  A @see QListBoxPixmap Square Box with an optional icon and a text
  underneath.
*/
class EntryItem : public Q3ListBoxItem
{
  public:
    EntryItem( Navigator *, int _id, const QString &_text, const QString & _pix );
    ~EntryItem();


    const QPixmap *pixmap() const { return &mPixmap; }

    int id() { return mId; }

    void setNewText( const QString &_text );

    void setHover( bool );
    void setPaintActive( bool );
    bool paintActive() const { return mPaintActive; }

    /**
      returns the width of this item.
    */
    virtual int width( const Q3ListBox * ) const;
    /**
      returns the height of this item.
    */
    virtual int height( const Q3ListBox * ) const;

  protected:
    void reloadPixmap();

    virtual void paint( QPainter *p );

  private:
    QPixmap mPixmap;
    QString mPixmapName;
    int const mId;
    Navigator* navigator() const;
    bool mHasHover;
    bool mPaintActive;
};

/**
 * Tooltip that changes text depending on the item it is above.
 * Compliments of "Practical Qt" by Dalheimer, Petersen et al.
 */
class EntryItemToolTip : public QWidget
{
Q_OBJECT
  public:
    EntryItemToolTip( Q3ListBox* parent )
      : mListBox( parent ) 
      {}
  protected:
	bool event(QEvent *event) {
			if (event->type() == QEvent::ToolTip) {
					QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
					if ( KoShellSettings::sidePaneShowText() ) return false;
					if ( !mListBox ) return false;
					Q3ListBoxItem* item = mListBox->itemAt( helpEvent->pos() );
					if ( !item ) return false;
					const QRect itemRect = mListBox->itemRect( item );
					if ( !itemRect.isValid() ) return false;
					const EntryItem *entryItem = static_cast<EntryItem*>( item );
					QString tipStr = entryItem->text();
					QToolTip::showText ( helpEvent->pos(), tipStr ); 		
					return true;
			}
			return false;
	}
  private:
    Q3ListBox* mListBox;
};

/**
  Navigation pane showing all parts relevant to the user
*/
class Navigator : public KListBox
{
    Q_OBJECT
  public:
    Navigator(bool _selectable, KMenu*, IconSidePane *, QWidget *parent = 0, const char *name = 0 );

    int insertItem(const QString &_text, const QString &_pix);

    QSize sizeHint() const;
    IconViewMode viewMode();
    bool showText();
    bool showIcons();
    void calculateMinWidth();
    bool leftMouseButtonPressed() const {return mLeftMouseButtonPressed;}
    int minWidth() const { return mMinWidth; }
    void resetWidth() { mMinWidth = 0; }

  signals:
    void itemSelected( int );
    void updateAllWidgets();

  protected:
    void resizeEvent( QResizeEvent * );
    void enterEvent( QEvent* );
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

    void setHoverItem( Q3ListBoxItem*, bool );
    void setPaintActiveItem( Q3ListBoxItem*, bool );

  protected slots:
    void slotExecuted( Q3ListBoxItem * );
    void slotMouseOn( Q3ListBoxItem *item );
    void slotMouseOff();
    void slotShowRMBMenu( Q3ListBoxItem *, const QPoint& );

  private:
    IconSidePane *mSidePane;
    int mMinWidth;
    Q3ListBoxItem *executedItem;
    bool mLeftMouseButtonPressed;
    KMenu *mPopupMenu;
    bool mSelectable;
    Q3ListBoxItem* mMouseOn;
};

class IconSidePane :public KVBox
{
    Q_OBJECT
  public:
    IconSidePane( QWidget *parent, const char *name = 0 );
    ~IconSidePane();

    void setActionCollection( KActionCollection *actionCollection );
    KActionCollection *actionCollection() const;

    int insertItem(const QString & _pix, const QString &_text);
    int insertItem(int _grp, const QString & _pix, const QString &_text);
    int insertGroup(const QString &_text, bool _selectable, QObject *_obj = 0L, const char *_slot = 0L);
    void renameItem( int _grp, int _id, const QString & _text );
    void removeItem( int _grp, int _id );
    void selectGroup(int);
    Navigator *group(int);

    IconViewMode sizeIntToEnum(int size) const;
    IconViewMode viewMode() const { return mViewMode; }
    void setViewMode(int choice){mViewMode = sizeIntToEnum(choice);}
    bool showText() const { return m_bShowText; }
    void toogleText(){m_bShowText=!m_bShowText;}
    bool showIcons() const { return m_bShowIcons; }
    void toogleIcons(){m_bShowIcons=!m_bShowIcons;}
    Q3ButtonGroup *buttonGroup() { return m_buttongroup; }
    int minWidth();
    void resetWidth();
    void changeStateMenu( QAction *_act);

  public slots:
    void itemSelected(int);
    void updateAllWidgets();

  private slots:
    void buttonClicked();

  private:
    QStackedWidget *mWidgetstack;
    Q3ValueList<int> mWidgetStackIds;
    Navigator *mCurrentNavigator;
    Q3ButtonGroup *m_buttongroup;
    KActionCollection *mActionCollection;
    KMenu *mPopupMenu;

    IconViewMode mViewMode;
    bool m_bShowIcons;
    bool m_bShowText;
    QAction *mLargeIcons, *mNormalIcons, *mSmallIcons, *mShowText, *mShowIcons;
};


#endif
