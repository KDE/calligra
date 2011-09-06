/*
    <This file is part of the KDE project>
    Copyright (C) 2011  Shreya Pandit <shreya@shreyapandit.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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


WebBrowserWidget::WebBrowserWidget(QWidget *parent)
        : QWidget(parent),KexiFormDataItemInterface()
        ,KFormDesigner::FormWidgetInterface()
	,m_readOnly(false)
	,m_urlChanged_enabled(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(sizeHint().height());
    setMinimumWidth(minimumHeight());
    m_view = new QWebView(this);
    m_backButton = new QPushButton(i18n("Back"),this);
    m_forward= new QPushButton(i18n("Forward"),this);
    m_reload=new QPushButton(i18n("Reload"),this);
    m_stop=new QPushButton(i18n("Stop"),this);
    h_layout = new QHBoxLayout;
    h_layout->addWidget(m_backButton);
    h_layout->addWidget(m_forward);
    h_layout->addWidget(m_reload);
    h_layout->addWidget(m_stop);
    h_layout->addStretch();
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

    connect(m_backButton,SIGNAL(clicked()),m_view,SLOT(back()));
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
    setUrl(QUrl(url));
}

void WebBrowserWidget::setUrl(const QUrl& url)
{
    m_url=url;
    m_view->load(m_url);
}

void WebBrowserWidget::updateToolBar()
{
    if(m_view->history()->canGoBack()) {
      m_backButton->setEnabled(true);
    }
    if(m_view->history()->canGoForward()) {
      m_forward->setEnabled(true);
    }
}


void WebBrowserWidget::setZoomFactor(qreal factor)
{
    m_zoomFactor=factor; //! \todo ?
}
bool WebBrowserWidget::cursorAtStart()
{
    return true; //! \todo ?
}

bool WebBrowserWidget::cursorAtEnd()
{
    return true; //! \todo ?
}


QVariant WebBrowserWidget::value()
{
    if (dataSource().isEmpty()) {

      return QVariant();
        //not db-aware
        return QVariant();
    }
    //db-aware mode
    
    return m_url;


}

bool WebBrowserWidget::valueIsNull()
{
    return (m_url).isEmpty();

}
void WebBrowserWidget::clear()
{
    setUrl(QString());
}



void WebBrowserWidget::setInvalidState(const QString& displayText)
{
    Q_UNUSED(displayText);

    if (!dataSource().isEmpty()) {
        m_url.clear();
    }
    setReadOnly(true);
}

void WebBrowserWidget::setValueInternal(const QVariant &add, bool removeOld)
{
    Q_UNUSED(add); //compares  
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

    m_urlChanged_enabled= false;		

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
