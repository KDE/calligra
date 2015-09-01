/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Radosław Wicik <radoslaw@wicik.pl>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "MapBrowserFactory.h"
#include "MapBrowserWidget.h"
#include <formeditor/WidgetInfo.h>
#include <formeditor/formIO.h>
#include <KexiIcon.h>
#include <kexi.h>

#include <KProperty>
#include <KPropertySet>

#include <KLocalizedString>

#include <QDebug>
#include <QVariant>
#include <QVariantList>
#include <qglobal.h>

#include <marble/MapThemeManager.h>

KEXI_PLUGIN_FACTORY(MapBrowserFactory, "kexiforms_mapwidgetplugin.json")

MapBrowserFactory::MapBrowserFactory(QObject* parent, const QVariantList& args)
  : KexiDBFactoryBase(parent)
{
    Q_UNUSED(args);
    KFormDesigner::WidgetInfo *mapBrowser = new KFormDesigner::WidgetInfo(this);
    mapBrowser->setIconName(koIconName("map_browser"));
    mapBrowser->setClassName("MapBrowserWidget");
    mapBrowser->setName(xi18n("Map Browser"));
    mapBrowser->setNamePrefix(
        xi18nc("A prefix for identifiers of map browser widgets. Based on that, identifiers such as "
            "mapBrowser1, mapBrowser2 are generated. "
            "This string can be used to refer the widget object as variables in programming "
            "languages or macros so it must _not_ contain white spaces and non latin1 characters, "
            "should start with lower case letter and if there are subsequent words, these should "
            "start with upper case letter. Example: smallCamelCase. "
            "Moreover, try to make this prefix as short as possible.",
            "mapBrowser"));
    mapBrowser->setDescription(xi18n("Displays an interactive map."));
    addClass(mapBrowser);
}

MapBrowserFactory::~MapBrowserFactory()
{
}

QWidget* MapBrowserFactory::createWidget(const QByteArray& classname,
                            QWidget* parent,
                            const char* name,
                            KFormDesigner::Container* container,
                            KFormDesigner::WidgetFactory::CreateWidgetOptions options)
{
    Q_UNUSED(options);
    QWidget *w = 0;
    QString text(container->form()->library()->textForWidgetName(name, classname));

    if (classname == "MapBrowserWidget")
        w = new MapBrowserWidget(parent);

    if (w){
        w->setObjectName(name);
        qDebug() << w << w->objectName() << "created";
        return w;
    }
    qWarning() << "w == 0";
    return 0;
}

bool MapBrowserFactory::createMenuActions(const QByteArray &classname, QWidget *w,
                                    QMenu *menu, KFormDesigner::Container *container)
{
    Q_UNUSED(classname);
    Q_UNUSED(w);
    Q_UNUSED(menu);
    Q_UNUSED(container);
    return false;
}

bool MapBrowserFactory::startInlineEditing(InlineEditorCreationArguments& args)
{
    Q_UNUSED(args);
    return false;
}

bool MapBrowserFactory::previewWidget(const QByteArray &classname,
                                QWidget *widget, KFormDesigner::Container *)
{
    Q_UNUSED(classname);
    Q_UNUSED(widget);
    return true;
}

void MapBrowserFactory::setPropertyOptions(KPropertySet& set, const KFormDesigner::WidgetInfo& info, QWidget* w)
{
    KFormDesigner::WidgetFactory::setPropertyOptions(set, info, w);

    KProperty *property = &set["latitude"];
    if (!property->isNull()) {
        property->setCaption(xi18n("Latitude"));
        property->setDescription(xi18n("Latitude"));
        property->setType(KProperty::Double);
        property->setOption("precision", 7);
        property->setOption("min", -90);
        property->setOption("max", 90);
        property->setOption("unit", QString::fromUtf8("°"));
    }

    property = &set["longitude"];
    if (!property->isNull()) {
        property->setCaption(xi18n("Longitude"));
        property->setDescription(xi18n("Longitude"));
        property->setOption("precision", 7);
        property->setType(KProperty::Double);
        property->setOption("min", -180);
        property->setOption("max", 180);
        property->setOption("unit", QString::fromUtf8("°"));
    }

    property = &set["mapThemeId"];
    if (!property->isNull()) {
        Marble::MapThemeManager themeManager;
        QStringList themes(themeManager.mapThemeIds());

        property->setCaption(xi18n("Theme"));
        property->setDescription(xi18n("Theme"));
        property->setType(KProperty::List);
        property->setListData(themes, themes);
    }

    property = &set["zoom"];
    if (!property->isNull()) {
        property->setCaption(xi18n("Zoom"));
        property->setDescription(xi18n("Zoom"));
        property->setOption("min", 0);
        property->setOption("max", 4000);
        property->setOption("step", 100);
        property->setOption("slider", true);
    }
}

#include "MapBrowserFactory.moc"
