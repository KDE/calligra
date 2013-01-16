/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
  */

#include "KexiStartupDialogTemplatesPage.h"

#include <core/kexi.h>
#include <core/kexitemplateloader.h>

#include <QPixmap>
#include <Q3Header>

#ifdef KEXI_SHOW_UNIMPLEMENTED
#define KEXI_STARTUP_SHOW_TEMPLATES
#define KEXI_STARTUP_SHOW_RECENT
#endif

/*QPixmap createIcon()
{

}*/

/*QString createText(const QString& name, const QString& description)
{
  QString txt = "<H2>" + name + "</H2>";
  if (description.isEmpty())
  return name + description
}*/

//! @internal
class TemplateItem : public K3ListViewItem
{
public:
    TemplateItem(Q3ListView* parent, const QString& aFilename,
                 const QString& name, const QString& description, const KIcon& icon,
                 const KexiProjectData::AutoOpenObjects& aAutoopenObjects)
            : K3ListViewItem(parent, name + "\n" + description)
            , autoopenObjects(&aAutoopenObjects)
            , filename(aFilename)
    {
        setPixmap(0, icon.pixmap(32, 32));
    }
    ~TemplateItem() {}

    const KexiProjectData::AutoOpenObjects* autoopenObjects;
    QString filename;
};

//-----------------------

class KexiStartupDialogTemplatesPage::Private
{
public:
    Private()
            : popuplated(false) {
    }

    KexiProjectData::AutoOpenObjects dummyAutoOpenObject;
    bool popuplated;
};

//-----------------------

KexiStartupDialogTemplatesPage::KexiStartupDialogTemplatesPage(QWidget * parent)
        : K3ListView(parent)
        , d(new Private)
{
    setObjectName("KexiStartupDialogTemplatesPage");
    addColumn(QString());
    header()->hide();
    setColumnWidthMode(0, Maximum);
    setResizeMode(LastColumn);
    setItemMargin(6);
    connect(this, SIGNAL(executed(Q3ListViewItem*)), this, SLOT(slotExecuted(Q3ListViewItem*)));
}

KexiStartupDialogTemplatesPage::~KexiStartupDialogTemplatesPage()
{
    delete d;
}

void KexiStartupDialogTemplatesPage::populate()
{
    if (d->popuplated)
        return;
    d->popuplated = true;
    KexiTemplateInfoList list = KexiTemplateLoader::loadListInfo();
    foreach(const KexiTemplateInfo& info, list) {
        new TemplateItem(this, info.filename, info.name,
                         info.description, info.icon, info.autoopenObjects);
    }
    if (firstChild())
        setSelected(firstChild(), true);

// templates = new KIconView(this, "templates");
// templates->setItemsMovable(false);
// templates->setShowToolTips(false);
// info = new KTextBrowser(this,"info");
// setResizeMode(templates,KeepSize);
// setResizeMode(info,KeepSize);
// connect(templates,SIGNAL(selectionChanged(QIconViewItem*)),this,SLOT(itemClicked(QIconViewItem*)));
}

/*
void TemplatesPage::addItem(const QString& key, const QString& name,
  const QString& description, const QPixmap& icon)
{
  TemplateItem *item = new TemplateItem(templates, name, icon);
  item->key=key;
  item->name=name;
  item->description=description;
}

void TemplatesPage::itemClicked(QIconViewItem *item) {
  if (!item) {
    info->setText("");
    return;
  }
  QString t = QString("<h2>%1</h2><p>%2</p>")
    .arg(static_cast<TemplateItem*>(item)->name)
    .arg(static_cast<TemplateItem*>(item)->description);
#ifndef DB_TEMPLATES
  t += QString("<p>") + i18n("We are sorry, templates are not yet available.") +"</p>";
#endif

  info->setText( t );
}*/

QString KexiStartupDialogTemplatesPage::selectedFileName() const
{
    TemplateItem* templateItem = static_cast<TemplateItem*>(selectedItem());
    return templateItem ? templateItem->filename : QString();
}

const KexiProjectData::AutoOpenObjects&
KexiStartupDialogTemplatesPage::autoopenObjectsForSelectedTemplate() const
{
    TemplateItem* templateItem = static_cast<TemplateItem*>(selectedItem());
    return templateItem ? *templateItem->autoopenObjects : d->dummyAutoOpenObject;
}

void KexiStartupDialogTemplatesPage::slotExecuted(Q3ListViewItem* item)
{
    TemplateItem* templateItem = static_cast<TemplateItem*>(item);
    if (!templateItem)
        return;

    emit selected(templateItem->filename);
}

#include "KexiStartupDialogTemplatesPage.moc"
