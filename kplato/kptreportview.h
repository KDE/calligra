/* This file is part of the KDE project
   Copyright (C) 2003 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTREPORTVIEW_H
#define KPTREPORTVIEW_H

#include "kptcontext.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptduration.h"
#include <qsplitter.h>
#include <QString>

#include <k3listview.h>

class KAction;
class KPrinter;

namespace Kugar
{
  class MReportViewer;
} // Kugar namespace

//class QString;
class QStringList;
class QDomDocument;
class QDomNode;
class QIODevice;

namespace KPlato
{

class View;
class Node;
class KugarReportViewer;
class ReportTagsPrivate;

class ReportView : public QSplitter
{
    Q_OBJECT

 public:

    ReportView(View *view, QWidget *parent);

    ~ReportView();

	void zoom(double /*zoom*/) {}

    void draw(const QString &report);
    View *mainView() const { return m_mainview; }

    void setup(KPrinter &printer);
    void print(KPrinter &printer);

    void setReportData();

    void getTemplateFile(const QString &tpl);
    void openTemplateFile(const QString &file);
    void loadTemplate(QIODevice &dev);
    void loadTemplate(QDomDocument &doc);
    void handleHeader(QDomNode &node);
    void handleDetail(QDomElement &elem);
    void handleKPlato(QDomElement &elem);
    void replaceTags(QDomNode &node);

    QString setReportDetail();
    QString setTaskChildren(Node *node);
    QString setTaskDetail(Node *node);
    QStringList getProperties(QDomElement &elem);

    QString setResourceDetail(Resource *res);
    QString setResourceGroupDetail(ResourceGroup *group);

    QString setDetail(const QString &source, QStringList &properties, QString &level);

    virtual bool setContext(Context::Reportview &context);
    virtual void getContext(Context::Reportview &context) const;

    void enableNavigationBtn();
    
signals:
    void setFirstPageActionEnabled(bool);
    void setNextPageActionEnabled(bool);
    void setPriorPageActionEnabled(bool);
    void setLastPageActionEnabled(bool);

public slots:
	void slotFirstPage();
	void slotNextPage();
	void slotPrevPage();
	void slotLastPage();

protected slots:
    void slotReportListClicked(Q3ListViewItem* item);
    void slotReportListSelectionChanged(Q3ListViewItem* item);
    
private:
    class ReportItem : public K3ListViewItem {
    public:
        ReportItem(K3ListView *p, QString name, QString _url) 
            : K3ListViewItem(p, name),
              url(_url)
        {}
        QString url;
    };
    void initReportList();
    
private:
    View *m_mainview;
    K3ListView *m_reportList;
    KugarReportViewer *m_reportview;
    int m_defaultFontSize;

    QDomDocument templateDoc;

    ReportTagsPrivate *m_reportTags;
};

}  //KPlato namespace

#endif
