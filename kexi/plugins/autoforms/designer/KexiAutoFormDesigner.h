/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <piggz1@gmail.com>

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


#ifndef KEXIAUTOFORMDESIGNER_H
#define KEXIAUTOFORMDESIGNER_H

#include <QWidget>
#include <QDomElement>

class QComboBox;
class QVBoxLayout;
class KexiDataSourceComboBox;

class KexiAutoFormDesigner : public QWidget
{
    Q_OBJECT
public:
    KexiAutoFormDesigner(QWidget *parent = 0);
    KexiAutoFormDesigner(QDomElement document, QWidget* parent = 0);
    
    
    
    virtual ~KexiAutoFormDesigner();
    
private:
    QVBoxLayout *m_layout;
    KexiDataSourceComboBox *m_dataSource;
    
    void setupUi();
    
};

#endif // KEXIAUTOFORMDESIGNER_H
