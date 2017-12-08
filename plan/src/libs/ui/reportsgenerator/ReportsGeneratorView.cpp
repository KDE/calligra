/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#include "ReportsGeneratorView.h"

#include "reportgenerator/ReportGenerator.h"
#include "WhatsThis.h"
#include "kptdebug.h"

#include <KoIcon.h>
#include <KoXmlReader.h>

#include <KActionCollection>
#include <KUrlRequester>
#include <KFile>

#include <QAction>
#include <QTreeView>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QModelIndexList>
#include <QStyledItemDelegate>
#include <QString>
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QMap>
#include <QMessageBox>
#include <QDebug>

namespace KPlato
{

#define FULLPATHROLE Qt::UserRole + 123

class TemplateFileDelegate : public QStyledItemDelegate
{
public:
    TemplateFileDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    QMap<QString, QUrl> files;
};

TemplateFileDelegate::TemplateFileDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *TemplateFileDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index);
    qDebug()<<Q_FUNC_INFO;
    return new QComboBox(parent);
}

void TemplateFileDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    qDebug()<<Q_FUNC_INFO<<cb;
    if (!cb) {
        return;
    }
    cb->setEditable(true);
    cb->addItems(files.keys());
    QString file = index.data().toString();
    cb->setCurrentText(file);
}

void TemplateFileDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    qDebug()<<Q_FUNC_INFO<<cb;
    if (cb) {
        QString cfile = index.data().toString();
        QString nfile = cb->currentText();
        qDebug()<<"template file:"<<nfile<<files;
        if (cfile != nfile) {
            model->setData(index, nfile);
            if (files.contains(nfile)) {
                nfile = files[nfile].url();
            }
            model->setData(index, nfile, FULLPATHROLE);
        }
    } else qDebug()<<"  No combo box editor!!";
}

class FileItemDelegate : public QStyledItemDelegate
{
public:
    FileItemDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    QMap<QString, QUrl> files;
};

FileItemDelegate::FileItemDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{
}

QWidget *FileItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    KUrlRequester *u = new KUrlRequester(parent);
    u->setMode(KFile::File);
    return u;
}

void FileItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    KUrlRequester *u = qobject_cast<KUrlRequester*>(editor);
    QString file = index.data().toString();
    if (!file.isEmpty()) {
        u->setUrl(QUrl(file));
    }
}

void FileItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    KUrlRequester *u = qobject_cast<KUrlRequester*>(editor);
    if (u && index.isValid()) {
        model->setData(index, u->url().url());
    }
}

class FileNameExtensionDelegate : public QStyledItemDelegate
{
public:
    FileNameExtensionDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

FileNameExtensionDelegate::FileNameExtensionDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{
}

QWidget *FileNameExtensionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox *cb = new QComboBox(parent);
    for (int i = 0; i < ReportsGeneratorView::addOptions().count(); ++i) {
        cb->addItem(ReportsGeneratorView::addOptions().at(i), ReportsGeneratorView::addTags().value(i));
    }
    return cb;
}

void FileNameExtensionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    if (cb) {
        int idx = ReportsGeneratorView::addTags().indexOf(index.data(Qt::UserRole).toString());
        cb->setCurrentIndex(idx < 0 ? 0 : idx);
    }
}

void FileNameExtensionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox*>(editor);
    if (cb && index.isValid()) {
        model->setData(index, cb->currentData(), Qt::UserRole);
        model->setData(index, cb->currentText());
    }
}

QStringList ReportsGeneratorView::addOptions()
{
    return QStringList() << i18n("Nothing") << i18n("Date") << i18n("Number");
}

QStringList ReportsGeneratorView::addTags()
{
    return QStringList() << "Nothing" << "Date" << "Number";

}

