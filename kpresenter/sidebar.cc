/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Lukas Tinkl <lukas@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qheader.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qtabwidget.h>
#include <qtooltip.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <knotifyclient.h>
#include <kiconview.h>
#include <kdebug.h>

#include "sidebar.h"
#include "kpresenter_view.h"
#include "kprcanvas.h"
#include <qapplication.h>
#include "kprcommand.h"

class ThumbToolTip : public QToolTip
{
public:
    ThumbToolTip(QWidget *parent)
     : QToolTip(parent)
     {}

protected:
    void maybeTip(const QPoint &pos)
    {
        QString title;
        QRect r(((ThumbBar*)parentWidget())->tip(pos, title));
        if (!r.isValid())
            return;

        tip(r, title);
    }
};

class OutlineToolTip : public QToolTip
{
public:
    OutlineToolTip(QWidget *parent)
     : QToolTip(parent)
     {}

protected:
    void maybeTip( const QPoint &pos )
    {
        QString title;
        QRect r(((Outline*)parentWidget())->tip(pos, title));
        if (!r.isValid())
            return;

        tip(r, title);
    }
};

class OutlineItem: public QCheckListItem
{
public:
    OutlineItem( QListView * parent )
     : QCheckListItem( parent, QString::null, QCheckListItem::CheckBox )
     {}

    virtual void stateChange(bool b)
    {
        static_cast<Outline*>(listView())->itemStateChange( this, b );
    }
};

SideBar::SideBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v)
  :QTabWidget(parent), doc(d), view(v)
{
  setTabPosition(QTabWidget::Top);
  setTabShape(QTabWidget::Triangular);

  _outline = new Outline(this, doc, view);
  addTab(_outline, i18n("Outline"));

  _thb = new ThumbBar(this, doc, view);
  addTab(_thb,i18n("Thumbs"));


  //TODO find a better way
  connect(_outline, SIGNAL(showPage(int)),
          this, SIGNAL(showPage(int)));

  connect(_thb, SIGNAL(showPage(int)),
          this, SIGNAL(showPage(int)));

  connect(_outline, SIGNAL(movePage(int,int)),
          this, SIGNAL(movePage(int,int)));

  connect(_outline, SIGNAL(selectPage(int,bool)),
          this, SIGNAL(selectPage(int,bool)));

  connect(this, SIGNAL(currentChanged(QWidget *)),
          this, SLOT(currentChanged(QWidget *)));

}

void SideBar::currentChanged(QWidget *tab)
{
    if (tab == _thb) {
        if (!_thb->uptodate && _thb->isVisible())
            _thb->rebuildItems();
    }
}

ThumbBar::ThumbBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v)
  :KIconView(parent), doc(d), view(v)
{
  uptodate = false;

  setArrangement(QIconView::LeftToRight);
  setAutoArrange(true);
  setSorting(false);
  setItemsMovable(false);
  setResizeMode(QIconView::Adjust);

  thumbTip = new ThumbToolTip(this);

  connect(this, SIGNAL(currentChanged(QIconViewItem *)),
          this, SLOT(itemClicked(QIconViewItem *)));
}

ThumbBar::~ThumbBar()
{
    delete thumbTip;
}

void ThumbBar::setCurrentPage( int pg )
{
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() )
    {
        if ( it->text().toInt() - 1 == pg ) {
            setCurrentItem( it );
            setSelected( it, FALSE ); // to avoid the blue "selected"-mark
            ensureItemVisible(it);
        }
    }

}

QRect ThumbBar::tip(const QPoint &pos, QString &title)
{
    QIconViewItem *item = findItem(viewportToContents(pos));
    if (!item)
        return QRect(0, 0, -1, -1);

    int pagenr =  item->index();
    title = doc->pageList().at(pagenr)->pageTitle(i18n("Slide %1").arg(pagenr + 1));

    QRect r = item->pixmapRect(FALSE);
    r = QRect(contentsToViewport(QPoint(r.x(), r.y())), QSize(r.width(), r.height()));
    return r;
}

void ThumbBar::rebuildItems()
{
    if( !isVisible())
        return;
  kdDebug(33001) << "ThumbBar::rebuildItems" << endl;

  QApplication::setOverrideCursor( Qt::waitCursor );

  clear();
  for ( unsigned int i = 0; i < doc->getPageNums(); i++ ) {
    QIconViewItem *item = new QIconViewItem(dynamic_cast<QIconView *>(this), QString::number(i+1), getSlideThumb(i));
    item->setDragEnabled(false);  //no dragging for now
  }

  uptodate = true;

  QApplication::restoreOverrideCursor();
}

QPixmap ThumbBar::getSlideThumb(int slideNr)
{
  //kdDebug(33001) << "ThumbBar::getSlideThumb: " << slideNr << endl;
  QRect rect = doc->pageList().at(slideNr)->getZoomPageRect();
  QPixmap pix( rect.size() );
  pix.fill( Qt::white );

  view->getCanvas()->drawPageInPix( pix, slideNr );

  int w = rect.width();
  int h = rect.height();

  if ( w > h ) {
      w = 130;
      h = 120;
  }
  else if ( w < h ) {
      w = 120;
      h = 130;
  }
  else if ( w == h ) {
      w = 130;
      h = 130;
  }

  const QImage img(pix.convertToImage().smoothScale( w, h, QImage::ScaleMin ));
  pix.convertFromImage(img);

  // draw a frame around the thumb to show its size
  QPainter p(&pix);
  p.setPen(Qt::black);
  p.drawRect(pix.rect());

  return pix;
}

void ThumbBar::itemClicked(QIconViewItem *i)
{
  if ( !i )
    return;
  emit showPage( i->index() );
}

