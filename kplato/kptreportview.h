/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTREPORTVIEW_H
#define KPTREPORTVIEW_H

#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptduration.h"
#include <qwidget.h>
#include <qstring.h>

class KPTView;
class KPTNode;
class KReportViewer;
class KPrinter;
class KAction;
//class QString;
class QStringList;
class QDomDocument;
class QDomNode;
class QIODevice;

class ReportTagsPrivate;

class KPTReportView : public QWidget
{
    Q_OBJECT

 public:

    KPTReportView( KPTView *view, QWidget *parent);

    ~KPTReportView();

	void zoom(double /*zoom*/) {}

    void draw(const QString &report);
    KPTView *mainView() { return m_mainview; }

    void print(KPrinter &printer);

    void setReportData();

    void getTemplateFile(const QString &tpl);
    void openTemplateFile(const QString &file);
    void loadTemplate(QIODevice &dev);
    void loadTemplate(QDomDocument &doc);
    void handleHeader(QDomNode &node);
    void handleDetailHeader(QDomNode &node);
    void handleDetail(QDomElement &elem);
    void replaceTags(QDomNode &node);

    QString setReportDetail();
    QString setTaskChildren(KPTNode *node);
    QString setTaskDetail(KPTNode *node);
    QStringList getProperties(QDomElement &elem);

    QString setResourceDetail(KPTResource *res);

    QString setDetail(QString source, QStringList &properties, QString &level);

public slots:
	void slotFirstPage();
	void slotNextPage();
	void slotPrevPage();
	void slotLastPage();


private:
    KPTView *m_mainview;
    KReportViewer *m_reportview;
    int m_defaultFontSize;

    QDomDocument templateDoc;

    ReportTagsPrivate *m_reportTags;
};
 #endif
