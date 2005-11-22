/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_ICON_VIEW_H
#define KIVIO_ICON_VIEW_H

#include <qdom.h>
#include <qiconview.h>
#include <qptrlist.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qdragobject.h>

class KivioStencilSpawnerSet;
class KivioStencilSpawner;

class KivioIconView : public QIconView
{
  Q_OBJECT
  protected:
    KivioStencilSpawnerSet *m_pSpawnerSet;
    static KivioStencilSpawner *m_pCurDrag;

    void drawBackground( QPainter *, const QRect & );
    QDragObject *dragObject();

  protected slots:
    void slotDoubleClicked( QIconViewItem * );

  signals:
    void createNewStencil( KivioStencilSpawner * );

  public:
    KivioIconView( bool _readWrite,QWidget *parent=0, const char *name=0 );
    virtual ~KivioIconView();

    static void clearCurrentDrag();
    static KivioStencilSpawner *curDragSpawner() { return m_pCurDrag; }

    void setStencilSpawnerSet( KivioStencilSpawnerSet * );
    KivioStencilSpawnerSet *spawnerSet() { return m_pSpawnerSet; }

  private:
    static QPtrList<KivioIconView> objList;
    bool isReadWrite;
};

class KivioIconViewItem : public QIconViewItem
{
  friend class KivioIconView;

  protected:
    KivioStencilSpawner *m_pSpawner;

  public:
    KivioIconViewItem( QIconView *parent );
    virtual ~KivioIconViewItem();

    void setStencilSpawner( KivioStencilSpawner * );
    KivioStencilSpawner *spawner() { return m_pSpawner; }

    virtual bool acceptDrop( const QMimeSource *e ) const;
};


#endif

