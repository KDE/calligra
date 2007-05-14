/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
 * Boston, MA 02110-1301, USA.
*/
#include "kexiscrollview.h"

#include <qcursor.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QEvent>
#include <Q3Frame>
#include <QMouseEvent>

#include <kdebug.h>
#include <kstaticdeleter.h>
#include <klocale.h>

#include <utils/kexirecordnavigator.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

//! @internal
class KexiScrollViewData
{
	public:
		QPixmap horizontalOuterAreaPixmapBuffer;
		QPixmap verticalOuterAreaPixmapBuffer;
};

// @todo warning: not reentrant!
static KStaticDeleter<KexiScrollViewData> KexiScrollView_data_deleter;
KexiScrollViewData* KexiScrollView_data = 0;

KexiScrollView::KexiScrollView(QWidget *parent, bool preview)
 : Q3ScrollView(parent, "kexiscrollview", Qt::WStaticContents)
 , m_widget(0)
 , m_helpFont(font())
 , m_preview(preview)
 , m_scrollViewNavPanel(0)
{
	setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	QColor fc = palette().active().foreground(),
		bc = viewport()->paletteBackgroundColor();
	m_helpColor = KexiUtils::blendedColors(fc, bc, 1, 2);
//	m_helpColor = QColor((fc.red()+bc.red()*2)/3, (fc.green()+bc.green()*2)/3, 
//		(fc.blue()+bc.blue()*2)/3);
	m_helpFont.setPointSize( m_helpFont.pointSize() * 3 );

	setFocusPolicy(Qt::WheelFocus);

	//initial resize mode is always manual;
	//will be changed on show(), if needed
	setResizePolicy(Manual);

	viewport()->setMouseTracking(true);
	m_resizing = false;
	m_enableResizing = true;
	m_snapToGrid = false;
	m_gridSize = 0;
	m_outerAreaVisible = true;

	m_delayedResize.setSingleShot( true );
	connect(&m_delayedResize, SIGNAL(timeout()), this, SLOT(refreshContentsSize()));
	m_smodeSet = false;
	if (m_preview) {
		refreshContentsSizeLater(true, true);
//! @todo allow to hide navigator
		updateScrollBars();
		m_scrollViewNavPanel = new KexiRecordNavigator(this, leftMargin(), "nav");
		m_scrollViewNavPanel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
	}
}

KexiScrollView::~KexiScrollView()
{
}

void
KexiScrollView::setWidget(QWidget *w)
{
	addChild(w);
	m_widget = w;
}

void
KexiScrollView::setRecordNavigatorVisible(bool visible)
{
	if(/*m_scrollViewNavPanel->isVisible() &&*/ !visible)
		m_scrollViewNavPanel->hide();
	else if(visible)  {
		m_scrollViewNavPanel->show();
		updateNavPanelGeometry();
	}
}

void
KexiScrollView::setSnapToGrid(bool enable, int gridSize)
{
	m_snapToGrid = enable;
	if(enable) {
		m_gridSize = gridSize;
	}
}

void
KexiScrollView::refreshContentsSizeLater(bool horizontal, bool vertical)
{
	Q_UNUSED( horizontal );
	Q_UNUSED( vertical );

	if (!m_smodeSet) {
		m_smodeSet = true;
		m_vsmode = vScrollBarMode();
		m_hsmode = hScrollBarMode();
	}
//	if (vertical)
		setVScrollBarMode(Q3ScrollView::AlwaysOff);
	//if (horizontal)
		setHScrollBarMode(Q3ScrollView::AlwaysOff);
	updateScrollBars();
	m_delayedResize.start( 100 );
}

