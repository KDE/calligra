/*
    <The basic code for the web widget in Kexi forms>
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
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QTextBrowser>
#include <QWebPage>
#include <QtGui/QWidget>
#include <QtGui/QApplication>

ToolBar::ToolBar(QWidget* parent):QWidget(parent)
{	

    m_backButton = new QPushButton("Back",this);
    m_forward= new QPushButton("Forward",this);
    m_reload=new QPushButton("Reload",this);

    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_backButton);
    m_layout->addWidget(m_forward);
    m_layout->addWidget(m_reload);
    setLayout(m_layout);
    m_layout->addStretch();

    connect(m_backButton,SIGNAL(clicked()),SLOT(onBackPressed()));
    connect(m_forward,SIGNAL(clicked()),SLOT(onForward()));
    connect(m_reload,SIGNAL(clicked()),SLOT(onReload()));


};

void ToolBar::onForward() 
{
    emit goForward();
//emit signal here
}


void ToolBar::onReload() 
{
    emit doreload();

}


void ToolBar::onBackPressed() 
{
    emit goBack();

}

WebBrowserWidget::WebBrowserWidget(QWidget *parent)
        : QWidget(parent),KexiFormDataItemInterface()
        ,KFormDesigner::FormWidgetInterface()
        //,m_url
	,m_readOnly(false)
	,m_urlChanged_enabled(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(sizeHint().height());
    setMinimumWidth(minimumHeight());

    m_softkeyAction = new QAction( tr("Options"), this );
    m_softkeyAction->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_softkeyAction);
 //   m_url=new Qurl();
    m_view = new QWebView(this);
    m_view->load(QUrl("http://www.kde.org"));
    v_layout = new QVBoxLayout();
    v_layout->addWidget(m_view);
    m_toolbar = new ToolBar(this);
    v_layout->addWidget(m_toolbar);
    setLayout(v_layout);
    v_layout->addStretch();
    connect(m_toolbar,SIGNAL(goBack()),SLOT(loadPreviousPage()));
    connect(m_toolbar,SIGNAL(goForward()),SLOT(loadNextPage())); 
    connect(m_toolbar,SIGNAL(doreload()),SLOT(onreload()));
    connect(m_view,SIGNAL(urlChanged(QUrl)),this,SLOT(setUrl(const QUrl)));


};

WebBrowserWidget::~WebBrowserWidget()
{

}


WebBrowserWidget::WebBrowserWidget()  
{
}

void WebBrowserWidget::loadPreviousPage()
{
    if(m_view->history()->canGoBack())
    {
        m_view->history()->back();
    }
}


void WebBrowserWidget::loadNextPage()
{
    if(m_view->history()->canGoForward())
    {
        m_view->history()->forward();
    }
}

void WebBrowserWidget::setDataSourcePartClass(const QString &ds) {
        KexiFormDataItemInterface::setDataSourcePartClass(ds);
    }
 


void WebBrowserWidget::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    
}
void WebBrowserWidget::onreload()
{
    m_view->reload();
}

void WebBrowserWidget::setUrl(const QUrl& url)
{
     m_url=url;
     m_view->load(m_url);
}//ok


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
    setUrl(QUrl("www.google.com"));
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
    m_urlChanged_enabled= false;		//if removeold is true then change the Url to value of add as specified in kexidataitem interface.cpp

    if (removeOld)
        { 			//set property editor to add
	 setUrl(add.toUrl()); 
	}       
    else
        { setUrl(QUrl( m_origValue.toString() + add.toString())) ;//else put value of add to current value of m_origValue
     	//setUrl(m_url);
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
