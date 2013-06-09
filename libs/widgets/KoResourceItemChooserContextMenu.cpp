
 /* This file is part of the KDE project
 *    Copyright (c) 2013 Sascha Suelzer <s_suelzer@lavabit.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License as published by the Free Software Foundation; either
 *    version 2 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public License
 *    along with this library; see the file COPYING.LIB.  If not, write to
 *    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  * Boston, MA 02110-1301, USA.
 * */
#include <QDebug>
#include <QLabel>
#include <QGridLayout>

#include <KoIcon.h>
#include <klocale.h>

#include "KoResourceItemChooserContextMenu.h"
#include "KoResource.h"

ContextMenuExistingTagAction::ContextMenuExistingTagAction(KoResource* resource, QString tag, QObject* parent)
: QAction(parent)
, m_resource(resource)
, m_tag(tag)
{
    setText(tag);
    connect (this, SIGNAL(triggered()),
             this, SLOT(onTriggered()));
}

ContextMenuExistingTagAction::~ContextMenuExistingTagAction()
{
}

void ContextMenuExistingTagAction::onTriggered()
{
    emit triggered(m_resource,m_tag);
}
ContextMenuNewTagAction::~ContextMenuNewTagAction()
{
}

ContextMenuNewTagAction::ContextMenuNewTagAction(KoResource* resource, QObject* parent)
    :QWidgetAction (parent)
    ,m_resource(resource)
{

    QWidget* pWidget = new QWidget (NULL);
    QHBoxLayout* pLayout = new QHBoxLayout();
    QLabel * label = new QLabel(NULL);
    QIcon icon = koIcon("document-new");
    QPixmap pixmap = QPixmap(icon.pixmap(16,16));
    label->setPixmap(pixmap);
    m_editBox = new KLineEdit(NULL);
    pLayout->addWidget(label);
    pLayout->addWidget(m_editBox);
    pWidget->setLayout(pLayout);
    m_resource = resource;

    setDefaultWidget(pWidget);
    m_editBox->setClickMessage(i18n("New tag"));
    connect (m_editBox, SIGNAL(returnPressed(QString)),
             this, SLOT(onTriggered(QString)));
}

void ContextMenuNewTagAction::onTriggered(const QString & tagName)
{
    if (!tagName.isEmpty()) {
        m_tag = tagName;
        emit triggered(m_resource,m_tag);
        this->parentWidget()->close();
    }
}

class KoResourceItemChooserContextMenu::Private
{
public:
    KoResource * resource;
    QStringList resourceTags;
    QStringList allTags;
    QString activeTag;
};

KoResourceItemChooserContextMenu::KoResourceItemChooserContextMenu(QWidget* parent)
: QMenu(parent)
, d(new Private())
{

}

KoResourceItemChooserContextMenu::~KoResourceItemChooserContextMenu()
{
    delete d;
}

void KoResourceItemChooserContextMenu::resource(KoResource* resource)
{
    d->resource = resource;
}

void KoResourceItemChooserContextMenu::allTags(const QStringList& allTags)
{
    d->allTags = allTags;
}

void KoResourceItemChooserContextMenu::resourceTags(const QStringList& resourceTags)
{
    d->resourceTags = resourceTags;
}

void KoResourceItemChooserContextMenu::activeTag(const QString& tag)
{
    d->activeTag = tag;
}

void KoResourceItemChooserContextMenu::onAddTagToResource(KoResource* resource, const QString& tag)
{
    emit addTagToResource(resource, tag);
}

void KoResourceItemChooserContextMenu::onCreateNewResourceTag(KoResource* resource, const QString& tag)
{
    emit createNewResourceTag(resource, tag);
}

void KoResourceItemChooserContextMenu::onRemoveTagFromResource(KoResource* resource, const QString& tag)
{
    emit removeTagFromResource(resource, tag);
}

QAction* KoResourceItemChooserContextMenu::exec(const QPoint& p, QAction* action)
{
    QImage image = d->resource->image();
    QIcon icon(QPixmap::fromImage(image));
    QAction * label = new QAction(d->resource->name(), this);
    label->setIcon(icon);

    addAction(label);

    QMenu * removableTagsMenu;
    QMenu * assignableTagsMenu;

    QStringList removables = d->resourceTags;
    QStringList assignables = d->allTags;

    removables.sort();
    assignables.sort();

    assignableTagsMenu = addMenu(koIcon("list-add"),i18n("Assign to tag:"));

    if (!removables.isEmpty()) {
        addSeparator();
        QString curTag = d->activeTag;
        if (removables.contains(curTag)) {
            assignables.removeAll(curTag);
            removables.removeAll(curTag);
            ContextMenuExistingTagAction * removeTagAction = new ContextMenuExistingTagAction(d->resource, curTag, this);
            removeTagAction->setText(i18n("Remove from this tag"));
            removeTagAction->setIcon(koIcon("list-remove"));

            connect(removeTagAction, SIGNAL(triggered(KoResource*,QString)),
                    this, SLOT(onRemoveTagFromResource(KoResource*,QString)));
            addAction(removeTagAction);
        }
        if (!removables.isEmpty()) {
            removableTagsMenu = addMenu(koIcon("list-remove"),i18n("Remove from other tag"));
            foreach (const QString &tag, removables) {
                assignables.removeAll(tag);
                ContextMenuExistingTagAction * removeTagAction = new ContextMenuExistingTagAction(d->resource, tag, this);

                connect(removeTagAction, SIGNAL(triggered(KoResource*,QString)),
                        this, SLOT(onRemoveTagFromResource(KoResource*,QString)));
                removableTagsMenu->addAction(removeTagAction);
            }
        }
    }


    foreach (const QString &tag, assignables) {
        ContextMenuExistingTagAction * addTagAction = new ContextMenuExistingTagAction(d->resource, tag, this);

        connect(addTagAction, SIGNAL(triggered(KoResource*,QString)),
                this, SLOT(onAddTagToResource(KoResource*,QString)));
        assignableTagsMenu->addAction(addTagAction);
    }
    assignableTagsMenu->addSeparator();

    ContextMenuNewTagAction * addTagAction = new ContextMenuNewTagAction(d->resource, this);
    connect(addTagAction, SIGNAL(triggered(KoResource*,QString)),
            this, SLOT(onCreateNewResourceTag(KoResource*,QString)));
    assignableTagsMenu->addAction(addTagAction);


    return QMenu::exec(p, action);
}
