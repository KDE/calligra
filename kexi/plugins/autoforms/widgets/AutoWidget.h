/*
    Kexi Auto Form Plugin*
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

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


#ifndef AUTOWIDGETBASE_H
#define AUTOWIDGETBASE_H

#include <QWidget>
#include <QVariant>
#include <dataviewcommon/kexiformdataiteminterface.h>

class AutoForm;
class QLabel;
class QHBoxLayout;
namespace KexiDB {
class Field;
}


class AutoWidget : public QWidget, public KexiFormDataItemInterface
{
public:
    virtual ~AutoWidget();
    
protected:
    explicit AutoWidget(AutoForm* parent);
    
    void setLabel(const QString& label);
    void setWidget(QWidget* widget);
    
    //virtual void focusInEvent(QFocusEvent* );
    virtual bool eventFilter(QObject *obj, QEvent *event);
    
private:
    QLabel *m_fieldLabel;
    QHBoxLayout *m_layout;
    QWidget *m_widget;
    
    AutoForm* m_parent;
    
protected slots:
    void gotFocus();
};

#endif // AUTOWIDGETBASE_H
