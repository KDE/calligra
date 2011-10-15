/* This file is part of the KDE project
   Copyright (C) 2011  Shreya Pandit <shreya@shreyapandit.com>

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

#include "WebBrowserWidget.h"
#include <QtWebKit>
#include <QtWebKit/QWebHistory>
#include <QWebView>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <KLocale>
#include <kguiitem.h>
#include <kstandardguiitem.h>


WebBrowserWidget::WebBrowserWidget(QWidget *parent)
        : QWidget(parent),KexiFormDataItemInterface()
        ,KFormDesigner::FormWidgetInterface()
	,m_readOnly(false)
	,m_urlChanged_enabled(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(sizeHint().height());
    setMinimumWidth(minimumHeight());
    QPair< KGuiItem, KGuiItem > backForward = KStandardGuiItem::backAndForward();
    m_view = new QWebView(this);
    m_reload=new KPushButton(i18n("Reload"),this);
    m_stop=new KPushButton(KStandardGuiItem::stop());
    m_back= new KPushButton(backForward.first);
    m_forward= new KPushButton(backForward.second);
    h_layout = new QHBoxLayout;
    h_layout->addWidget(m_reload);
    h_layout->addWidget(m_stop);
    h_layout->addStretch();
    h_layout->addWidget(m_back);
    h_layout->addWidget(m_forward);
    v_layout = new QVBoxLayout();
    v_layout->addWidget(m_view);
    v_layout->addLayout(h_layout);
    setLayout(v_layout);
  
    if(!designMode()){
     m_pbar=new QProgressBar();
     h_layout->addWidget(m_pbar);
    }
    else{
      m_pbar=0;
    }

    connect(m_back,SIGNAL(clicked()),m_view,SLOT(back()));
    connect(m_forward,SIGNAL(clicked()),m_view,SLOT(forward()));
    connect(m_reload,SIGNAL(clicked()),m_view,SLOT(reload()));
    connect(m_stop,SIGNAL(clicked()),m_view,SLOT(stop()));
    connect(m_view,SIGNAL(loadProgress(int)),m_pbar,SLOT(setValue(int)));
    connect(m_view,SIGNAL(loadFinished(bool)),SLOT(hide_bar()));
  
};
  
WebBrowserWidget::WebBrowserWidget()  
{
}

WebBrowserWidget::~WebBrowserWidget()	
{

}

void WebBrowserWidget::setDataSourcePartClass(const QString &ds) {
        KexiFormDataItemInterface::setDataSourcePartClass(ds);
    }
 


void WebBrowserWidget::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    
}

void WebBrowserWidget::hide_bar()
{
    m_pbar->setVisible(false);    
}

void WebBrowserWidget::setUrl(const QString& url)
{
    setUrl(url.isEmpty() ? QUrl() : QUrl(url));
}

void WebBrowserWidget::setUrl(const QUrl& url)
{
    m_view->setUrl(url);
}

void WebBrowserWidget::updateToolBar()
{
    if(m_view->history()) {
      m_back->setEnabled(true);
    }
    if(m_view->history()) {
      m_forward->setEnabled(true);
    }
}


void WebBrowserWidget::setZoomFactor(qreal factor)
{
    m_view->setZoomFactor(factor);
}

void WebBrowserWidget::setTextScale(qreal scale)
{
    m_view->setTextSizeMultiplier(scale);
}

bool WebBrowserWidget::cursorAtStart()
{
    return false;
}

bool WebBrowserWidget::cursorAtEnd()
{
    return false;
}


QVariant WebBrowserWidget::value()
{
    if (dataSource().isEmpty()) {

      return QVariant();
    }
   
    return m_view->url();
}

bool WebBrowserWidget::valueIsNull()
{
    return m_view->url().isValid();

}
void WebBrowserWidget::clear()
{
    setUrl(QUrl());
}

void WebBrowserWidget::setInvalidState(const QString& displayText)
{
    Q_UNUSED(displayText);

    if (!dataSource().isEmpty()) {
        m_view->setUrl(QUrl());
    }
    setReadOnly(true);
}

void WebBrowserWidget::setValueInternal(const QVariant &add, bool removeOld)
{
    Q_UNUSED(add);
    Q_UNUSED(removeOld);

    if (isReadOnly())
        return;
    m_urlChanged_enabled= false;		
    
    if (removeOld) { 			
	setUrl(add.toString());  
    }       
    else{
        setUrl(m_origValue.toString() + add.toString());
     	
    }

    if (removeOld)
        { 			
	 setUrl(add.toString()); 
	}       
    else
        { setUrl(m_origValue.toString() + add.toString()) ;
     	
        }

    m_urlChanged_enabled = true;
}

bool WebBrowserWidget::valueIsEmpty()
{
    return false;
}


bool WebBrowserWidget::isReadOnly() const
{
    return m_readOnly;
}


void  WebBrowserWidget::setReadOnly(bool val)
{
    m_readOnly=val;
}

#include "WebBrowserWidget.moc"
