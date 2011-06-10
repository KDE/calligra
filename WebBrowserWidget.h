/* This file is part of the KDE project
   Copyright (C) 2011 by Radosław Wicik <radoslaw@wicik.pl>

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


#ifndef WEBBROWSERWIDGET_H
#define WEBBROWSERWIDGET_H
#include <QtGui/QWidget>
#include "widgetfactory.h"	//these already inherit Qt headers reqd
#include "container.h"
#include "FormWidgetInterface.h"



class QWebView;
class QLineEdit;
class QVBoxLayout;
class QLabel;
class QAction;
class QWebHistory;
class MyToolBar; //added
class QHBoxLayout;
class QPushButton;
class QLabel;



class WebBrowserWidget :  public QWidget , public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    
public:
    WebBrowserWidget(QWidget *parent=0);


private slots:
    void openUrl();
    void onLoadFinished(bool finished);
     void loadPreviousPage();
     private:
    QAction* m_softkeyAction;
    QWebView* m_view;
    QLineEdit* m_lineEdit;
    QLabel* m_label;
    QVBoxLayout* m_layout;
    ToolBar* m_toolbar;
    QWebHistory* m_history;
};

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    ToolBar(QWidget *parent = 0);
   // ~MyToolBar();

signals:
   void goBack();
    
private slots:
    void onBackPressed();   
  
private:
    QPushButton* m_backButton;
    QHBoxLayout* m_layout;
  
};
#endif // WEBBROWSERWIDGET_H