void
KexiScrollView::refreshContentsSize()
{
	if(!m_widget)
		return;
	if (m_preview) {
		resizeContents(m_widget->width(), m_widget->height());
//		kDebug() << "KexiScrollView::refreshContentsSize(): ( " 
	//		<< m_widget->width() <<", "<< m_widget->height() << endl;
		setVScrollBarMode(m_vsmode);
		setHScrollBarMode(m_hsmode);
		m_smodeSet = false;
		updateScrollBars();
	}
	else {
		// Ensure there is always space to resize Form
		int w = contentsWidth(), h = contentsHeight();
		bool change = false;
		const int delta_x = qMax( (KexiScrollView_data ? 
			KexiScrollView_data->verticalOuterAreaPixmapBuffer.width() : 0), 300);
		const int delta_y = qMax( (KexiScrollView_data ? 
			KexiScrollView_data->horizontalOuterAreaPixmapBuffer.height() : 0), 300);
		if((m_widget->width() + delta_x * 2 / 3) > w) {
			w = m_widget->width() + delta_x;
			change = true;
		}
		else if((w - m_widget->width()) > delta_x) {
			w = m_widget->width() + delta_x;
			change = true;
		}
		if((m_widget->height() + delta_y * 2 / 3) > h) {
			h = m_widget->height() + delta_y;
			change = true;
		}
		else if((h - m_widget->height()) > delta_y) {
			h = m_widget->height() + delta_y;
			change = true;
		}
		if (change) {
	repaint();
	viewport()->repaint();
	repaintContents();
	updateContents(0, 0, 2000,2000);
	clipper()->repaint();

			resizeContents(w, h);
		}
//		kDebug() << "KexiScrollView::refreshContentsSize(): ( " 
	//		<< contentsWidth() <<", "<< contentsHeight() << endl;
		updateScrollBars();
		setVScrollBarMode(Auto);
		setHScrollBarMode(Auto);
	}
	updateContents();
	updateScrollBars();
}

void
KexiScrollView::updateNavPanelGeometry()
{
	if (m_scrollViewNavPanel)
		m_scrollViewNavPanel->updateGeometry(leftMargin());
}

void
KexiScrollView::contentsMousePressEvent(QMouseEvent *ev)
{
	if(!m_widget)
		return;

	QRect r3(0, 0, m_widget->width() + 4, m_widget->height() + 4);
	if(!r3.contains(ev->pos())) // clicked outside form
		//m_form->resetSelection();
		emit outerAreaClicked();

	if(!m_enableResizing)
		return;

	QRect r(m_widget->width(),  0, 4, m_widget->height() + 4); // right limit
	QRect r2(0, m_widget->height(), m_widget->width() + 4, 4); // bottom limit
	if(r.contains(ev->pos()) || r2.contains(ev->pos()))
	{
		m_resizing = true;
		emit resizingStarted();
	}
}

void
KexiScrollView::contentsMouseReleaseEvent(QMouseEvent *)
{
	if(m_resizing) {
		m_resizing = false;
		emit resizingEnded();
	}

	unsetCursor();
}

void
KexiScrollView::contentsMouseMoveEvent(QMouseEvent *ev)
{
	if(!m_widget || !m_enableResizing)
		return;

	if(m_resizing) // resize widget
	{
		int tmpx = ev->x(), tmpy = ev->y();
		const int exceeds_x = (tmpx - contentsX() + 5) - clipper()->width();
		const int exceeds_y = (tmpy - contentsY() + 5) - clipper()->height();
		if (exceeds_x > 0)
			tmpx -= exceeds_x;
		if (exceeds_y > 0)
			tmpy -= exceeds_y;
		if ((tmpx - contentsX()) < 0)
			tmpx = contentsX();
		if ((tmpy - contentsY()) < 0)
			tmpy = contentsY();

		// we look for the max widget right() (or bottom()), which would be the limit for form resizing (not to hide widgets)
		const QList<QWidget*> list( m_widget->findChildren<QWidget*>() ); /* not recursive*/
		foreach(QWidget *w, list) {
			tmpx = qMax(tmpx, (w->geometry().right() + 10));
			tmpy = qMax(tmpy, (w->geometry().bottom() + 10));
		}

		int neww = -1, newh;
		if(cursor().shape() == Qt::SizeHorCursor)
		{
			if(m_snapToGrid)
				neww = int( float(tmpx) / float(m_gridSize) + 0.5 ) * m_gridSize;
			else
				neww = tmpx;
			newh = m_widget->height();
		}
		else if(cursor().shape() == Qt::SizeVerCursor)
		{
			neww = m_widget->width();
			if(m_snapToGrid)
				newh = int( float(tmpy) / float(m_gridSize) + 0.5 ) * m_gridSize;
			else
				newh = tmpy;
		}
		else if(cursor().shape() == Qt::SizeFDiagCursor)
		{
			if(m_snapToGrid) {
				neww = int( float(tmpx) / float(m_gridSize) + 0.5 ) * m_gridSize;
				newh = int( float(tmpy) / float(m_gridSize) + 0.5 ) * m_gridSize;
			} else {
				neww = tmpx;
				newh = tmpy;
			}
		}
		//needs update?
		if (neww!=-1 && m_widget->size() != QSize(neww, newh)) {
			m_widget->resize( neww, newh );
			refreshContentsSize();
			updateContents();
		}
	}
	else // update mouse cursor
	{
		QPoint p = ev->pos();
		QRect r(m_widget->width(),  0, 4, m_widget->height()); // right
		QRect r2(0, m_widget->height(), m_widget->width(), 4); // bottom
		QRect r3(m_widget->width(), m_widget->height(), 4, 4); // bottom-right corner

		if(r.contains(p))
			setCursor(Qt::SizeHorCursor);
		else if(r2.contains(p))
			setCursor(Qt::SizeVerCursor);
		else if(r3.contains(p))
			setCursor(Qt::SizeFDiagCursor);
		else
			unsetCursor();
	}
}

