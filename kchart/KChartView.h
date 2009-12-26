/* This file is part of the KDE project

   Copyright 1999-2007  Kalle Dalheimer <kalle@kde.org>
   Copyright 2005-2007  Inge Wallin <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef KCHART_VIEW
#define KCHART_VIEW


// Qt
#include <QMouseEvent>

// KOffice
#include <KoView.h>
#include <KoViewConverter.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoZoomMode.h>


class QPaintEvent;
class KAction;
class KToggleAction;
class KoCanvasBase;
class KoCanvasController;


namespace KChart
{

class KChartPart;
class KChartCanvas;
class ViewAdaptor;

class KChartView : public KoView
{
    Q_OBJECT
public:
    explicit KChartView( KChartPart* part, QWidget* parent = 0 );
    ~KChartView();

    virtual ViewAdaptor* dbusObject();
    KoViewConverter *viewConverter() const { return m_zoomHandler; }
    KChartCanvas    *canvasWidget()  const { return m_canvas; }
    void config(int flag);

    virtual KoZoomController *zoomController() const { return m_zoomController; }


public slots:
    void  saveConfig();
    void  loadConfig();
    void  defaultConfig();

    void  slotRepaint();

    void  slotConfigPageLayout();
    void  importData();
    void  extraCreateTemplate();

    void selectionChanged();
    void documentViewRectChanged( const QRectF &viewRect );
    void zoomChanged( KoZoomMode::Mode mode, qreal zoom );

signals:
    void selectionChange();

protected:
    void          paintEvent( QPaintEvent* );

    virtual void  updateReadWrite( bool readwrite );

    virtual void  mousePressEvent ( QMouseEvent * );

    virtual KoPrintJob * createPrintJob();

private:
    KAction  *m_importData;
    KAction  *m_edit;
    KAction  *m_saveconfig;
    KAction  *m_loadconfig;
    KAction  *m_defaultconfig;

    ViewAdaptor      *m_dbus;

    KoZoomHandler    *m_zoomHandler;
    KoZoomController *m_zoomController;

    KChartCanvas       *m_canvas;
    KoCanvasController *m_canvasController;
};

}  //KChart namespace

#endif
