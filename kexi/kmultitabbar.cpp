/***************************************************************************
                          kmultitabbar.cpp -  description
                             -------------------
    begin                :  2001
    copyright            : (C) 2001,2002 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kmultitabbar.h"
#include "kmultitabbar.moc"
#include <qbutton.h>
#include <qpopupmenu.h>
#include <qlayout.h>
#include <qpainter.h>
#include <kdebug.h>
#include <qtooltip.h>

KMultiTabBarInternal::KMultiTabBarInternal(QWidget *parent, KMultiTabBar::KMultiTabBarBasicMode bm):QScrollView(parent)
	{
		m_tabs.setAutoDelete(true);
		setHScrollBarMode(AlwaysOff);
		setVScrollBarMode(AlwaysOff);
		if (bm==KMultiTabBar::Vertical)
		{
			box=new QVBox(viewport());			
			box->setFixedWidth(24);
			setFixedWidth(24);
		}
		else
		{
			box=new QHBox(viewport());			
			box->setFixedHeight(24);
			setFixedHeight(24);
		}
		addChild(box);
		setFrameStyle(NoFrame);
		viewport()->setBackgroundMode(Qt::PaletteBackground);

	}

void KMultiTabBarInternal::drawContents ( QPainter * paint, int clipx, int clipy, int clipw, int cliph )
{
	QScrollView::drawContents (paint , clipx, clipy, clipw, cliph );

	if (position==KMultiTabBar::Right)
	{

                paint->setPen(colorGroup().shadow());
                paint->drawLine(0,0,0,viewport()->height());
                paint->setPen(colorGroup().background().dark(120));
                paint->drawLine(1,0,1,viewport()->height());


	}
	else
	if (position==KMultiTabBar::Left)
	{
                paint->setPen(colorGroup().light());
		paint->drawLine(23,0,23,viewport()->height());
                paint->drawLine(22,0,22,viewport()->height());

                paint->setPen(colorGroup().shadow());
                paint->drawLine(0,0,0,viewport()->height());
	}
	else
	if (position==KMultiTabBar::Bottom)
	{
		paint->setPen(colorGroup().shadow());
		paint->drawLine(0,0,viewport()->width(),0);
                paint->setPen(colorGroup().background().dark(120));
                paint->drawLine(0,1,viewport()->width(),1);
	}
	else
	{
	        paint->setPen(colorGroup().light());
		paint->drawLine(0,23,viewport()->width(),23);
                paint->drawLine(0,22,viewport()->width(),22);

/*                paint->setPen(colorGroup().shadow());
                paint->drawLine(0,0,0,viewport()->height());*/

	}


}

void KMultiTabBarInternal::contentsMousePressEvent(QMouseEvent *ev)
{
	ev->ignore();
}

void KMultiTabBarInternal::mousePressEvent(QMouseEvent *ev)
{
	ev->ignore();
}


KMultiTabBarTab* KMultiTabBarInternal::getTab(int id)
{
        for (uint pos=0;pos<m_tabs.count();pos++)
        {
                if (m_tabs.at(pos)->id()==id)
                        return m_tabs.at(pos);
        }
        return 0;
}


int KMultiTabBarInternal::insertTab(QPixmap pic ,int id,const QString& text)
{
	KMultiTabBarTab  *tab;
	m_tabs.append(tab= new KMultiTabBarTab(pic,text,id,box,position));
	tab->show();
	return 0;
}

void KMultiTabBarInternal::removeTab(int id)
{
	for (uint pos=0;pos<m_tabs.count();pos++)
	{
		if (m_tabs.at(pos)->id()==id)
		{
			m_tabs.remove(pos);
			break;
		}
	}
}

void KMultiTabBarInternal::setPosition(enum KMultiTabBar::KMultiTabBarPosition pos)
{
	position=pos;
	for (uint i=0;i<m_tabs.count();i++)
		m_tabs.at(i)->setPosition(position);
	viewport()->repaint();
}


