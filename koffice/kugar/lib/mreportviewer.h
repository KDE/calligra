/***************************************************************************
             mreportviewer.h  -  Kugar QT report viewer widget
             -------------------
   begin     : Fri Aug 13 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#ifndef MREPORTVIEWER_H
#define MREPORTVIEWER_H

#include <qapplication.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qprogressdialog.h>
#include <qguardedptr.h>
#include <koffice_export.h>
#include "mreportengine.h"
#include "mpagecollection.h"
#include "mpagedisplay.h"

#define M_PROGRESS_DELAY 500            // Number of ms to delay progress dialog display

class KPrinter;

/** Application reference, required for event processing */
extern QApplication *mAppRef;

/**Kugar QT report viewer widget
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class KUGAR_EXPORT MReportViewer : public QWidget
{
    Q_OBJECT

public:
    MReportViewer( QWidget *parent = 0, const char *name = 0 );
    MReportViewer( MReportEngine* engine, QWidget *parent = 0, const char *name = 0 );
    virtual ~MReportViewer();

    bool setReportData( const QString & );
    bool setReportData( QIODevice * );
    bool setReportTemplate( const QString & );
    bool setReportTemplate( QIODevice * );
    bool renderReport();
    void clearReport();
    void printReport();
    void printReport( KPrinter &printer );
    void setupPrinter( KPrinter &printer );
    void printReportSilent( int printFrom = -1, int printTo = -1, int printCopies = -1, QString printerName = QString::null );

    QSize sizeHint() const;

public slots:
    void slotFirstPage();
    void slotNextPage();
    void slotPrevPage();
    void slotLastPage();

signals:
    void preferedTemplate( const QString & );

private slots:
    void slotCancelPrinting();
    void slotRenderProgress( int );

protected:
    void paintEvent( QPaintEvent *event );
    void resizeEvent( QResizeEvent *event );

    QScrollView *scroller;
    MPageDisplay *display;
    MReportEngine *rptEngine;
    QGuardedPtr<MPageCollection> report;
    KPrinter *printer;

private:
    QProgressDialog* progress;
    int totalSteps;
    void init();
};

}

#endif
