/*
 * This file is part of Krita
 *
 * Copyright (c) 2011 Matus Talcik <matus.talcik@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KRITA_WIDGET_OPENCL_H_
#define _KRITA_WIDGET_OPENCL_H_

#include <kis_config_widget.h>

#include <QHBoxLayout>
#include <QTextEdit>

class KisFilter;

class KritaWidgetOpenCL : public KisConfigWidget
{
    Q_OBJECT
public:
    KritaWidgetOpenCL(QWidget * parent);

    virtual void setConfiguration(const KisPropertiesConfiguration*);
    virtual KisPropertiesConfiguration* configuration() const;
    
private:
    QHBoxLayout *layout;
    QTextEdit *textEdit;
};

#endif
