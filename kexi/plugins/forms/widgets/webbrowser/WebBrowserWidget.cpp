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
//#include <QApplication>
//#include <QLabel>
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
//#include <QtGui/QLineEdit>

//class QPushButton;

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

void ToolBar::onForward() //this is the slot,ie function
{
    emit goForward();
//emit signal here
}


void ToolBar::onReload() //this is the slot,ie function
{
    emit doreload();
//emit signal here
}


void ToolBar::onBackPressed() //this is the slot,ie function
{
    emit goBack();
//emit signal here
}

WebBrowserWidget::WebBrowserWidget(QWidget *parent): QWidget(parent),KexiFormDataItemInterface(),KFormDesigner::FormWidgetInterface()//m_url
{

  
    m_view = new QWebView(this);
    m_view->load(QUrl("http://www.kde.org"));
    v_layout = new QVBoxLayout();
    v_layout->addWidget(m_view);
    m_toolbar = new ToolBar(this);
    v_layout->addWidget(m_toolbar);
    setLayout(v_layout);
    v_layout->addStretch();
   connect(m_view,SIGNAL(loadFinished(bool)),SLOT(onLoadFinished(bool)));
   connect(m_toolbar,SIGNAL(goBack()),SLOT(loadPreviousPage()));
  connect(m_toolbar,SIGNAL(goForward()),SLOT(loadNextPage())); 
  connect(m_toolbar,SIGNAL(doreload()),SLOT(onreload()));
  connect(m_view,SIGNAL(urlChanged(QUrl)),this,SLOT(openurl()));


};

/*void WebBrowserWidget::setValueInternal(const QVariant&, bool b){}
void WebBrowserWidget::setInvalidState(const QString& q){}
void  WebBrowserWidget::setReadOnly(bool b1){}
bool    WebBrowserWidget::valueIsNull(){return true;}
bool   WebBrowserWidget::valueIsEmpty(){return true;}
bool    WebBrowserWidget::cursorAtStart(){return true;}
bool    WebBrowserWidget::cursorAtEnd(){return true;}
void    WebBrowserWidget::clear(){}
QVariant WebBrowserWidget::value(){return QUrl;}
*/
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

void WebBrowserWidget::onreload()
{
   m_view->reload();
}

void WebBrowserWidget::seturl(QUrl m_url)
{
       //if (!url.startsWith("http://",Qt::CaseInsensitive))
       // url.prepend("http://");
    m_view->load(m_url);
}//ok

void WebBrowserWidget::openUrl()
{
	seturl(m_url);
}



/*void WebBrowserWidget::onLoadFinished(bool finished)
{
    if(finished){
        m_lineEdit->clear();
        m_lineEdit->setPlaceholderText("Enter url ..."); //takes a const qstring parameter
         }
}*/
//K_EXPORT_KEXI_FORM_WIDGET_FACTORY_PLUGIN(StdWidgetFactory, stdwidgets)
#include "WebBrowserWidget.moc"
