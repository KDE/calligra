/***************************************************************************
                          kexiformbase.cpp  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klineedit.h>

#include <qptrlist.h>
#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <kurlrequester.h>


#include <qobjectlist.h>

#include "kexiformbase.h"


class KexiFormBase::EditGUIClient: public KXMLGUIClient
{
	public:
		EditGUIClient():KXMLGUIClient()
		{
		        m_lineedit = new KAction(i18n("Line Edit"), "lineedit",
                		Key_F5, actionCollection(), "widget_line_edit");

		        m_button = new KAction(i18n("Push Button"), "button",
		                Key_F6,  actionCollection(), "widget_push_button");

		        m_urlreq = new KAction(i18n("URL Request"), "button",
		                Key_F7, actionCollection(), "widget_url_requester");
			setXMLFile("kexiformeditorui.rc");
		}
		virtual ~EditGUIClient(){;}
		void activate(QObject* o)
		{
			connect(m_lineedit,SIGNAL(activated()),o,SLOT(slotWidgetLineEdit()));
			connect(m_button,SIGNAL(activated()),o,SLOT(slotWidgetPushButton()));
			connect(m_urlreq,SIGNAL(activated()),o,SLOT(slotWidgetURLRequester()));
		}
		void deactivate(QObject* o)
		{
			m_lineedit->disconnect(o);
			m_button->disconnect(o);
			m_urlreq->disconnect(o);
		}
	private:
	KAction *m_lineedit;
	KAction *m_button;
	KAction *m_urlreq;
};

KexiFormBase::EditGUIClient *KexiFormBase::m_editGUIClient=0;

KexiFormBaseResizeHandle::KexiFormBaseResizeHandle(QWidget *parent,QWidget *buddy, HandlePos pos):QWidget(parent)
{
	m_dragging=false;
	setBackgroundColor(black);
	setFixedWidth(6);
	setFixedHeight(6);
	m_pos=pos;	
	m_buddy=buddy;
	buddy->installEventFilter(this);
	updatePos();
	show();
}

void KexiFormBaseResizeHandle::updatePos()
{
	switch (m_pos)
	{
		case TopLeft: 		move(m_buddy->x(),m_buddy->y());
					setCursor(QCursor(SizeFDiagCursor));
					break;
		case TopCenter: 	move(m_buddy->x()+m_buddy->width()/2-3,m_buddy->y());
					setCursor(QCursor(SizeVerCursor));
					break;
		case TopRight: 		move(m_buddy->x()+m_buddy->width()-6,m_buddy->y());
					setCursor(QCursor(SizeBDiagCursor));
					break;
		case LeftCenter: 	move(m_buddy->x(),m_buddy->y()+m_buddy->height()/2-3);
					setCursor(QCursor(SizeHorCursor));
					break;
		case RightCenter: 	move(m_buddy->x()+m_buddy->width()-6,m_buddy->y()+m_buddy->height()/2-3);
					setCursor(QCursor(SizeHorCursor));
					break;
		case BottomLeft: 	move(m_buddy->x(),m_buddy->y()+m_buddy->height()-6);
					setCursor(QCursor(SizeBDiagCursor));
					break;
		case BottomCenter: 	move(m_buddy->x()+m_buddy->width()/2-3,m_buddy->y()+m_buddy->height()-6);
					setCursor(QCursor(SizeVerCursor));
					break;
		case BottomRight:	move(m_buddy->x()+m_buddy->width()-6,m_buddy->y()+m_buddy->height()-6);
				  	setCursor(QCursor(SizeFDiagCursor));
					break;

	}	

}


bool KexiFormBaseResizeHandle::eventFilter(QObject *obj, QEvent *ev)
{
	if ((ev->type()==QEvent::Move) || (ev->type()==QEvent::Resize))
	{
		QTimer::singleShot(0,this,SLOT(updatePos()));
	}
	return false;
}

void KexiFormBaseResizeHandle::mousePressEvent(QMouseEvent *ev)
{
	m_dragging=true;
	m_x=ev->x();
	m_y=ev->y();
}

void KexiFormBaseResizeHandle::mouseMoveEvent(QMouseEvent *ev)
{
	int m_dotSpacing=KexiFormBase::dotSpacing();
	if (!m_dragging) return;

	int tmpx=m_buddy->x();
	int tmpy=m_buddy->y();
	int tmpw=m_buddy->width();
	int tmph=m_buddy->height();
	int dummyx=ev->x()-m_x;
	int dummyy=ev->y()-m_y;
        dummyy = (((float)dummyy)/((float)m_dotSpacing)+0.5);
        dummyy*=m_dotSpacing;
        dummyx = (((float)dummyx)/((float)m_dotSpacing)+0.5);
        dummyx*=m_dotSpacing;

	switch (m_pos)
	{
		case TopRight:
				tmpw=tmpw+dummyx;
				tmpy=tmpy+dummyy;
				tmph=tmph-dummyy;
			break;
		case RightCenter:
				tmpw=tmpw+dummyx;
			break;
		case BottomRight:
				tmpw=tmpw+dummyx;
				tmph=tmph+dummyy;
			break;
		case TopCenter:
				tmpy=tmpy+dummyy;
				tmph=tmph-dummyy;
			break;
		case BottomCenter:
				tmph=tmph+dummyy;
			break;
		case TopLeft:
				tmpx=tmpx+dummyx;
				tmpw=tmpw-dummyx;
				tmpy=tmpy+dummyy;
				tmph=tmph-dummyy;
			break;
		case LeftCenter:
				tmpx=tmpx+dummyx;
				tmpw=tmpw-dummyx;
			break;
		case BottomLeft:
				tmpx=tmpx+dummyx;
				tmpw=tmpw-dummyx;
				tmph=tmph+dummyy;
			break;
	}

	if ( (tmpx!=m_buddy->x()) || (tmpy!=m_buddy->y()) )
		m_buddy->move(tmpx,tmpy);

	if ( (tmpw!=m_buddy->width()) || (tmph!=m_buddy->height()) )
		m_buddy->resize(tmpw,tmph);

}

void KexiFormBaseResizeHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	m_dragging=false;
}



KexiFormBaseResizeHandle::~KexiFormBaseResizeHandle()
{
}

KexiFormBaseResizeHandleSet::KexiFormBaseResizeHandleSet(QWidget *modify):QObject(modify->parentWidget()),m_widget(modify)
{
	QWidget *parent=modify->parentWidget();
	handles[0]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::TopLeft);
	handles[1]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::TopCenter);
	handles[2]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::TopRight);
	handles[3]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::LeftCenter);
	handles[4]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::RightCenter);
	handles[5]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::BottomLeft);
	handles[6]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::BottomCenter);
	handles[7]=new KexiFormBaseResizeHandle(parent,modify,KexiFormBaseResizeHandle::BottomRight);
}

KexiFormBaseResizeHandleSet::~KexiFormBaseResizeHandleSet()
{
	for (int i=0;i<8;i++) delete handles[i];
}



KexiFormBase::KexiFormBase(QWidget *parent, const char *name, QString identifier)
	: KexiDialogBase(parent,name)
{
//	initActions();
	
	setCaption(identifier);

	KIconLoader *iloader = KGlobal::iconLoader();
	setIcon(iloader->loadIcon("form", KIcon::Small));
	
	m_dotSpacing = dotSpacing();

	resize( 250, 250 );

	setBackgroundPixmap(m_dotBg);
	
	m_widgetRectRequested = false;
	m_widgetRect = false;
	m_activeWidget=0;
	m_activeMoveWidget=0;
	m_resizeHandleSet=0;
}

int KexiFormBase::dotSpacing()
{ 
	return 10;
}

KXMLGUIClient *KexiFormBase::guiClient()
{
	if (!m_editGUIClient)
		m_editGUIClient=new EditGUIClient();
	return m_editGUIClient;	
}

void KexiFormBase::activateActions()
{
	m_editGUIClient->activate(this);
}

void KexiFormBase::deactivateActions()
{
	m_editGUIClient->deactivate(this);
}


#if 0
void KexiFormBase::initActions()
{
        //own-actions
#if 0
#endif
}
#endif

void KexiFormBase::slotWidgetLineEdit()
{
	kdDebug() << "add line edit widget at " << this << endl;
	m_pendingWidget = new KLineEdit(this);
	m_widgetRectRequested = true;
}

void KexiFormBase::slotWidgetPushButton()
{
	m_pendingWidget = new QPushButton("push button", this);
	m_widgetRectRequested = true;
}

void KexiFormBase::slotWidgetURLRequester()
{
	m_pendingWidget = new KURLRequester("urlrequest", this);
	m_widgetRectRequested = true;
}

void KexiFormBase::mouseMoveEvent(QMouseEvent *ev)
{
	if(m_widgetRectRequested)
	{
		m_widgetRect = true;
		m_widgetRectBX = (((float)ev->x())/((float)m_dotSpacing)+0.5);
		m_widgetRectBX*=m_dotSpacing;
		m_widgetRectBY = (((float)ev->y())/((float)m_dotSpacing)+0.5);
		m_widgetRectBY*=m_dotSpacing;
		m_widgetRectEX = m_widgetRectBX;
		m_widgetRectEY = m_widgetRectBY;
		m_widgetRectRequested = false;
	}

	if(m_widgetRect)
	{
                m_widgetRectEX = (((float)ev->x())/((float)m_dotSpacing)+0.5);
		m_widgetRectEX*=m_dotSpacing;
                m_widgetRectEY = (((float)ev->y())/((float)m_dotSpacing)+0.5);
		m_widgetRectEY*=m_dotSpacing;
//		m_widgetRectEX = ev->x();
//		m_widgetRectEY = ev->y();
		repaint();
	}
}

void KexiFormBase::resizeEvent(QResizeEvent *ev)
{
	QPainter *p = new QPainter();
	m_dotBg = QPixmap(size());
	p->begin(&m_dotBg, this);
	
	// drawing the dots -- got the dot?
	QColor c = paletteBackgroundColor();
//	QPen(c);
	p->setPen(QPen(c));
	QBrush bg(c);
	p->setBrush(bg);
	p->drawRect(0, 0, width(), height());
	QPen dots(black, 1);
	p->setPen(dots);
	int cols = width() / m_dotSpacing;
	int rows = height() / m_dotSpacing;
	
	for(int rowcursor = 1; rowcursor < rows; ++rowcursor)
	{
		for(int colcursor = 1; colcursor < cols; ++colcursor)
		{
			p->drawPoint(colcursor * m_dotSpacing, rowcursor * m_dotSpacing);
		}
	}
	
	p->end();
	setPaletteBackgroundPixmap(m_dotBg);
}

void KexiFormBase::paintEvent(QPaintEvent *ev)
{
	QPainter p(this);
	if(m_widgetRect)
	{
		QPen wpen(black, 2);
		p.setPen(wpen);
		p.drawRect(m_widgetRectBX, m_widgetRectBY, m_widgetRectEX-m_widgetRectBX, m_widgetRectEY-m_widgetRectBY);
	}
	p.end();
}


void KexiFormBase::installEventFilterRecursive(QObject *obj)
{
	obj->installEventFilter(this);
	static_cast<QWidget*>(obj)->setCursor(QCursor(SizeAllCursor));
	if ( obj->children() )
	{
		QObjectListIt it( *obj->children() );
		QObject *obj1;
        	while( (obj1=it.current()) != 0 )
		{
			++it;
			if (obj1->isWidgetType())
		    	installEventFilterRecursive(obj1);
            	}
        }

}

void KexiFormBase::mouseReleaseEvent(QMouseEvent *ev)
{
	if(m_widgetRect)
	{
		insertWidget(m_pendingWidget, m_widgetRectBX, m_widgetRectBY,
			m_widgetRectEX-m_widgetRectBX, m_widgetRectEY-m_widgetRectBY);
		m_widgetRectBX = 0;
		m_widgetRectBY = 0;
		m_widgetRectEX = 0;
		m_widgetRectEY = 0;

		installEventFilterRecursive(m_pendingWidget);

		m_widgetRect = false;


		repaint();
	}
}

void KexiFormBase::insertWidget(QWidget *widget, int x, int y, int w, int h)
{
	widget->move(x, y);
	widget->resize(w, h);
	widget->show();
	widget->setFocusPolicy(QWidget::NoFocus);
	activateWidget(widget);
//	widget->repaint();
//	grabMouse();
//	grabKeyboard();
}

void KexiFormBase::setResizeHandles(QWidget *m_activeWidget)
{
	if (!m_resizeHandleSet)
	{
		m_resizeHandleSet=new KexiFormBaseResizeHandleSet(m_activeWidget);
		return;
	}
	if (m_resizeHandleSet &&(m_resizeHandleSet->widget()!=m_activeWidget))
	{
		delete m_resizeHandleSet;
		m_resizeHandleSet=new KexiFormBaseResizeHandleSet(m_activeWidget);
	}

}

void KexiFormBase::activateWidget(QWidget *widget)
{
	m_activeWidget=widget;
	while (!(m_activeWidget->parentWidget(true)==this))
		m_activeWidget=m_activeWidget->parentWidget();
	setResizeHandles(m_activeWidget);

}

bool KexiFormBase::eventFilter(QObject *obj, QEvent *ev)
{
	kdDebug() << "event!" << endl;
	QWidget *sh;
	switch (ev->type())
	{
		case QEvent::MouseButtonPress:
			activateWidget(static_cast<QWidget*>(obj));
			m_activeMoveWidget=m_activeWidget;
			m_moveBX=static_cast<QMouseEvent*>(ev)->x();
			m_moveBY=static_cast<QMouseEvent*>(ev)->y();
			return true;
			break;
		case QEvent::MouseButtonRelease:
			m_activeMoveWidget=0;
			return true;
			break;
		case QEvent::MouseMove:
			if (m_activeMoveWidget)
			{
				int tmpx,tmpy;
		                tmpx = (((float)(m_activeMoveWidget->x()+static_cast<QMouseEvent*>(ev)->x()-m_moveBX))/
					((float)m_dotSpacing)+0.5);
		                tmpx*=m_dotSpacing;
		                tmpy = (((float)(m_activeMoveWidget->y()+static_cast<QMouseEvent*>(ev)->y()-m_moveBY))/
					((float)m_dotSpacing)+0.5);
		                tmpy*=m_dotSpacing;
				if ((tmpx!=m_activeMoveWidget->x()) ||(tmpy!=m_activeMoveWidget->y()) )
					m_activeMoveWidget->move(tmpx,tmpy);
			}
			return true;
			break;
		default:
			break;
	}
	switch( ev->type() )
	{
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseButtonDblClick:
		case QEvent::MouseMove:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::Wheel:
		case QEvent::ContextMenu:
			return true; // ignore
		default:
			break;
	}
	return false;
}

KexiFormBase::~KexiFormBase(){
}

#include "kexiformbase.moc"
