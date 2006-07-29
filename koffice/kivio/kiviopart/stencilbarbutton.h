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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef STENCILBARBUTTON_H
#define STENCILBARBUTTON_H

#include <qpushbutton.h>

class QPixmap;
namespace Kivio {

class DragBarButton : public QPushButton
{
  Q_OBJECT
  public:
    DragBarButton( const QString&, QWidget* parent, const char* name = 0 );
    ~DragBarButton();
  
    void setIcon( const QString& );
    void setPixmap( const QPixmap& );
  
    QSize sizeHint() const;
    
    Qt::Orientation orientation() const { return m_orientation; }
  
  signals:
    void beginDrag();
    void finishDrag();
    void closeRequired(DragBarButton *);
  
  public slots:
    void setOrientation(Orientation orientation);
  
  protected:
    void drawButton( QPainter* );
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
  
    void enterEvent( QEvent* );
    void leaveEvent( QEvent* );
  
  private:
    QPixmap* m_pClosePix;
    QPixmap* m_pIcon;
    bool m_bPressed;
    bool m_bDragged;
    bool m_bMouseOn;
    bool m_bClose;
    QPoint m_ppoint;
    Qt::Orientation m_orientation;
};

}

#endif

