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
#ifndef STENCILBARDOCKMANAGER_H
#define STENCILBARDOCKMANAGER_H

#include <qwidget.h>
#include <qptrlist.h>

class QSplitter;
namespace Kivio {
class DragBarButton;
}
class KivioStackBar;
class KoToolDockMoveManager;
class KivioDoc;
class KivioView;

class StencilBarDockManager : public QWidget
{
  Q_OBJECT
  public:
    enum BarPos {
      Left = 0,
      Top = 1,
      Right = 2,
      Bottom = 3,
      OnDesktop = 4,
      AutoSelect = 5,
      OnTopLevelBar = 6
    };
  
    StencilBarDockManager( KivioView* parent, const char* name = 0 );
    ~StencilBarDockManager();
  
    void setDoc( KivioDoc* );
  
  
    void insertStencilSet( QWidget*,  const QString&, BarPos = AutoSelect,
                           QRect r = QRect(), KivioStackBar* destinationBar = 0L );
  
  public slots:
      void slotDeleteStencilSet( DragBarButton*,QWidget*,KivioStackBar* );
  
  protected slots:
    void slotBeginDragPage( DragBarButton* );
    void slotFinishDragPage( DragBarButton* );
    void slotMoving();
  
  private:
    KivioDoc *m_pDoc;
    DragBarButton* dragButton;
    QWidget* dragWidget;
    KoToolDockMoveManager* moveManager;
    QPtrList<KivioStackBar> m_pBars;
    QSplitter* split1;
    QSplitter* split2;
    KivioView* m_pView;
  
    BarPos dragPos;
    KivioStackBar* m_destinationBar;
};

#endif