KMultiTabBarButton::KMultiTabBarButton(const QPixmap& pic,const QString& text, QPopupMenu *popup,
		int id,QWidget *parent,KMultiTabBar::KMultiTabBarPosition pos)
	:QPushButton(pic,text,parent)
{
	position=pos;
  	if (popup) setPopup(popup);
	setFlat(true);
	setFixedHeight(24);
	setFixedWidth(24);
	m_id=id;
	QToolTip::add(this,text);
	connect(this,SIGNAL(clicked()),this,SLOT(slotClicked()));
}


void KMultiTabBarButton::slotClicked()
{
	emit clicked(m_id);
}

void KMultiTabBarButton::setPosition(KMultiTabBar::KMultiTabBarPosition pos)
{
	position=pos;
	repaint();
}




KMultiTabBarTab::KMultiTabBarTab(const QPixmap& pic, const QString& text,
		int id,QWidget *parent,KMultiTabBar::KMultiTabBarPosition pos)
	:KMultiTabBarButton(pic,text,0,id,parent,pos)
{
	setToggleButton(true);
}


void KMultiTabBarTab::drawButton(QPainter *paint)
{
        QPixmap pixmap = iconSet()->pixmap( QIconSet::Small, QIconSet::Normal );
    paint->fillRect(0, 0, 24, 24, colorGroup().background());
	if (!isOn())
	{

		if (position==KMultiTabBar::Right)
		{
			paint->fillRect(0,0,21,21,QBrush(colorGroup().background()));

			paint->setPen(colorGroup().background().dark(150));
			paint->drawLine(0,22,23,22);

			paint->drawPixmap(12-pixmap.width()/2,12-pixmap.height()/2,pixmap);

//			paint->setPen(colorGroup().light());
//			paint->drawLine(19,21,21,21);
//			paint->drawLine(21,19,21,21);


			paint->setPen(colorGroup().shadow());
			paint->drawLine(0,0,0,23);
			paint->setPen(colorGroup().background().dark(120));
			paint->drawLine(1,0,1,23);

		}
		else
		if ((position==KMultiTabBar::Bottom) || (position==KMultiTabBar::Top))
		{
                        paint->fillRect(0,1,23,22,QBrush(colorGroup().background()));

                        paint->drawPixmap(12-pixmap.width()/2,12-pixmap.height()/2,pixmap);

                        paint->setPen(colorGroup().background().dark(120));
                        paint->drawLine(23,0,23,23);


                        paint->setPen(colorGroup().light());
                        paint->drawLine(0,22,23,22);
                        paint->drawLine(0,23,23,23);
                	paint->setPen(colorGroup().shadow());
                	paint->drawLine(0,0,23,0);
                        paint->setPen(colorGroup().background().dark(120));
                        paint->drawLine(0,1,23,1);

		}
		else
		{
			paint->setPen(colorGroup().background().dark(120));
			paint->drawLine(0,23,23,23);
			paint->fillRect(0,0,23,21,QBrush(colorGroup().background()));
			paint->drawPixmap(12-pixmap.width()/2,12-pixmap.height()/2,pixmap);

			paint->setPen(colorGroup().light());
			paint->drawLine(23,0,23,23);
			paint->drawLine(22,0,22,23);

			paint->setPen(colorGroup().shadow());
			paint->drawLine(0,0,0,23);


		}


	}
	else
	{
		if (position==KMultiTabBar::Right)
		{
			paint->setPen(colorGroup().shadow());
			paint->drawLine(0,23,23,23);
			paint->drawLine(0,22,23,22);
			paint->drawLine(23,0,23,23);
			paint->drawLine(22,0,22,23);
			paint->fillRect(0,0,21,21,QBrush(colorGroup().light()));
			paint->drawPixmap(10-pixmap.width()/2,10-pixmap.height()/2,pixmap);
//			paint->drawLine(19,21,21,21);
//			paint->drawLine(21,19,21,21);
		}
		else 
		if (position==KMultiTabBar::Top)
		{
//			paint->setPen(colorGroup().shadow());
//			paint->drawLine(0,23,23,23);
//			paint->drawLine(0,22,23,22);
			paint->fillRect(0,0,23,23,QBrush(colorGroup().light()));
			paint->drawPixmap(10-pixmap.width()/2,10-pixmap.height()/2,pixmap);
		}
		else
		{
			paint->setPen(colorGroup().shadow());
			paint->drawLine(0,23,23,23);
			paint->drawLine(0,22,23,22);
			paint->fillRect(0,0,23,21,QBrush(colorGroup().light()));
			paint->drawPixmap(10-pixmap.width()/2,10-pixmap.height()/2,pixmap);
		}

	}
}







