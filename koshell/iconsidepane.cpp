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
#include <qframe.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qstyle.h>
#include <qtooltip.h>
#include <qwidgetstack.h>


#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kpushbutton.h>

#include "iconsidepane.h"

EntryItem::EntryItem( Navigator *parent, int _id, const QString &_text, const QString & _pix )
  : QListBoxItem( parent ),
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
    mPixmap = KGlobal::iconLoader()->loadIcon( mPixmapName, KIcon::Desktop, size );
  else
    mPixmap = QPixmap();
}

Navigator* EntryItem::navigator() const
{
  return static_cast<Navigator*>( listBox() );
}

int EntryItem::width( const QListBox *listbox ) const
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
      w = QMAX( w, listbox->fontMetrics().width( text() ) );
  }
  return w + ( KDialog::marginHint() * 2 );
}

int EntryItem::height( const QListBox *listbox ) const
{
  int h = 0;
  if ( navigator()->showIcons() )
    h = (int)navigator()->viewMode() + 4;
  if ( navigator()->showText() ) {
    if ( navigator()->viewMode() == SmallIcons || !navigator()->showIcons() )
      h = QMAX( h, listbox->fontMetrics().lineSpacing() ) + KDialog::spacingHint() * 2;
    else
      h = (int)navigator()->viewMode() + listbox->fontMetrics().lineSpacing() + 4;
  }
  return h;
}