void 
KexiScrollView::setupPixmapBuffer(QPixmap& pixmap, const QString& text, int lines)
{
	Q_UNUSED( lines );

	QFontMetrics fm(m_helpFont);
	const int flags = Qt::AlignCenter|Qt::AlignTop;
	QRect rect(fm.boundingRect(0,0,1000,1000,flags,text));
	const int txtw = rect.width(), txth = rect.height();//fm.width(text), txth = fm.height()*lines;
	pixmap = QPixmap(txtw, txth);
	if (!pixmap.isNull()) {
		//create pixmap once
		pixmap.fill( viewport()->paletteBackgroundColor() );
		QPainter pb(&pixmap);
		pb.initFrom(this);
		pb.setPen(m_helpColor);
		pb.setFont(m_helpFont);
		pb.drawText(0, 0, txtw, txth, Qt::AlignCenter|Qt::AlignTop, text);
	}
}

void
KexiScrollView::drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph ) 
{
	Q3ScrollView::drawContents(p, clipx, clipy, clipw, cliph);
	if (m_widget) {
		if(m_preview || !m_outerAreaVisible)
			return;

		//draw right and bottom borders
		const int wx = childX(m_widget);
		const int wy = childY(m_widget);
		p->setPen(palette().active().foreground());
		p->drawLine(wx+m_widget->width(), wy, wx+m_widget->width(), wy+m_widget->height());
		p->drawLine(wx, wy+m_widget->height(), wx+m_widget->width(), wy+m_widget->height());
//kDebug() << "KexiScrollView::drawContents() " << wy+m_widget->height() << endl;

		if (!KexiScrollView_data) {
			KexiScrollView_data_deleter.setObject( KexiScrollView_data, new KexiScrollViewData() );

			//create flicker-less buffer
			setupPixmapBuffer( KexiScrollView_data->horizontalOuterAreaPixmapBuffer, i18n("Outer Area"), 1 );
			setupPixmapBuffer( KexiScrollView_data->verticalOuterAreaPixmapBuffer, i18n("Outer\nArea"), 2 );
		}
		if (!KexiScrollView_data->horizontalOuterAreaPixmapBuffer.isNull() 
			&& !KexiScrollView_data->verticalOuterAreaPixmapBuffer.isNull() 
			&& !m_delayedResize.isActive() /* only draw text if there's not pending delayed resize*/)
		{
			if (m_widget->height()>(KexiScrollView_data->verticalOuterAreaPixmapBuffer.height()+20)) {
				p->drawPixmap( 
					qMax( m_widget->width(), KexiScrollView_data->verticalOuterAreaPixmapBuffer.width() + 20 ) + 20,
					qMax( (m_widget->height() - KexiScrollView_data->verticalOuterAreaPixmapBuffer.height())/2, 20 ),
					KexiScrollView_data->verticalOuterAreaPixmapBuffer
				);
			}
			p->drawPixmap( 
				qMax( (m_widget->width() - KexiScrollView_data->horizontalOuterAreaPixmapBuffer.width())/2, 20 ),
				qMax( m_widget->height(), KexiScrollView_data->horizontalOuterAreaPixmapBuffer.height() + 20 ) + 20,
				KexiScrollView_data->horizontalOuterAreaPixmapBuffer
			);
		}
	}
}

void
KexiScrollView::leaveEvent( QEvent *e )
{
	QWidget::leaveEvent(e);
	m_widget->update(); //update form elements on too fast mouse move
}

void
KexiScrollView::setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h )
{
/*todo*/
//	kDebug(44021)<<"KexiScrollView::setHBarGeometry"<<endl;
	if (m_scrollViewNavPanel && m_scrollViewNavPanel->isVisible()) {
		m_scrollViewNavPanel->setHBarGeometry( hbar, x, y, w, h );
	}
	else {
		hbar.setGeometry( x, y, w, h );
	}
}

KexiRecordNavigator*
KexiScrollView::recordNavigator() const
{
	return m_scrollViewNavPanel;
}

#include "kexiscrollview.moc"

