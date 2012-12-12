/* This file is part of the KDE project
   Copyright (C) 2006-2012 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbutils.h"

#include <QApplication>
#include <QFontMetrics>

#include <KMenu>
#include <KIconLoader>
#include <KIconEffect>

#include <db/queryschema.h>
#include <db/utils.h>

#include <KoIcon.h>

#include <formeditor/widgetlibrary.h>
#include <kexiutils/utils.h>
#include "../kexiformpart.h"
#include "../kexiformmanager.h"
#include <widget/utils/kexicontextmenuutils.h>


class KexiDBWidgetContextMenuExtender::Private
{
public:
    Private(KexiDataItemInterface* iface_)
      : iface(iface_)
      , contextMenuHasTitle(false)
    {
    }

    KexiDataItemInterface* iface;
    QPointer<QMenu> contextMenu;
    QPointer<QAction> titleAction;
    bool contextMenuHasTitle; //!< true if KPopupTitle has been added to the context menu.

};


//! Static data for kexi forms
struct KexiFormStatics
{
    QPixmap dataSourceTagIcon() {
        initDataSourceTagIcon();
        return m_dataSourceTagIcon;
    }

    QPixmap dataSourceRTLTagIcon() {
        initDataSourceTagIcon();
        return m_dataSourceRTLTagIcon;
    }

    void initDataSourceTagIcon() {
        if (!m_dataSourceTagIcon.isNull())
            return;
        QFontMetrics fm(QApplication::fontMetrics());
        int size = IconSize(KIconLoader::Small);
        if (size < KIconLoader::SizeSmallMedium && fm.height() >= KIconLoader::SizeSmallMedium)
            size = KIconLoader::SizeSmallMedium;
        m_dataSourceTagIcon = SmallIcon(koIconName("data-source-tag"), size);
        KIconEffect::semiTransparent(m_dataSourceTagIcon);
        m_dataSourceRTLTagIcon = QPixmap::fromImage(m_dataSourceTagIcon.toImage().mirrored(true /*h*/, false /*v*/));
    }
private:
    QPixmap m_dataSourceTagIcon;
    QPixmap m_dataSourceRTLTagIcon;
};

K_GLOBAL_STATIC(KexiFormStatics, g_KexiFormStatics)

//-------

QColor KexiFormUtils::lighterGrayBackgroundColor(const QPalette& palette)
{
    return KexiUtils::blendedColors(
        palette.color(QPalette::Active, QPalette::Background),
        palette.color(QPalette::Active, QPalette::Base),
        1, 2);
}

QPixmap KexiFormUtils::dataSourceTagIcon()
{
    return g_KexiFormStatics->dataSourceTagIcon();
}

QPixmap KexiFormUtils::dataSourceRTLTagIcon()
{
    return g_KexiFormStatics->dataSourceRTLTagIcon();
}

//-------

KexiDBWidgetContextMenuExtender::KexiDBWidgetContextMenuExtender(QObject* parent, KexiDataItemInterface* iface)
        : QObject(parent)
	, d(new Private(iface))
{
}

KexiDBWidgetContextMenuExtender::~KexiDBWidgetContextMenuExtender()
{
}

void KexiDBWidgetContextMenuExtender::exec(QMenu *menu, const QPoint &globalPos)
{
    KMenu kmenu;
    foreach(QAction* action, menu->actions()) {
        kmenu.addAction(action);
    }
    createTitle(&kmenu);
    kmenu.exec(globalPos);
}

void KexiDBWidgetContextMenuExtender::createTitle(KMenu *menu)
{
    if (!menu)
        return;

    QString icon;
    if (dynamic_cast<QWidget*>(d->iface)) {
        icon = KexiFormManager::self()->library()->iconName(
                   dynamic_cast<QWidget*>(d->iface)->metaObject()->className());
    }
    d->contextMenuHasTitle = d->iface->columnInfo() ?
        KexiContextMenuUtils::updateTitle(
            menu,
            d->iface->columnInfo()->captionOrAliasOrName(),
            KexiDB::simplifiedTypeName(*d->iface->columnInfo()->field), icon)
        : false;

    updatePopupMenuActions(menu);
}

void KexiDBWidgetContextMenuExtender::updatePopupMenuActions(QMenu *menu)
{
    const bool readOnly = d->iface->isReadOnly();

    foreach(QAction* action, menu->actions()) {
        const QString text(action->text());
        if (text.startsWith(QObject::tr("Cu&t")) /*do not use i18n()!*/
            || text.startsWith(QObject::tr("C&lear"))
            || text.startsWith(QObject::tr("&Paste"))
            || text.startsWith(QObject::tr("Delete")))
        {
            action->setEnabled(!readOnly);
        }
        else if (text.startsWith(QObject::tr("&Redo")))
        {
//! @todo maybe redo will be enabled one day?
            action->setVisible(false);
        }
    }
}

//------------------

KexiSubwidgetInterface::KexiSubwidgetInterface()
{
}

KexiSubwidgetInterface::~KexiSubwidgetInterface()
{
}