KMultiTabBar::KMultiTabBar(QWidget *parent,KMultiTabBarBasicMode bm):QWidget(parent)
{
	buttons.setAutoDelete(false);
	if (bm==Vertical)
	{
		l=new QVBoxLayout(this);
		setFixedWidth(24);
	}
	else
	{
		l=new QHBoxLayout(this);
		setFixedHeight(24);
	}
	l->setMargin(0);
	l->setAutoAdd(false);
	
	internal=new KMultiTabBarInternal(this,bm);
	setPosition((bm==KMultiTabBar::Vertical)?KMultiTabBar::Right:KMultiTabBar::Bottom);
	l->insertWidget(0,internal);
	l->insertWidget(0,btnTabSep=new QFrame(this));
	btnTabSep->setFixedHeight(4);
	btnTabSep->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	btnTabSep->setLineWidth(2);
	btnTabSep->hide();
	
}

/*int KMultiTabBar::insertButton(QPixmap pic,int id ,const QString&)
{
  (new KToolbarButton(pic,id,internal))->show();
  return 0;
}*/

int KMultiTabBar::insertButton(QPixmap pic ,int id,QPopupMenu *popup,const QString&)
{
	KMultiTabBarButton  *btn;
	buttons.append(btn= new KMultiTabBarButton(pic,QString::null,
			popup,id,this,position));
	l->insertWidget(0,btn);
	btn->show();
	btnTabSep->show();
	return 0;
}

int KMultiTabBar::insertTab(QPixmap pic ,int id ,const QString& text)
{
 internal->insertTab(pic,id,text);
 return 0;
}

KMultiTabBarButton* KMultiTabBar::getButton(int id)
{
	for (uint pos=0;pos<buttons.count();pos++)
	{
		if (buttons.at(pos)->id()==id)
			return buttons.at(pos);
	}
	return 0;
}

KMultiTabBarTab* KMultiTabBar::getTab(int id)
{
	return internal->getTab(id);
}



void KMultiTabBar::removeButton(int id)
{
	for (uint pos=0;pos<buttons.count();pos++)
	{
		if (buttons.at(pos)->id()==id)
		{
			buttons.take(pos)->deleteLater();
			break;
		}
	}
	if (buttons.count()==0) btnTabSep->hide();
}

void KMultiTabBar::removeTab(int id)
{
	internal->removeTab(id);
}

void KMultiTabBar::setTab(int id,bool state)
{
	KMultiTabBarTab *tab=getTab(id);
	if (tab)
	{
		if(state) tab->setOn(true); else tab->setOn(false);
	}
}

bool KMultiTabBar::isTabRaised(int id)
{
	KMultiTabBarTab *tab=getTab(id);
	if (tab)
	{
		if (tab->isOn()) return true;
	}
	return false;
}


void KMultiTabBar::setPosition(KMultiTabBarPosition pos)
{
	position=pos;
	internal->setPosition(pos);
	for (uint i=0;i<buttons.count();i++)
		buttons.at(i)->setPosition(pos);
}

QPtrList<KMultiTabBarTab>* KMultiTabBar::tabs(){return internal->tabs();}

