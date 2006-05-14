/*
  This file is part of KDE KOffice project.

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

#include <qbitmap.h>
#include <qcursor.h>
#include <qdrawutil.h>
#include <qfontmetrics.h>
#include <q3frame.h>
#include <QLabel>
#include <qobject.h>
#include <qpainter.h>
#include <q3ptrlist.h>
#include <qstyle.h>
#include <QToolTip>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <QPixmap>
#include <QEvent>
#include <Q3ValueList>
#include <QResizeEvent>
#include <QMouseEvent>


#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <kvbox.h>

#include "iconsidepane.h"

EntryItem::EntryItem( Navigator *parent, int _id, const QString &_text, const QString & _pix )
  : Q3ListBoxItem( parent ),
    mPixmapName(_pix),
    mId(_id),
    mHasHover( false ),
    mPaintActive( false )
{
  reloadPixmap();
  setCustomHighlighting( true );
  setText( _text  );
}

EntryItem::~EntryItem()
{
}

void EntryItem::setNewText(const QString &_text)
{
    setText( _text );
}

void EntryItem::reloadPixmap()
{
  int size = (int)navigator()->viewMode();
  if ( size != 0 )
    mPixmap = KGlobal::iconLoader()->loadIcon( mPixmapName, K3Icon::Desktop, size );
  else
    mPixmap = QPixmap();
}

Navigator* EntryItem::navigator() const
{
  return static_cast<Navigator*>( listBox() );
}

int EntryItem::width( const Q3ListBox *listbox ) const
{
  int w = 0;
  if( navigator()->showIcons() ) {
    w = navigator()->viewMode();
    if ( navigator()->viewMode() == SmallIcons )
      w += 4;
  }
  if( navigator()->showText() ) {
    if ( navigator()->viewMode() == SmallIcons )
      w += listbox->fontMetrics().width( text() );
    else
      w = qMax( w, listbox->fontMetrics().width( text() ) );
  }
  return w + ( KDialog::marginHint() * 2 );
}

int EntryItem::height( const Q3ListBox *listbox ) const
{
  int h = 0;
  if ( navigator()->showIcons() )
    h = (int)navigator()->viewMode() + 4;
  if ( navigator()->showText() ) {
    if ( navigator()->viewMode() == SmallIcons || !navigator()->showIcons() )
      h = qMax( h, listbox->fontMetrics().lineSpacing() ) + KDialog::spacingHint() * 2;
    else
      h = (int)navigator()->viewMode() + listbox->fontMetrics().lineSpacing() + 4;
  }
  return h;
}

void EntryItem::paint( QPainter *p )
{
  reloadPixmap();

  Q3ListBox *box = listBox();
  bool iconAboveText = ( navigator()->viewMode() > SmallIcons )
                     && navigator()->showIcons();
  int w = box->viewport()->width();
  int y = 2;

  // draw selected
   if ( isCurrent() || isSelected() || mHasHover || mPaintActive ) {
    int h = height( box );

    QBrush brush;
    if ( isCurrent() || isSelected() || mPaintActive )
      brush = box->colorGroup().brush( QColorGroup::Highlight );
    else
      brush = box->colorGroup().highlight().light( 115 );
    p->fillRect( 1, 0, w - 2, h - 1, brush );
    QPen pen = p->pen();
    QPen oldPen = pen;
    pen.setColor( box->colorGroup().mid() );
    p->setPen( pen );

    p->drawPoint( 1, 0 );
    p->drawPoint( 1, h - 2 );
    p->drawPoint( w - 2, 0 );
    p->drawPoint( w - 2, h - 2 );

    p->setPen( oldPen );
  }

  if ( !mPixmap.isNull() && navigator()->showIcons() ) {
      int x = iconAboveText ? ( ( w - mPixmap.width() ) / 2 ) :
                              KDialog::marginHint();
    p->drawPixmap( x, y, mPixmap );
  }

  QColor shadowColor = listBox()->colorGroup().background().dark(115);
  if ( isCurrent() || isSelected() ) {
    p->setPen( box->colorGroup().highlightedText() );
  }

  if ( !text().isEmpty() && navigator()->showText() ) {
    QFontMetrics fm = p->fontMetrics();

    int x = 0;
    if ( iconAboveText ) {
      x = ( w - fm.width( text() ) ) / 2;
      y += fm.height() - fm.descent();
      if ( navigator()->showIcons() )
        y += mPixmap.height();
    } else {
      x = KDialog::marginHint() + 4;
      if( navigator()->showIcons() ) {
        x += mPixmap.width();
      }

      if ( !navigator()->showIcons() || mPixmap.height() < fm.height() )
        y += fm.ascent() + fm.leading()/2;
      else
        y += mPixmap.height()/2 - fm.height()/2 + fm.ascent();
    }

    if ( isCurrent() || isSelected() || mHasHover ) {
      p->setPen( box->colorGroup().highlight().dark(115) );
      p->drawText( x + ( QApplication::isRightToLeft() ? -1 : 1),
                   y + 1, text() );
      p->setPen( box->colorGroup().highlightedText() );
    }
    else
      p->setPen( box->colorGroup().text() );

    p->drawText( x, y, text() );
  }

  // ensure that we don't have a stale flag around
  if (  isCurrent() || isSelected() ) mHasHover = false;
}

void EntryItem::setHover( bool hasHover )
{
  mHasHover = hasHover;
}

void EntryItem::setPaintActive( bool paintActive )
{
  mPaintActive = paintActive;
}

// ************************************************

Navigator::Navigator(bool _selectable, KMenu * menu, IconSidePane *_iconsidepane, QWidget *parent, const char *name )
  : KListBox( parent, name ), mSidePane( _iconsidepane ), mPopupMenu( menu )
{
  setSelectionMode( KListBox::Single );
  viewport()->setBackgroundMode( Qt::PaletteBackground );
  setFrameStyle( Q3Frame::NoFrame );
  setHScrollBarMode( Q3ScrollView::AlwaysOff );
  //setAcceptDrops( true );
  mMinWidth = 0;
  mSelectable = _selectable;
  executedItem = 0;
  mMouseOn = 0;

  setFocusPolicy( Qt::NoFocus );

  connect( this, SIGNAL( clicked( Q3ListBoxItem* ) ),
           SLOT( slotExecuted( Q3ListBoxItem* ) ) );
  connect( this, SIGNAL( onItem( Q3ListBoxItem * ) ),
            SLOT(  slotMouseOn( Q3ListBoxItem * ) ) );
  connect( this, SIGNAL( onViewport() ), SLOT(  slotMouseOff() ) );

  QToolTip::remove( this );
  if ( !mSidePane->showText() )
    new EntryItemToolTip( this );
}

IconViewMode Navigator::viewMode()
{
  return mSidePane->viewMode();
}

bool Navigator::showText()
{
  return mSidePane->showText();
}

bool Navigator::showIcons()
{
  return mSidePane->showIcons();
}

void Navigator::mouseReleaseEvent(QMouseEvent *e)
{
  KListBox::mouseReleaseEvent(e);
  if ( e->button() != Qt::LeftButton || !mLeftMouseButtonPressed )
    return;
  if ( itemAt( e->pos() ) && executedItem == selectedItem() )
    emit itemSelected( currentItem() );
   if ( !mSelectable )
     clearSelection();
}

void Navigator::mousePressEvent(QMouseEvent *e)
{
  if ( e->button() != Qt::LeftButton || itemAt( e->pos() ) == 0 )
  {
    mLeftMouseButtonPressed = false;
    if (e->button() == Qt::RightButton)
      slotShowRMBMenu( 0,mapToGlobal( e->pos() ) );
    return;
  }
  else
    mLeftMouseButtonPressed = true;
  KListBox::mousePressEvent(e);
}

void Navigator::enterEvent( QEvent *event )
{
  // work around Qt behaviour: onItem is not emmitted in enterEvent()
  KListBox::enterEvent( event );
  emit onItem( itemAt( mapFromGlobal( QCursor::pos() ) ) );
}

void Navigator::slotExecuted( Q3ListBoxItem *item )
{
   if ( !item )
     return;
   executedItem = item;
}

QSize Navigator::sizeHint() const
{
  return QSize( mMinWidth, 100 );
}

void Navigator::calculateMinWidth()
{
  mMinWidth = mSidePane->minWidth();

  for (EntryItem *item = static_cast<EntryItem *>(firstItem()) ; item; item = static_cast<EntryItem *>(item->next()))
  {
    if (item->width( this ) > mMinWidth)
      mMinWidth = item->width( this );
  }
  //kDebug() << "minWidth:" << mMinWidth << endl;
  parentWidget()->setFixedWidth( mMinWidth );
  triggerUpdate(true);
}

int Navigator::insertItem(const QString &_text, const QString & _pix)
{
  EntryItem *item = new EntryItem( this, count(), _text, _pix );
  if (item->width( this ) > mSidePane->minWidth() )
  {
    mMinWidth = item->width( this );
    parentWidget()->setMinimumWidth( mMinWidth );
    //kDebug() << "minWidth:" << mMinWidth << endl;
  }
  return item->id();
}

void Navigator::setHoverItem( Q3ListBoxItem* item, bool hover )
{
    static_cast<EntryItem*>( item )->setHover( hover );
    updateItem( item );
}

void Navigator::setPaintActiveItem( Q3ListBoxItem* item, bool paintActive )
{
    static_cast<EntryItem*>( item )->setPaintActive( paintActive );
    updateItem( item );
}

void Navigator::slotMouseOn( Q3ListBoxItem* newItem )
{
    Q3ListBoxItem* oldItem = mMouseOn;
    if ( oldItem == newItem )
      return;

    if ( oldItem && !oldItem->isCurrent() && !oldItem->isSelected() )
        setHoverItem( oldItem, false );

    if ( newItem && !newItem->isCurrent() && !newItem->isSelected() )
        setHoverItem( newItem, true );
    mMouseOn = newItem;
}

void Navigator::slotMouseOff()
{
    slotMouseOn( 0 );
}

void Navigator::resizeEvent( QResizeEvent *event )
{
  Q3ListBox::resizeEvent( event );
  triggerUpdate( true );
}

void Navigator::slotShowRMBMenu( Q3ListBoxItem *, const QPoint &pos )
{
  QAction *choice = mPopupMenu->exec( pos );

  if ( !choice  )
    return;
  mSidePane->resetWidth();
  mSidePane->changeStateMenu( choice);
  calculateMinWidth();
  emit updateAllWidgets();
}

// ************************************************

IconSidePane::IconSidePane(QWidget *parent, const char *name )
  : KVBox( parent )
{
  setObjectName(name);
  m_buttongroup = new Q3ButtonGroup(1, Qt::Horizontal, this);
  m_buttongroup->setExclusive(true);
  m_buttongroup->hide();
  mWidgetstack = new QStackedWidget(this);
  mWidgetstack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  // setup the popup menu
  m_bShowIcons = KoShellSettings::sidePaneShowIcons();
  m_bShowText = KoShellSettings::sidePaneShowText();
  mViewMode = sizeIntToEnum( KoShellSettings::sidePaneIconSize() );
  mPopupMenu = new KMenu(0);
  mPopupMenu->setTitle( i18n( "Icon Size" ) );

  mLargeIcons = mPopupMenu->addAction(i18n( "Large" ));
  mLargeIcons->setEnabled( m_bShowIcons);

  mNormalIcons = mPopupMenu->addAction(i18n( "Normal" ));
  mNormalIcons->setEnabled(m_bShowIcons);

  mSmallIcons = mPopupMenu->addAction(i18n("Small"));
  mSmallIcons->setEnabled(m_bShowIcons);
  switch(KoShellSettings::sidePaneIconSize())
  {
    case LargeIcons:
      mLargeIcons->setChecked(true);
      break;
    case NormalIcons:
      mNormalIcons->setChecked(true);
      break;
    case SmallIcons:
      mSmallIcons->setChecked(true);
      break;
  }

  mPopupMenu->insertSeparator();

  mShowIcons = mPopupMenu->addAction(i18n( "Show Icons" ));
  mShowIcons->setEnabled(m_bShowText);
  mShowIcons->setChecked(mShowIcons);
  mShowText = mPopupMenu->addAction(i18n( "Show Text" ));
  mShowText->setEnabled(m_bShowIcons);
  mShowText->setChecked(mShowText);

  if ( !mShowText )
    m_buttongroup->hide();
}

IconSidePane::~IconSidePane()
{
}

void IconSidePane::changeStateMenu( QAction *choice)
{
  if(choice == mSmallIcons )
  {
    mSmallIcons->setChecked(true);
    mNormalIcons->setChecked(false);
    mLargeIcons->setChecked(false);
    KoShellSettings::setSidePaneIconSize( (int)SmallIcons );
  }
  else if(choice == mNormalIcons )
  {
    mSmallIcons->setChecked(false);
    mNormalIcons->setChecked(true);
    mLargeIcons->setChecked(false);
    KoShellSettings::setSidePaneIconSize( (int)NormalIcons );
  }
  else if(choice == mLargeIcons )
  {
    mSmallIcons->setChecked(false);
    mNormalIcons->setChecked(false);
    mLargeIcons->setChecked(true);
    KoShellSettings::setSidePaneIconSize((int)LargeIcons);
   }
  else if(choice == mShowIcons)
  {
    mSmallIcons->setEnabled(showIcons());
    mNormalIcons->setEnabled(showIcons());
    mLargeIcons->setEnabled(showIcons());
    mShowText->setEnabled(showIcons());
    mShowIcons->setChecked(showIcons());
    toogleIcons();
    QToolTip::remove( mCurrentNavigator );
  }
  else if(choice == mShowText )
  {
    toogleText();
    resetWidth();
    mSmallIcons->setEnabled(true);
    mNormalIcons->setEnabled(true);
    mLargeIcons->setEnabled(true);
    mShowIcons->setEnabled(showText());
    mShowText->setChecked(showText());
    new EntryItemToolTip( mCurrentNavigator );
    KoShellSettings::setSidePaneShowText( showText() );
  }
}


int IconSidePane::insertItem(int _grp, const QString & _pix, const QString &_text)
{
  return static_cast<Navigator*>( mWidgetstack->widget(_grp))->insertItem( _text, _pix );
}

int IconSidePane::insertItem(const QString & _pix, const QString &_text)
{
  return mCurrentNavigator->insertItem(_text, _pix);
}

void IconSidePane::renameItem( int _grp, int _id, const QString & _text )
{
  Navigator *navigator = static_cast<Navigator*>(mWidgetstack->widget(_grp));
  if (!navigator)
    return;
  EntryItem *item = 0;
  for (uint i=0; i< navigator->count(); i++)
  {
    item = static_cast<EntryItem *>(navigator->item(i));
    if (_id == item->id())
    {
      item->setNewText(_text);
      navigator->triggerUpdate(false);
      break;
    }
  }
}

void IconSidePane::removeItem( int _grp, int _id )
{
  Navigator *navigator = static_cast<Navigator*>(mWidgetstack->widget(_grp));
  if (!navigator)
    return;
  for (uint i=0; i< navigator->count(); i++)
  {
    if (_id == static_cast<EntryItem *>(navigator->item(i))->id())
    {
      navigator->removeItem(i);
      break;
    }
  }
}

int IconSidePane::insertGroup(const QString &_text, bool _selectable, QObject *_obj, const char *_slot)
{
  mCurrentNavigator = new Navigator(_selectable, mPopupMenu, this, mWidgetstack );
  //connect( mCurrentNavigator, SIGNAL( itemSelected( int ) ), this, SLOT( itemSelected( int ) ) );
  if ( _obj != 0L && _slot != 0L )
    connect( mCurrentNavigator, SIGNAL( itemSelected(int ) ), _obj, _slot );
  connect( mCurrentNavigator, SIGNAL( updateAllWidgets() ), this, SLOT(updateAllWidgets()) );
  int const id = mWidgetstack->addWidget(mCurrentNavigator);
  mWidgetStackIds.append( id );
  KPushButton *b = new KPushButton( _text, m_buttongroup );
  m_buttongroup->insert( b, id );
  connect( b, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
  b->setToggleButton( true );
  b->setFocusPolicy( Qt::NoFocus );
  if (m_buttongroup->count()==1)
  {
    mCurrentNavigator->calculateMinWidth();
    m_buttongroup->setButton(m_buttongroup->id(b));
    mWidgetstack->setCurrentIndex(id);
  }
  if ( b->width() > minimumWidth() )
    setMinimumWidth( b->width() );
  return id;
}

void IconSidePane::buttonClicked()
{
    mWidgetstack->setCurrentIndex( m_buttongroup->selectedId() );
}

void IconSidePane::selectGroup(int group_id)
{
    mWidgetstack->setCurrentIndex(group_id);
}

void IconSidePane::itemSelected(int item)
{
  kDebug() << "Item selected:" << item << endl;
}

Navigator * IconSidePane::group(int _grp)
{
  return static_cast<Navigator*>(mWidgetstack->widget(_grp));
}

void IconSidePane::updateAllWidgets()
{
  QList<int>::iterator it;
  for ( it = mWidgetStackIds.begin(); it != mWidgetStackIds.end(); ++it )
    static_cast<Navigator*>(mWidgetstack->widget(*it))->triggerUpdate( true );
}

int IconSidePane::minWidth()
{
  int width = 0;
  QList<int>::iterator it;
  Navigator *n;
  for ( it = mWidgetStackIds.begin(); it != mWidgetStackIds.end(); ++it )
  {
    n = static_cast<Navigator*>(mWidgetstack->widget(*it));
    if ( n->minWidth() > width )
        width = n->minWidth();
  }
  return width;
}

void IconSidePane::resetWidth()
{
  QList<int>::iterator it;
  Navigator *n;
  for ( it = mWidgetStackIds.begin(); it != mWidgetStackIds.end(); ++it )
  {
    n = static_cast<Navigator*>(mWidgetstack->widget(*it));
    n->resetWidth();
    n->triggerUpdate(true);
  }
}

IconViewMode IconSidePane::sizeIntToEnum(int size) const
{
  switch ( size ) {
    case int(LargeIcons):
      return LargeIcons;
      break;
    case int(NormalIcons):
      return NormalIcons;
      break;
    case int(SmallIcons):
      return SmallIcons;
      break;
    default:
      // Stick with sane values
      return NormalIcons;
      kDebug() << "View mode not implemented!" << endl;
      break;
  }
}

void IconSidePane::setActionCollection( KActionCollection *actionCollection )
{
  mActionCollection = actionCollection;
}

KActionCollection *IconSidePane::actionCollection() const
{
  return mActionCollection;
}

#include "iconsidepane.moc"

// vim: sw=2 sts=2 et tw=80
