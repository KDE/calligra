/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#ifndef KEXIMOBILETOOLBAR_H
#define KEXIMOBILETOOLBAR_H

#include <QToolBar>

class QVBoxLayout;
class QPushButton;
class KexiRecordNavigatorHandler;

class KexiMobileToolbar : public QToolBar
{
    Q_OBJECT
public:
    KexiMobileToolbar(QWidget* parent = 0);
    virtual ~KexiMobileToolbar();

    void setRecordHandler(KexiRecordNavigatorHandler *handler);
    
private:

    QAction *m_openFileAction;
    QAction *m_gotoNavigatorAction;
    
    QAction *m_previousRecord;
    QAction *m_nextRecord;
    QAction *m_recordNumber;
    
    KexiRecordNavigatorHandler *m_recordHandler;
    
    void updatePage();

private slots:
  void openFileClicked();
  void gotoNavigatorClicked();
  
  //Record Navigation
  void recordPrevious();
  void recordNext();
  
signals:
    void pageOpenFile();
    void pageNavigator();
    void pageItem();
    
};

#endif // KEXIMOBILETOOLBAR_H