ReportsGeneratorView::ReportsGeneratorView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    debugPlan<<"----------------- Create ReportsGeneratorView ----------------------";
    QVBoxLayout * l = new QVBoxLayout(this);
    l->setMargin(0);
    m_view = new QTreeView(this);
    QStandardItemModel *m = new QStandardItemModel(m_view);
    m->setHorizontalHeaderLabels(QStringList() << i18n("Name") << i18n("Report Template") << i18n("Report File") << i18n("Add"));
    m->setHeaderData(0, Qt::Horizontal, xi18nc("@info:tooltip", "Report name"), Qt::ToolTipRole);
    m->setHeaderData(1, Qt::Horizontal, xi18nc("@info:tooltip", "Report template file name"), Qt::ToolTipRole);
    m->setHeaderData(2, Qt::Horizontal, xi18nc("@info:tooltip", "Name of the generated report file"), Qt::ToolTipRole);
    m->setHeaderData(3, Qt::Horizontal, xi18nc("@info:tooltip", "Information added to filename"), Qt::ToolTipRole);
    m_view->setModel(m);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setRootIsDecorated(false);
    m_view->setAlternatingRowColors(true);

    connect(m_view, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotContextMenuRequested(const QPoint&)));
    l->addWidget(m_view);

    TemplateFileDelegate *del = new TemplateFileDelegate(m_view);
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation, "reports", QStandardPaths::LocateDirectory);
    qDebug()<<"standardpath:"<<path;
    if (!path.isEmpty()) {
        QDir dir(path);
        qDebug()<<dir.entryList(QDir::Files|QDir::QDir::NoDotAndDotDot);
        for (const QString &file : dir.entryList(QDir::Files|QDir::QDir::NoDotAndDotDot)) {
            QUrl url;
            url.setUrl(path + '/' + file);
            qDebug()<<"templates:"<<url<<path<<file;
            del->files.insert(url.fileName(), url);
        }
    }
    m_view->setItemDelegateForColumn(1, del);

    m_view->setItemDelegateForColumn(2, new FileItemDelegate(m_view));

    m_view->setItemDelegateForColumn(3, new FileNameExtensionDelegate(m_view));
    m_view->header()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_view->header()->resizeSection(3, 12);

    connect(m_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(slotSelectionChanged()));
    setupGui();

    Help::add(this,
                   xi18nc("@info:whatsthis",
                          "<title>Add and generate reports</title>"
                          "<para>"
                          "Enables you to add and generate reports based on Open Document (.odf) files."
                          "</para><para>"
                          "You can create a report template using any Open Document text editor."
                          "<nl/><link url='%1'>More...</link>"
                          "</para>", Help::page("Manual/Reports_Generator_View")));
}

void ReportsGeneratorView::setGuiActive(bool activate)
{
    debugPlan<<activate;
    updateActionsEnabled(true);
    ViewBase::setGuiActive(activate);
}

void ReportsGeneratorView::slotCurrentChanged(const QModelIndex &, const QModelIndex &)
{
    slotEnableActions();
}

void ReportsGeneratorView::slotSelectionChanged()
{
    slotEnableActions();
}

QModelIndexList ReportsGeneratorView::selectedRows() const
{
    return m_view->selectionModel()->selectedRows();
}

int ReportsGeneratorView::selectedRowCount() const
{
    return selectedRows().count();
}

void ReportsGeneratorView::slotContextMenuRequested(const QPoint& pos)
{
    debugPlan;
    emit requestPopupMenu("reportsgeneratorview_popup", m_view->mapToGlobal(pos));
}

void ReportsGeneratorView::slotEnableActions()
{
    updateActionsEnabled(isReadWrite());
}

void ReportsGeneratorView::updateActionsEnabled(bool on)
{
    actionAddReport->setEnabled(on);
    actionRemoveReport->setEnabled(on && selectedRowCount() > 0);
    actionGenerateReport->setEnabled(on && selectedRowCount() > 0);
}

