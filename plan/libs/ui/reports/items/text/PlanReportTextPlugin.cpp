/* This file is part of the KDE project
 *   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
 *   Copyright (C) 2016 by Dag Andersen <danders@get2net.dk>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "PlanReportTextPlugin.h"

#include "PlanReportItemText.h"
#include "PlanReportDesignerItemText.h"
#ifdef KREPORT_SCRIPTING
#include "PlanReportScriptText.h"
#endif

#include "KReportPluginMetaData.h"

#include <QDebug>

KREPORT_PLUGIN_FACTORY(PlanReportTextPlugin, "text.json")

PlanReportTextPlugin::PlanReportTextPlugin(QObject *parent, const QVariantList &args)
    : KReportPluginInterface(parent, args)
{
}

PlanReportTextPlugin::~PlanReportTextPlugin()
{

}

QObject* PlanReportTextPlugin::createRendererInstance(const QDomNode& element)
{
    return new PlanReportItemText(element);
}

QObject* PlanReportTextPlugin::createDesignerInstance(const QDomNode& element, KReportDesigner* designer, QGraphicsScene* scene)
{
    return new PlanReportDesignerItemText(element, designer, scene);
}

QObject* PlanReportTextPlugin::createDesignerInstance(KReportDesigner* designer, QGraphicsScene* scene, const QPointF& pos)
{
    return new PlanReportDesignerItemText(designer, scene, pos);
}

#ifdef KREPORT_SCRIPTING
QObject* PlanReportTextPlugin::createScriptInstance(KReportItemBase* item)
{
    PlanReportItemText *text = dynamic_cast<PlanReportItemText*>(item);
    if (text) {
        return new Scripting::Text(text);
    }
    return 0;
}
#endif

#include "PlanReportTextPlugin.moc"
