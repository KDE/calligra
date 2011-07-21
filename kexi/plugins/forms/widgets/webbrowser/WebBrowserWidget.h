/* <The basic code for the web widget in Kexi forms>
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
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include "widgetfactory.h"	
#include "container.h"
#include <formeditor/FormWidgetInterface.h>
#include <plugins/forms/kexiformdataiteminterface.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QUrl>

class QWebView;
class QLineEdit;
class QVBoxLayout;
class QLabel;
class QAction;
class QWebHistory;
class ToolBar; //added
class QHBoxLayout;
class QLabel;
class QUrl;


class KEXIFORMUTILS_EXPORT WebBrowserWidget :  public QWidget, 
					       public KexiFormDataItemInterface,
					       public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString dataSourcePartClass READ dataSourcePartClass WRITE setDataSourcePartClass)
    Q_PROPERTY(QString url READ url WRITE setUrl)

    
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

    virtual QVariant value();
    virtual void setInvalidState(const QString& displayText);
    virtual bool valueIsNull();
    virtual bool valueIsEmpty();
    virtual bool cursorAtStart();
    virtual bool cursorAtEnd();
    virtual void clear();
    bool isReadOnly() const ;
    virtual void setReadOnly(bool readOnly);  


public slots:
    void setDataSource(const QString &ds);
    void setDataSourcePartClass(const QString &ds);
    void setUrl(const QString& url);
    void loadPreviousPage();
    void loadNextPage(); 
    void onreload(); 

protected:
    virtual void setValueInternal(const QVariant& add, bool removeOld); 
    void setUrl(const QUrl& url);
  //  void updateUrl();
    bool m_readOnly;
    QUrl m_url;


private:
    QAction* m_softkeyAction;
    QWebView* m_view;
    QLineEdit* m_lineEdit;
    QLabel *m_label;
    QVBoxLayout* v_layout;
    ToolBar* m_toolbar;
    bool m_urlChanged_enabled;   
    QWebHistory* m_history;
   
};

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    ToolBar(QWidget *parent = 0);

signals: 
    void goBack();
    void goForward();
    void  doreload();
    
private slots:
    void onBackPressed(); 
    void onForward();
    void onReload();
    
private:

    QPushButton* m_backButton;
    QPushButton* m_forward;
    QHBoxLayout* m_layout;
    QPushButton* m_reload;  
};
#endif 