void ReportsGeneratorView::setupGui()
{
    // Umpff, adding a specific list name for this view in calligraplan.rc does not work!
    // But reusing an already existing name works, so...
    // Not important atm, the whole setup should be refactored anyway.
    //QString name = "reportsgeneratorview_list";
    QString name = "workpackage_list";

    KActionCollection *coll = actionCollection();

    actionAddReport = new QAction(koIcon("list-add"), i18n("Add Report"), this);
    coll->addAction("add_report", actionAddReport);
    coll->setDefaultShortcut(actionAddReport, Qt::CTRL + Qt::Key_I);
    connect(actionAddReport, SIGNAL(triggered(bool)), SLOT(slotAddReport()));
    addAction(name, actionAddReport);
    addContextAction(actionAddReport);

    actionRemoveReport = new QAction(koIcon("list-remove"), i18n("Remove Report"), this);
    coll->addAction("remove_report", actionRemoveReport);
    coll->setDefaultShortcut(actionRemoveReport, Qt::CTRL + Qt::Key_D);
    connect(actionRemoveReport, SIGNAL(triggered(bool)), SLOT(slotRemoveReport()));
    addAction(name, actionRemoveReport);
    addContextAction(actionRemoveReport);

    actionGenerateReport = new QAction(koIcon("document-export"), i18n("Generate Report"), this);
    coll->addAction("generate_report", actionGenerateReport);
    coll->setDefaultShortcut(actionGenerateReport, Qt::CTRL + Qt::Key_G);
    connect(actionGenerateReport, SIGNAL(triggered(bool)), SLOT(slotGenerateReport()));
    addAction(name, actionGenerateReport);
    addContextAction(actionGenerateReport);

//     createOptionAction();
}


void ReportsGeneratorView::slotOptions()
{
    debugPlan;
//     SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog(this, m_view, this);
//     dlg->addPrintingOptions();
//     connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
//     dlg->show();
//     dlg->raise();
//     dlg->activateWindow();
}

void ReportsGeneratorView::slotAddReport()
{
    debugPlan;
    QAbstractItemModel *m = m_view->model();
    int row = m->rowCount();
    m->insertRow(row);
    QModelIndex idx = m->index(row, 0);
    m->setData(idx, i18n("New report"));
    QModelIndex add = m->index(row, 3);
    m->setData(add, ReportsGeneratorView::addOptions().at(0));
    m->setData(add, ReportsGeneratorView::addTags().at(0), Qt::UserRole);

    m_view->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
    m_view->edit(idx);
    emit optionsModified();
}

void ReportsGeneratorView::slotRemoveReport()
{
    debugPlan<<selectedRows();
    QAbstractItemModel *m = m_view->model();
    QModelIndexList lst = selectedRows();
    if (lst.isEmpty()) {
        return;
    }
    // Assumption: model is flat
    // We must do this in descending row order
    QMap<int, QModelIndex> map;
    for (int i = 0; i < lst.count(); ++i) {
        map.insert(-lst.at(i).row(), lst.at(i)); // sort descending
    }
    for (const QModelIndex &idx : map) {
        Q_ASSERT(!idx.parent().isValid()); // must be flat
        m->removeRow(idx.row(), idx.parent());
    }
    emit optionsModified();
}

void ReportsGeneratorView::slotGenerateReport()
{
    debugPlan;
    QAbstractItemModel *model = m_view->model();
    for (const QModelIndex &idx : selectedRows()) {
        QString name = model->index(idx.row(), 0).data().toString();
        QString tmp = model->index(idx.row(), 1).data(FULLPATHROLE).toString();
        QString file = model->index(idx.row(), 2).data().toString();
        if (tmp.isEmpty()) {
            QMessageBox::information(this, xi18nc("@title:window", "Generate Report"),
                                     i18n("Failed to generate %1."
                                          "\nTemplate file name is empty.", name));
            continue;
        }
        if (file.isEmpty()) {
            debugPlan<<"No files for report:"<<name<<tmp<<file;
            QMessageBox::information(this, xi18nc("@title:window", "Generate Report"),
                                     i18n("Failed to generate %1."
                                          "\nReport file name is empty.", name));
            continue;
        }
        QString addition = model->index(idx.row(), 3).data(Qt::UserRole).toString();
        if (addition == "Date") {
            int dotpos = file.lastIndexOf('.');
            QString date = QDate::currentDate().toString();
            file = file.insert(dotpos, date.prepend('-'));
        } else if (addition == "Number") {
            int dotpos = file.lastIndexOf('.');
            QString fn = file;
            for (int i = 1; QFile::exists(fn); ++i) {
                fn = file.insert(dotpos, QString::number(i).prepend('-'));
            }
            file = fn;
        }
        generateReport(tmp, file);
    }
}

