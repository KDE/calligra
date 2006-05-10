/***************************************************************************
             mpagedisplay.h  -  Kugar page display widget
             -------------------
   begin     : Sun Aug 15 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MPAGEDISPLAY_H
#define MPAGEDISPLAY_H

#include <QWidget>
#include <qpainter.h>
#include <qpixmap.h>
#include <q3picture.h>
#include <qsize.h>
//Added by qt3to4:
#include <QPaintEvent>

/**Kugar page display widget
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MPageDisplay : public QWidget
{
    Q_OBJECT
public:
    /** Constructor */
    MPageDisplay( QWidget *parent = 0, const char *name = 0 );
    /** Destructor */
    virtual ~MPageDisplay();

private:
    /** Page Widget's image buffer */
    QPixmap buffer;

public:
    /** Sets the page image */
    void setPage( Q3Picture* image );
    /** Sets the page display dimensions */
    void setPageDimensions( QSize size );

    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;

protected:
    /** Page widget's's paint event */
    void paintEvent( QPaintEvent* event );

};

}

#endif
