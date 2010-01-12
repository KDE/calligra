/* This file is part of the KDE project
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002-2003 Stephan Binner <binner@kde.org>
   Copyright (C) 2003,2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Stephan Kulow <coolo@kde.org>

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

#include "FlattenPathPlugin.h"
#include "KarbonPathFlattenCommand.h"

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPathShape.h>
#include <KoParameterShape.h>

#include <kgenericfactory.h>
#include <knuminput.h>
#include <kactioncollection.h>
#include <kicon.h>
#include <klocale.h>

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>


typedef KGenericFactory<FlattenPathPlugin, QWidget> FlattenPathPluginFactory;
K_EXPORT_COMPONENT_FACTORY(karbon_flattenpathplugin, FlattenPathPluginFactory("karbonflattenpathplugin"))

FlattenPathPlugin::FlattenPathPlugin(QWidget *parent, const QStringList &)
        : Plugin(parent/*, name*/)
{
    KAction *actionFlattenPath  = new KAction(KIcon("14_flatten"), i18n("&Flatten Path..."), this);
    actionCollection()->addAction("path_flatten", actionFlattenPath);
    connect(actionFlattenPath, SIGNAL(triggered()), this, SLOT(slotFlattenPath()));

    m_flattenPathDlg = new FlattenDlg(parent);
    m_flattenPathDlg->setFlatness(10.0);
}

void FlattenPathPlugin::slotFlattenPath()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    KoShape * shape = selection->firstSelectedShape();
    if (! shape)
        return;

    // check if we have a path based shape
    KoPathShape * path = dynamic_cast<KoPathShape*>(shape);
    if (! path)
        return;

    // check if it is no parametric shape
    KoParameterShape * ps = dynamic_cast<KoParameterShape*>(shape);
    if (ps && ps->isParametricShape())
        return;

    if (QDialog::Rejected == m_flattenPathDlg->exec())
        return;

    canvasController->canvas()->addCommand(
        new KarbonPathFlattenCommand(path, m_flattenPathDlg->flatness()));
}

FlattenDlg::FlattenDlg(QWidget* parent, const char* name)
        : KDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setCaption(i18n("Flatten Path"));
    setButtons(Ok | Cancel);

    // add input fields on the left:
    QGroupBox* group = new QGroupBox(i18n("Properties"), this);

    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(new QLabel(i18n("Flatness:")));
    m_flatness = new KDoubleNumInput(group);
    layout->addWidget(m_flatness);

    group->setLayout(layout);
    group->setMinimumWidth(300);

    // signals and slots:
    connect(this, SIGNAL(okClicked()), this, SLOT(accept()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(reject()));

    setMainWidget(group);
}

qreal FlattenDlg::flatness() const
{
    return m_flatness->value();
}

void FlattenDlg::setFlatness(qreal value)
{
    m_flatness->setValue(value);
}

#include "FlattenPathPlugin.moc"