void EntryItem::paint( QPainter *p )
{
  reloadPixmap();

  QListBox *box = listBox();
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
      p->drawText( x + ( QApplication::reverseLayout() ? -1 : 1),
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

Navigator::Navigator(bool _selectable, KPopupMenu * menu, IconSidePane *_iconsidepane, QWidget *parent, const char *name )
  : KListBox( parent, name ), mSidePane( _iconsidepane ), mPopupMenu( menu )
{
  setSelectionMode( KListBox::Single );
  viewport()->setBackgroundMode( PaletteBackground );
  setFrameStyle( QFrame::NoFrame );
  setHScrollBarMode( QScrollView::AlwaysOff );
  //setAcceptDrops( true );
  mMinWidth = 0;
  mSelectable = _selectable;
  executedItem = 0;
  mMouseOn = 0;

  setFocusPolicy( NoFocus );

  connect( this, SIGNAL( clicked( QListBoxItem* ) ),
           SLOT( slotExecuted( QListBoxItem* ) ) );
  connect( this, SIGNAL( onItem( QListBoxItem * ) ),
            SLOT(  slotMouseOn( QListBoxItem * ) ) );
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
  if ( e->button() != LeftButton || !mLeftMouseButtonPressed )
    return;
  if ( itemAt( e->pos() ) && executedItem == selectedItem() )
    emit itemSelected( currentItem() );
   if ( !mSelectable )
     clearSelection();
}

void Navigator::mousePressEvent(QMouseEvent *e)
{
  if ( e->button() != LeftButton || itemAt( e->pos() ) == 0 )
  {
    mLeftMouseButtonPressed = false;
    if (e->button() == RightButton)
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

void Navigator::slotExecuted( QListBoxItem *item )
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
  //kdDebug() << "minWidth:" << mMinWidth << endl;
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
    //kdDebug() << "minWidth:" << mMinWidth << endl;
  }
  return item->id();
}

void Navigator::setHoverItem( QListBoxItem* item, bool hover )
{
    static_cast<EntryItem*>( item )->setHover( hover );
    updateItem( item );
}

void Navigator::setPaintActiveItem( QListBoxItem* item, bool paintActive )
{
    static_cast<EntryItem*>( item )->setPaintActive( paintActive );
    updateItem( item );
}

void Navigator::slotMouseOn( QListBoxItem* newItem )
{
    QListBoxItem* oldItem = mMouseOn;
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
  QListBox::resizeEvent( event );
  triggerUpdate( true );
}

void Navigator::slotShowRMBMenu( QListBoxItem *, const QPoint &pos )
{
  int choice = mPopupMenu->exec( pos );

  if ( choice == -1 )
    return;

  mSidePane->resetWidth();
  if ( choice >= SmallIcons ) {
    mSidePane->setViewMode( mSidePane->sizeIntToEnum( choice ) );
    mPopupMenu->setItemChecked( (int)SmallIcons, false);
    mPopupMenu->setItemChecked( (int)NormalIcons, false);
    mPopupMenu->setItemChecked( (int)LargeIcons, false);
    mPopupMenu->setItemChecked( mSidePane->viewMode(), true);
    KoShellSettings::setSidePaneIconSize( choice );
  }
  else
  {
    // either icons or text were toggled
    if ( choice == ShowIcons ) {
        mSidePane->toogleIcons();
        mPopupMenu->setItemChecked( (int)ShowIcons,  mSidePane->showIcons() );
        mPopupMenu->setItemEnabled( (int)ShowText,  mSidePane->showIcons() );
        mPopupMenu->setItemEnabled( (int)SmallIcons, mSidePane->showIcons());
        mPopupMenu->setItemEnabled( (int)NormalIcons, mSidePane->showIcons());
        mPopupMenu->setItemEnabled( (int)LargeIcons, mSidePane->showIcons());
        KoShellSettings::setSidePaneShowIcons( mSidePane->showIcons() );

        QToolTip::remove( this );
    } else {
        mSidePane->toogleText();
        mSidePane->resetWidth();
        mPopupMenu->setItemChecked( (int)ShowText,  mSidePane->showText() );
        mPopupMenu->setItemEnabled( (int)ShowIcons,  mSidePane->showText() );
        mPopupMenu->setItemEnabled( (int)SmallIcons, true);
        mPopupMenu->setItemEnabled( (int)NormalIcons, true);
        mPopupMenu->setItemEnabled( (int)LargeIcons, true);
        KoShellSettings::setSidePaneShowText( mSidePane->showText() );
        new EntryItemToolTip( this );
//         if ( !mSidePane->showText() )
//           mSidePane->buttonGroup()->hide();
//         else
//           mSidePane->buttonGroup()->show();
    }
  }
  calculateMinWidth();
  emit updateAllWidgets();
}

// ************************************************

IconSidePane::IconSidePane(QWidget *parent, const char *name )
  : QVBox( parent, name )
{
  m_buttongroup = new QButtonGroup(1, QGroupBox::Horizontal, this);
  m_buttongroup->setExclusive(true);
  m_buttongroup->hide();
  mWidgetstack = new QWidgetStack(this);
  mWidgetstack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  
  // setup the popup menu
  mShowIcons = KoShellSettings::sidePaneShowIcons();
  mShowText = KoShellSettings::sidePaneShowText();
  mViewMode = sizeIntToEnum( KoShellSettings::sidePaneIconSize() );
  mPopupMenu = new KPopupMenu(0);
  mPopupMenu->insertTitle( i18n( "Icon Size" ) );
  mPopupMenu->insertItem( i18n( "Large" ), (int)LargeIcons );
  mPopupMenu->setItemEnabled( (int)LargeIcons, mShowIcons );
  mPopupMenu->insertItem( i18n( "Normal" ), (int)NormalIcons );
  mPopupMenu->setItemEnabled( (int)NormalIcons, mShowIcons );
  mPopupMenu->insertItem( i18n( "Small" ), (int)SmallIcons );
  mPopupMenu->setItemEnabled( (int)SmallIcons, mShowIcons );
  mPopupMenu->setItemChecked( (int)mViewMode, true );
  mPopupMenu->insertSeparator();
  mPopupMenu->insertItem( i18n( "Show Icons" ), (int)ShowIcons );
  mPopupMenu->setItemChecked( (int)ShowIcons, mShowIcons );
  mPopupMenu->setItemEnabled( (int)ShowIcons, mShowText );
  mPopupMenu->insertItem( i18n( "Show Text" ), (int)ShowText );
  mPopupMenu->setItemChecked( (int)ShowText, mShowText );
  mPopupMenu->setItemEnabled( (int)ShowText, mShowIcons );
  if ( !mShowText )
    m_buttongroup->hide();
}

IconSidePane::~IconSidePane()
{
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
  b->setFocusPolicy( NoFocus );
  if (m_buttongroup->count()==1)
  {
    mCurrentNavigator->calculateMinWidth();
    m_buttongroup->setButton(m_buttongroup->id(b));
    mWidgetstack->raiseWidget(id);
  }
  if ( b->width() > minimumWidth() )
    setMinimumWidth( b->width() );
  return id;
}

void IconSidePane::buttonClicked()
{
    mWidgetstack->raiseWidget( m_buttongroup->selectedId() );
}

void IconSidePane::selectGroup(int group_id)
{
    mWidgetstack->raiseWidget(group_id);
}

void IconSidePane::itemSelected(int item)
{
  kdDebug() << "Item selected:" << item << endl;
}

Navigator * IconSidePane::group(int _grp)
{
  return static_cast<Navigator*>(mWidgetstack->widget(_grp));
}

void IconSidePane::updateAllWidgets()
{
  QValueList<int>::iterator it;
  for ( it = mWidgetStackIds.begin(); it != mWidgetStackIds.end(); ++it )
    static_cast<Navigator*>(mWidgetstack->widget(*it))->triggerUpdate( true );
}

int IconSidePane::minWidth()
{
  int width = 0;
  QValueList<int>::iterator it;
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
  QValueList<int>::iterator it;
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
      kdDebug() << "View mode not implemented!" << endl;
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