bool ReportsGeneratorView::generateReport(const QString &templateFile, const QString &file)
{
    ReportGenerator rg;
    rg.setReportType("odt"); // TODO: handle different report types
    rg.setTemplateFile(templateFile);
    rg.setReportFile(file);
    rg.setProject(project());
    rg.setScheduleManager(scheduleManager());
    if (!rg.open()) {
        debugPlan<<"Failed to open report generator";
        QMessageBox::warning(this, i18n("Failed to open report generator"), rg.lastError());
        return false;
    }
    if (!rg.createReport()) {
        QMessageBox::warning(this, i18n("Failed to create report"), rg.lastError());
        return false;
    }
    return true;
}

bool ReportsGeneratorView::loadContext(const KoXmlElement &context)
{
    debugPlan;
    m_view->header()->setStretchLastSection((bool)(context.attribute("stretch-last-column", "1").toInt()));
    KoXmlElement e = context.namedItem("sections").toElement();
    if (!e.isNull()) {
        QHeaderView *h = m_view->header();
        QString s("section-%1");
        for (int i = 0; i < h->count(); ++i) {
            if (e.hasAttribute(s.arg(i))) {
                int index = e.attribute(s.arg(i), "-1").toInt();
                if (index >= 0 && index < h->count()) {
                    h->moveSection(h->visualIndex(index), i);
                }
            }
        }
    }
    KoXmlElement parent = context.namedItem("data").toElement();
    if (!parent.isNull()) {
        debugPlan<<"Load data";
        int row = 0;
        QAbstractItemModel *model = m_view->model();
        forEachElement(e, parent) {
            if (e.tagName() != "row") {
                continue;
            }
            model->insertRow(row);
            QString name = e.attribute("name");
            QString tmp = e.attribute("template");
            QString file = e.attribute("file");
            QString add = e.attribute("add");
            QModelIndex idx = model->index(row, 0);
            model->setData(idx, name);
            idx = model->index(row, 1);
            model->setData(idx, tmp, FULLPATHROLE);
            model->setData(idx, QUrl(tmp).fileName());
            idx = model->index(row, 2);
            model->setData(idx, file);
            idx = model->index(row, 3);
            model->setData(idx, add, Qt::UserRole);
            model->setData(idx, ReportsGeneratorView::addOptions().value(ReportsGeneratorView::addTags().indexOf(add)));
            ++row;
        }
    }
    ViewBase::loadContext(context);
    for (int c = 0; c < m_view->header()->count(); ++c) {
        m_view->resizeColumnToContents(c);
    }
    return true;
}

void ReportsGeneratorView::saveContext(QDomElement &context) const
{
    debugPlan;
    context.setAttribute( "stretch-last-column", QString::number(m_view->header()->stretchLastSection()) );
    QDomElement e = context.ownerDocument().createElement("sections");
    context.appendChild(e);
    QHeaderView *h = m_view->header();
    for (int i = 0; i < h->count(); ++i) {
        e.setAttribute(QString("section-%1").arg(i), h->logicalIndex(i));
    }
    QDomElement data = context.ownerDocument().createElement("data");
    context.appendChild(data);
    const QAbstractItemModel *model = m_view->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        e = data.ownerDocument().createElement("row");
        data.appendChild(e);
        QModelIndex idx = model->index(row, 0);
        e.setAttribute("name", idx.data().toString());
        idx = model->index(row, 1);
        e.setAttribute("template", idx.data(FULLPATHROLE).toString());
        idx = model->index(row, 2);
        e.setAttribute("file", idx.data().toString());
        idx = model->index(row, 3);
        e.setAttribute("add", idx.data(Qt::UserRole).toString());
    }
    ViewBase::saveContext(context);
}


} // namespace KPlato
