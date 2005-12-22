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
#ifndef KIVIO_STACK_BAR_H
#define KIVIO_STACK_BAR_H

#include <qdockwindow.h>
#include <qptrdict.h>

namespace Kivio {
  class DragBarButton;
}

class KivioView;

using namespace Kivio;

class KivioStackBar : public QDockWindow
{
  Q_OBJECT
  public:
    KivioStackBar(KivioView* view, QWidget* parent=0, const char* name=0);
    ~KivioStackBar();
  
    QWidget* findPage(const QString& name);
    QWidget* findPage(DragBarButton*);
    void insertPage(QWidget*, const QString&);
    void removePage(QWidget*);
    void deletePageAndButton(DragBarButton*);
    void showPage(QWidget*);
  
    QWidget* visiblePage() const { return m_visiblePage; }
    
    KivioView* view() const { return m_view; } 
  
  signals:
    void aboutToShow(QWidget*);
    void beginDragPage(DragBarButton*);
    void finishDragPage(DragBarButton*);
    void deleteButton(DragBarButton*, QWidget*, KivioStackBar*);
  
  protected:
    virtual void closeEvent(QCloseEvent*);
  
  protected slots:
    void showButtonPage();
    void buttonBeginDrag();
    void buttonFinishDrag();
    void slotDeleteButton(DragBarButton*);
    void newPlace(QDockWindow::Place place);
  
  private:
    QPtrDict<QWidget> m_data;
    QWidget* m_visiblePage;
    KivioView* m_view;
};

#endif
