/***************************************************************************
 * KoScriptManager.cpp
 * This file is part of the KDE project
 * copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
 * copyright (C) 2006-2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "KoScriptManager.h"

#include "KoScriptManagerAdd.h"
#include "KoKrossDebug.h"

#include <kross/core/manager.h>
#include <kross/core/actioncollection.h>
#include <kross/ui/model.h>
#include <kross/ui/view.h>

#include <KoIcon.h>

#include <klocalizedstring.h>

#include <QStandardPaths>
#include <QBoxLayout>

class KoScriptManagerView : public Kross::ActionCollectionView
{
public:
    KoScriptManagerView(KoScriptManagerCollection *collection) : Kross::ActionCollectionView(collection)
    {
        setDragEnabled(true);
        setAcceptDrops(true);

        Kross::ActionCollectionModel::Mode modelmode = Kross::ActionCollectionModel::Mode(Kross::ActionCollectionModel::Icons | Kross::ActionCollectionModel::ToolTips | Kross::ActionCollectionModel::UserCheckable);
        Kross::ActionCollectionModel *model = new Kross::ActionCollectionModel(this, Kross::Manager::self().actionCollection(), modelmode);
        setModel(model);
        //selectionModel();
    }

    ~KoScriptManagerView() override {}

    Kross::ActionCollectionModel *model() const
    {
            return static_cast<Kross::ActionCollectionModel*>(Kross::ActionCollectionView::model());
    }

    void slotAdd() override
    {
        Kross::ActionCollection *collection = model()->rootCollection();
        if (itemSelection().count() == 1) {
            collection = Kross::ActionCollectionModel::collection(itemSelection().indexes().first());
        }
        KoScriptManagerAddWizard wizard(this, collection);
        int result = wizard.exec();
        Q_UNUSED(result);
    }
    void slotRemove() override
    {
        if (itemSelection().isEmpty()) {
            return;
        }
        // get the actions/collections, indexes will change when things are removed
        QList<Kross::Action*> actions;
        QList<Kross::ActionCollection*> collections;
        foreach (const QModelIndex &idx, itemSelection().indexes()) {
            Kross::Action *a =  model()->action(idx);
            if (a) {
                actions << a;
            } else {
                Kross::ActionCollection *c = model()->collection(idx);
                if (c) {
                    collections << c;
                }
            }
        }
        foreach (Kross::Action *action, actions) {
            QModelIndex idx = model()->indexForAction(action);
            if (idx.isValid()) {
                //debugKoKross<<"action:"<<action->name();
                delete action;
            }
        }
        foreach (Kross::ActionCollection *collection, collections) {
            QModelIndex idx = model()->indexForCollection(collection);
            if (idx.isValid()) {
                //debugKoKross<<"collection:"<<collection->name();
                delete collection;
            }
        }
    }
};

KoScriptManagerCollection::KoScriptManagerCollection(QWidget *parent)
    : QWidget(parent),
    m_modified(false)
{
    QHBoxLayout *mainlayout = new QHBoxLayout();
    mainlayout->setMargin(0);
    setLayout(mainlayout);

    m_view = new KoScriptManagerView(this);
    mainlayout->addWidget(m_view);

    QWidget *btnwidget = new QWidget(this);
    QVBoxLayout *btnlayout = new QVBoxLayout();
    btnlayout->setMargin(0);
    btnwidget->setLayout(btnlayout);
    mainlayout->addWidget(btnwidget);

    m_view->createButton(btnwidget, "run");
    m_view->createButton(btnwidget, "stop");

    QFrame *hr1 = new QFrame(btnwidget);
    hr1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    btnlayout->addWidget(hr1, 0);

    m_view->createButton(btnwidget, "edit");
    m_view->createButton(btnwidget, "add");
    m_view->createButton(btnwidget, "remove");

    btnlayout->addStretch(1);
    m_view->expandAll();
}

KoScriptManagerCollection::~KoScriptManagerCollection()
{
}

KoScriptManagerDialog::KoScriptManagerDialog()
    : KoDialog()
{
    setCaption(i18n("Script Manager"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    setButtonText(KoDialog::Ok, i18n("Save"));
    setButtonIcon(KoDialog::Ok, koIcon("document-save"));
    m_collection = new KoScriptManagerCollection(mainWidget());
    setMainWidget(m_collection);
    resize(QSize(520, 380).expandedTo(minimumSizeHint()));
    connect(this, &QDialog::accepted, this, &KoScriptManagerDialog::slotAccepted);
}

KoScriptManagerDialog::~KoScriptManagerDialog()
{
}

void KoScriptManagerDialog::slotAccepted()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (! dir.isEmpty()) {
        const QString scriptsDir = dir + QStringLiteral("/scripts/");
        QDir().mkpath(scriptsDir);

        const QString file = QFileInfo(scriptsDir, "scripts.rc").absoluteFilePath();
        QFile f(file);
        if (f.open(QIODevice::WriteOnly))
            if (Kross::Manager::self().actionCollection()->writeXml(&f))
                debugKoKross<<"Successfully saved file:"<<file;
        f.close();
    }
}
