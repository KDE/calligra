/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef KPLATO_REPORTPAGE_H
#define KPLATO_REPORTPAGE_H

#include <qwidget.h>

class QPixmap;
class ORODocument;

namespace KPlato
{

class ReportPage : public QWidget
{
    Q_OBJECT
public:
    ReportPage(QWidget *parent, ORODocument *r);

    ~ReportPage();

    void renderPage(int);

public slots:
    virtual void paintEvent(QPaintEvent*);

private:
    ORODocument *m_reportDocument;
    int m_page;
    bool m_repaint;
    QPixmap *m_pixmap;
};

}

#endif

