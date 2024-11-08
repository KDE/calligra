/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoSemanticStylesheetsEditor.h"

#include "ui_KoSemanticStylesheetsEditor.h"

#include "KoDocumentRdf.h"
#include "KoRdfSemanticItem.h"
#include "KoRdfSemanticItemRegistry.h"

#include <kdebug.h>

class Q_DECL_HIDDEN KoSemanticStylesheetsEditor::Private
{
public:
    Ui::KoSemanticStylesheetsEditor *m_ui;
    KoDocumentRdf *m_rdf;
    QTreeWidgetItem *m_systemSheetsParentItem;
    QTreeWidgetItem *m_userSheetsParentItem;
    QMap<QString, QTreeWidgetItem *> m_systemSheetsItems;
    QMap<QString, QTreeWidgetItem *> m_userSheetsItems;
    Private()
        : m_systemSheetsParentItem(0)
        , m_userSheetsParentItem(0)
    {
    }
    ~Private()
    {
        delete m_ui;
    }
};

enum {
    ColName = 0,
    ColSemobj = 1,
    ColVar = 0,
    ColDesc = 1
};

class KoSemanticStylesheetWidgetItem : public QTreeWidgetItem
{
public:
    KoDocumentRdf *m_rdf;
    hKoSemanticStylesheet m_ss;
    hKoRdfSemanticItem m_si;
    KoSemanticStylesheetWidgetItem(KoDocumentRdf *rdf, hKoSemanticStylesheet ss, hKoRdfSemanticItem si, QTreeWidgetItem *parent, int type = Type)
        : QTreeWidgetItem(parent, type)
        , m_rdf(rdf)
        , m_ss(ss)
        , m_si(si)
    {
    }
    KoSemanticStylesheetWidgetItem(KoDocumentRdf *rdf, hKoRdfSemanticItem si, QTreeWidgetItem *parent, int type = Type)
        : QTreeWidgetItem(parent, type)
        , m_rdf(rdf)
        , m_ss(0)
        , m_si(si)
    {
    }

    virtual void setData(int column, int role, const QVariant &value)
    {
        if (m_ss && m_ss->isMutable() && column == ColName) {
            QString newName = value.toString();
            kDebug(30015) << "update to value:" << newName;
            m_ss->name(newName);
            QVariant v = m_ss->name();
            QTreeWidgetItem::setData(column, role, v);
        } else {
            QTreeWidgetItem::setData(column, role, value);
        }
    }
};

