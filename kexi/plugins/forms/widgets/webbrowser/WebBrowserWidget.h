/* <This file is part of the KDE project>
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


#ifndef WEBBROWSERWIDGET_H
#define WEBBROWSERWIDGET_H
#include <QProgressBar>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include "widgetfactory.h"	
#include "container.h"
#include <formeditor/FormWidgetInterface.h>
#include <widget/dataviewcommon/kexiformdataiteminterface.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QUrl>
#include<QWebView>
  
class QWebView;
class QVBoxLayout;
class QWebHistory;
class QHBoxLayout;
class QProgressBar;
class QHBoxLayout;
class QUrl;

class KEXIFORMUTILS_EXPORT WebBrowserWidget :  public QWidget, 
					       public KexiFormDataItemInterface,
					       public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor)
    Q_PROPERTY(QString title READ title)
    
public:
    WebBrowserWidget();    
    ~WebBrowserWidget();
    WebBrowserWidget(QWidget *parent=0);

    inline QString dataSource() const {
        return KexiFormDataItemInterface::dataSource();
    }
    inline QString dataSourcePartClass() const {
        return KexiFormDataItemInterface::dataSourcePartClass();
    }

    inline QString url() const {
	
	return m_url.toString();
    }
    
    inline QString title() const {
	
 	return m_view->title();
    }

    inline qreal zoomFactor() const {
	
	return m_view->zoomFactor();
    }
  
    QWebView* m_view;
    virtual QVariant value();
    virtual void setInvalidState(const QString& displayText);
    virtual bool valueIsNull();
    virtual bool valueIsEmpty();
    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();
    bool isReadOnly() const ;
    virtual void setReadOnly(bool readOnly);  
    void updateToolBar(); 
 
public slots:
    void setDataSource(const QString &ds);
    void setDataSourcePartClass(const QString &ds);
    void setUrl(const QString& url);
    void setZoomFactor(qreal factor);
    void hide_bar();
    
protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld); 
    void setUrl(const QUrl& url);
    bool m_readOnly;
    QUrl m_url;

private:
    QVBoxLayout* v_layout;
    QWebHistory* m_history;
    qreal m_zoomFactor; 
    QProgressBar* m_pbar;
    bool  m_urlChanged_enabled;
    QPushButton* m_backButton;
    QPushButton* m_forward;
    QHBoxLayout* h_layout;
    QPushButton* m_reload;  
    QPushButton* m_stop;  
  
};

#endif 


