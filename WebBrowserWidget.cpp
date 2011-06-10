/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Radosław Wicik <radoslaw@wicik.pl>

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
#include <QApplication>
#include <QLabel>
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
#include <QtGui/QLineEdit>

MyToolBar::MyToolBar(QWidget* parent):QWidget(parent)
{	

    m_backButton = new QPushButton("Back",this);
    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_backButton);
    setLayout(m_layout);
    m_layout->addStretch();

   connect(m_backButton,SIGNAL(clicked()),SLOT(onBackPressed()));
}

void MyToolBar::onBackPressed() //this is the slot,ie function
{
    emit goBack();
//emit signal here
}




WebBrowserWidget::WebBrowserWidget(QWidget *parent)
  : : QWidget(parent),KFormDesigner::FormWidgetInterface()
{

    m_softkeyAction = new QAction( tr("Options"), this );
    m_softkeyAction->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_softkeyAction);

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setStyleSheet("background-color:white; padding: 6px ; color:blue");
    m_lineEdit->setPlaceholderText("Enter url ...");

    m_view = new QWebView(this);
    m_view->load(QUrl("http://www.kde.org"));

    m_layout = new QVBoxLayout();

    m_layout->addWidget(m_lineEdit);
    m_layout->addWidget(m_view);
    m_layout->insertSpacing(1,10);

    //add toolbar
    m_toolbar = new MyToolBar(this);
    m_layout->addWidget(m_toolbar);

    setLayout(m_layout);
    m_layout->addStretch();

    connect(m_lineEdit,SIGNAL(editingFinished()),SLOT(openUrl()));
    connect(m_view,SIGNAL(loadFinished(bool)),SLOT(onLoadFinished(bool)));
   connect(m_toolbar,SIGNAL(goBack()),SLOT(loadPreviousPage()));
}
void MyWebWidget::loadPreviousPage()
{
    if(m_view->history()->canGoBack())
    {
        m_view->history()->back();
    }
}


void MyWebWidget::openUrl()
{
    QString url(m_lineEdit->text());
    if(!url.contains("http://",Qt::CaseInsensitive))
        url.prepend("http://");
    m_view->load(QUrl(url));

}//ok

void MyWebWidget::onLoadFinished(bool finished)
{
    if(finished){
        m_lineEdit->clear();
        m_lineEdit->setPlaceholderText("Enter url ..."); //takes a const qstring parameter
         }
}


#include "WebBrowserWidget.moc"