KoSemanticStylesheetsEditor::KoSemanticStylesheetsEditor(QWidget *parent, KoDocumentRdf *rdf)
    : KoDialog(parent)
    , d(new Private())
{
    d->m_rdf = rdf;
    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    d->m_ui = new Ui::KoSemanticStylesheetsEditor();
    d->m_ui->setupUi(widget);
    d->m_ui->newStylesheet->setEnabled(false);
    d->m_ui->deleteStylesheet->setEnabled(false);
    connect(d->m_ui->newStylesheet, SIGNAL(clicked()), this, SLOT(newStylesheet()));
    connect(d->m_ui->deleteStylesheet, SIGNAL(clicked()), this, SLOT(deleteStylesheet()));
    connect(d->m_ui->stylesheets,
            SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this,
            SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
    d->m_ui->stylesheets->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->m_ui->stylesheets->sortItems(ColName, Qt::DescendingOrder);
    connect(d->m_ui->definition, SIGNAL(textChanged()), this, SLOT(definitionChanged()));
    // setup system/user parent treewidgets
    d->m_systemSheetsParentItem = new QTreeWidgetItem(d->m_ui->stylesheets);
    d->m_systemSheetsParentItem->setText(ColName, i18n("System"));
    d->m_systemSheetsParentItem->setText(ColSemobj, "");
    d->m_ui->stylesheets->expandItem(d->m_systemSheetsParentItem);

    QTreeWidgetItem *treewidget = 0;
    treewidget = d->m_systemSheetsParentItem;
    const QStringList classNames = KoRdfSemanticItemRegistry::instance()->classNames();
    foreach (const QString &semanticClass, classNames) {
        hKoRdfSemanticItem si(static_cast<KoRdfSemanticItem *>(rdf->createSemanticItem(semanticClass, this).data()));
        KoSemanticStylesheetWidgetItem *item = new KoSemanticStylesheetWidgetItem(rdf, si, treewidget);
        item->setText(ColName, semanticClass);
        d->m_systemSheetsItems[semanticClass] = item;
        d->m_ui->stylesheets->expandItem(item);
    }
    d->m_userSheetsParentItem = new QTreeWidgetItem(d->m_ui->stylesheets);
    d->m_userSheetsParentItem->setText(ColName, i18n("User"));
    d->m_userSheetsParentItem->setText(ColSemobj, "");
    d->m_ui->stylesheets->expandItem(d->m_userSheetsParentItem);

    treewidget = d->m_userSheetsParentItem;
    foreach (const QString &semanticClass, classNames) {
        hKoRdfSemanticItem si(static_cast<KoRdfSemanticItem *>(rdf->createSemanticItem(semanticClass, this).data()));
        KoSemanticStylesheetWidgetItem *item = new KoSemanticStylesheetWidgetItem(rdf, si, treewidget);
        item->setText(ColName, semanticClass);
        d->m_userSheetsItems[semanticClass] = item;
        d->m_ui->stylesheets->expandItem(item);
    }

    // initialize stylesheets tree
    foreach (const QString &semanticClass, classNames) {
        kDebug(30015) << "semanticClass:" << semanticClass;
        hKoRdfSemanticItem p(static_cast<KoRdfSemanticItem *>(rdf->createSemanticItem(semanticClass, this).data()));
        setupStylesheetsItems(semanticClass, p, p->stylesheets(), d->m_systemSheetsItems);
        setupStylesheetsItems(semanticClass, p, p->userStylesheets(), d->m_userSheetsItems, true);
    }
    connect(d->m_ui->variables, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(onVariableActivated(QTableWidgetItem *)));
    d->m_ui->variables->horizontalHeader()->setResizeMode(ColVar, QHeaderView::ResizeToContents);
    d->m_ui->variables->horizontalHeader()->setResizeMode(ColDesc, QHeaderView::ResizeToContents);
}

KoSemanticStylesheetsEditor::~KoSemanticStylesheetsEditor()
{
}

void KoSemanticStylesheetsEditor::setupStylesheetsItems(const QString &semanticClass,
                                                        hKoRdfSemanticItem si,
                                                        const QList<hKoSemanticStylesheet> &ssl,
                                                        const QMap<QString, QTreeWidgetItem *> &m,
                                                        bool editable)
{
    foreach (hKoSemanticStylesheet ss, ssl) {
        kDebug(30015) << "ss:" << ss->name();
        QTreeWidgetItem *parent = m[semanticClass];
        KoSemanticStylesheetWidgetItem *item = new KoSemanticStylesheetWidgetItem(d->m_rdf, ss, si, parent);
        item->setText(ColName, ss->name());
        item->setText(ColSemobj, semanticClass);
        if (editable) {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
}

void KoSemanticStylesheetsEditor::slotOk()
{
}

void KoSemanticStylesheetsEditor::maskButtonsDependingOnCurrentItem(QTreeWidgetItem *current)
{
    bool newEnabled = true;
    bool delEnabled = true;
    for (QMap<QString, QTreeWidgetItem *>::iterator iter = d->m_userSheetsItems.begin(); iter != d->m_userSheetsItems.end(); ++iter) {
        if (iter.value() == current) {
            delEnabled = false;
        }
    }
    if (d->m_userSheetsParentItem == current) {
        newEnabled = false;
        delEnabled = false;
    } else {
        while (current) {
            kDebug(30015) << "current:" << current->text(ColName);
            if (d->m_systemSheetsParentItem == current) {
                newEnabled = false;
                delEnabled = false;
                break;
            }
            current = current->parent();
        }
    }
    d->m_ui->newStylesheet->setEnabled(newEnabled);
    d->m_ui->deleteStylesheet->setEnabled(delEnabled);
}

void KoSemanticStylesheetsEditor::definitionChanged()
{
    if (KoSemanticStylesheetWidgetItem *ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(d->m_ui->stylesheets->currentItem())) {
        if (!ssitem->m_ss) {
            return;
        }
        QString desc = d->m_ui->definition->toPlainText();
        kDebug(30015) << "ss:" << ssitem->m_ss->name() << " desc:" << desc;
        ssitem->m_ss->templateString(desc);
    }
}

struct SignalBlockerRAII {
    QObject *obj;
    bool oldval;
    SignalBlockerRAII(QObject *o)
        : obj(o)
    {
        oldval = obj->blockSignals(true);
    }
    ~SignalBlockerRAII()
    {
        obj->blockSignals(oldval);
    }
};

void KoSemanticStylesheetsEditor::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    SignalBlockerRAII _blocker1(d->m_ui->definition);
    kDebug(30015) << "current:" << current;
    kDebug(30015) << "previous:" << previous;
    maskButtonsDependingOnCurrentItem(current);
    if (previous) {
        QString desc = d->m_ui->definition->toPlainText();
        kDebug(30015) << "desc:" << desc;
        if (KoSemanticStylesheetWidgetItem *ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(previous)) {
            kDebug(30015) << "ssitem, ss?:" << ssitem->m_ss;

            if (ssitem->m_ss) {
                ssitem->m_ss->templateString(desc);
            }
        }
    }
    if (KoSemanticStylesheetWidgetItem *ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(current)) {
        d->m_ui->definition->setPlainText(QString());
        d->m_ui->variables->clear();
        if (!ssitem->m_ss) {
            return;
        }
        d->m_ui->definition->setPlainText(ssitem->m_ss->templateString());
        // update the list of available variables
        QTableWidget *v = d->m_ui->variables;
        v->clear();
        int row = 0;
        QMap<QString, QString> m;
        ssitem->m_si->setupStylesheetReplacementMapping(m);
        QMap<QString, QString>::const_iterator mi = m.constBegin();
        QMap<QString, QString>::const_iterator me = m.constEnd();
        for (; mi != me; ++mi) {
            QTableWidgetItem *item = 0;
            QString varName = mi.key();
            QString desc("t");
            v->setRowCount(row + 1);
            item = new QTableWidgetItem(varName);
            v->setItem(row, ColVar, item);
            item = new QTableWidgetItem(desc);
            v->setItem(row, ColDesc, item);
            ++row;
        }
    }
}

void KoSemanticStylesheetsEditor::newStylesheet()
{
    kDebug(30015);
    if (KoSemanticStylesheetWidgetItem *ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(d->m_ui->stylesheets->currentItem())) {
        if (ssitem->m_ss) {
            ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(ssitem->parent());
            if (!ssitem) {
                return;
            }
        }
        kDebug(30015) << ssitem;
        hKoRdfSemanticItem si = ssitem->m_si;
        hKoSemanticStylesheet ss = si->createUserStylesheet(QString("new stylesheet %1").arg(QDateTime::currentDateTime().toSecsSinceEpoch()), "");
        QTreeWidgetItem *parent = ssitem;
        KoSemanticStylesheetWidgetItem *item = new KoSemanticStylesheetWidgetItem(d->m_rdf, ss, si, parent);
        item->setText(ColName, ss->name());
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        d->m_ui->stylesheets->setCurrentItem(item);
        d->m_ui->stylesheets->scrollToItem(item);
    }
}

void KoSemanticStylesheetsEditor::deleteStylesheet()
{
    kDebug(30015);
    if (KoSemanticStylesheetWidgetItem *ssitem = dynamic_cast<KoSemanticStylesheetWidgetItem *>(d->m_ui->stylesheets->currentItem())) {
        if (!ssitem->m_ss) {
            return;
        }
        hKoSemanticStylesheet sheet = ssitem->m_ss;
        ssitem->m_ss = 0;
        ssitem->m_si->destroyUserStylesheet(sheet);
        ssitem->parent()->removeChild(ssitem);
    }
}

void KoSemanticStylesheetsEditor::onVariableActivated(QTableWidgetItem *item)
{
    QTableWidgetItem *vitem = d->m_ui->variables->item(item->row(), ColVar);
    QString vtext = vitem->text();
    QTextCursor cursor(d->m_ui->definition->textCursor());
    cursor.insertText(vtext);
}

#include <KoSemanticStylesheetsEditor.moc>