Outline::Outline( QWidget *parent, KPresenterDoc *d, KPresenterView *v )
    : KListView( parent ), doc( d ), view( v )
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    addColumn( i18n( "Number" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    outlineTip = new OutlineToolTip(this);

    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( moved( QListViewItem *, QListViewItem *, QListViewItem * ) ),
             this, SLOT( movedItems( QListViewItem *, QListViewItem *, QListViewItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int ) ) );

    connect( this, SIGNAL( doubleClicked ( QListViewItem * )),
             this, SLOT(renamePageTitle()));

    setAcceptDrops( TRUE );
    setDropVisualizer( TRUE );
    setDragEnabled( TRUE );

}

Outline::~Outline()
{
    delete outlineTip;
}

void Outline::rebuildItems()
{
    clear();
    // Rebuild all the items
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        QCheckListItem *item = new OutlineItem( this );
        QString title = doc->pageList().at(i)->pageTitle( i18n( "Slide %1" ).arg( i + 1 ) );
        //kdDebug(33001) << "Outline::rebuildItems slide " << i+1 << " selected:" << doc->isSlideSelected( i ) << endl;
        item->setOn( doc->isSlideSelected( i ) ); // calls itemStateChange !
        item->setText( 1, QString::number( i + 1 ) ); // page number
        if (title.length() > 12) // restrict to a maximum of 12 characters
            item->setText(0, title.left(5) + "..." + title.right(4));
        else
            item->setText( 0, title );
    }
}

void Outline::updateItem( int pagenr /* 0-based */)
{
    // Find item
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text(1).toInt() == pagenr+1 )
        {
            QString title = doc->pageList().at(pagenr)->pageTitle( i18n( "Slide %1" ).arg( pagenr + 1 ) );
            if (title.length() > 12) // restrict to a maximum of 12 characters
                it.current()->setText( 0, title.left(5) + "..." + title.right(4));
            else
                it.current()->setText( 0, title );

            it.current()->setText( 1, QString::null ); // hack, to make itemStateChange do nothing
            static_cast<OutlineItem*>(it.current())->setOn( doc->isSlideSelected( pagenr ) );
            it.current()->setText( 1, QString::number( pagenr + 1 ) ); // page number
            return;
        }
    }
    kdWarning() << "Item for page " << pagenr << " not found" << endl;
}

void Outline::itemStateChange( OutlineItem * item, bool state )
{
    QString text = item->text( 1 );
    if ( !text.isEmpty() ) // empty if we are called from rebuildItems
        emit selectPage( text.toInt() - 1, state );
}

QRect Outline::tip(const QPoint &pos, QString &title)
{
    QListViewItem *item = itemAt( pos );
    if (!item)
        return QRect(0, 0, -1, -1);

    int pagenr = item->text(1).toInt() - 1;
    title = doc->pageList().at(pagenr)->pageTitle(i18n("Slide %1").arg(pagenr + 1));

    return itemRect(item);
}

void Outline::itemClicked( QListViewItem *i )
{
    if ( !i )
        return;
    emit showPage( i->text( 1 ).toInt() - 1 );
}


void Outline::setCurrentPage( int pg )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
        if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
            setCurrentItem( it.current() );
            setSelected( it.current(), TRUE );
            ensureItemVisible(it.current());
        }
    }
}

void Outline::setOn( int pg, bool on )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
        if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
            ( (QCheckListItem*)it.current() )->setOn( on );
            return;
        }
    }
}

void Outline::contentsDropEvent( QDropEvent *e )
{
    disconnect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    KListView::contentsDropEvent( e );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
}

void Outline::movedItems( QListViewItem *i, QListViewItem *, QListViewItem *newAfter )
{
    movedItem = i;
    movedAfter = newAfter;
    QTimer::singleShot( 300, this, SLOT( doMoveItems() ) );

}

void Outline::doMoveItems()
{
    int num = movedItem->text( 1 ).toInt() - 1;
    int numNow;
    if ( !movedAfter ) {
        numNow = 0;
    } else {
        numNow = movedAfter->text( 1 ).toInt();
        if ( numNow > num )
            numNow--;
    }
    if(num!=numNow) {
        emit movePage( num, numNow );
        setSelected( movedItem, true );
    }
}

void Outline::rightButtonPressed( QListViewItem *, const QPoint &pnt, int )
{
    if ( !selectedItem() || !doc->isReadWrite())
        return;
    view->openPopupMenuSideBar(pnt);
}

void Outline::renamePageTitle()
{
    QListViewItem *item = QListView::selectedItem();
    if ( !item )
        return;

    int pageNumber = item->text( 1 ).toInt() - 1;
    bool ok;
    QString activeTitle = doc->pageList().at( pageNumber )->pageTitle( i18n( "Slide %1" ).arg( pageNumber + 1 ) );
    QString newTitle = KLineEditDlg::getText( i18n("Rename Page"),i18n("Page Title"), activeTitle, &ok, this );

    // Have a different name ?
    if ( ok ) { // User pushed an OK button.
        if ( (newTitle.stripWhiteSpace()).isEmpty() ) { // Title is empty.
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Page title cannot be empty."), i18n("Rename Page") );
            // Recursion
            renamePageTitle();
        }
        else if ( newTitle != activeTitle ) { // Title changed.
            KPresenterDoc *doc=view->kPresenterDoc();
            KPrChangeTitlePageNameCommand *cmd=new KPrChangeTitlePageNameCommand( i18n("Rename Page"),doc, activeTitle, newTitle,doc->pageList().at(pageNumber)  );
            cmd->execute();
            doc->addCommand(cmd);
        }
    }
}


#include <sidebar.moc>
