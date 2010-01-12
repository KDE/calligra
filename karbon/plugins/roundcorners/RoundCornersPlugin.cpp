/* This file is part of the KDE project
 * Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2002-2003 Stephan Binner <binner@kde.org>
 * Copyright (C) 2003,2006 Laurent Montel <montel@kde.org>
 * Copyright (C) 2003,2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
 * Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2006-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "RoundCornersPlugin.h"
#include "RoundCornersCommand.h"

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPathShape.h>
#include <KoParameterShape.h>
#include <KoParameterToPathCommand.h>
#include <KoUnitDoubleSpinBox.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kicon.h>
#include <kactioncollection.h>

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>

typedef KGenericFactory<RoundCornersPlugin, QWidget> RoundCornersPluginFactory;
K_EXPORT_COMPONENT_FACTORY(karbon_roundcornersplugin, RoundCornersPluginFactory("karbonroundcornersplugin"))

RoundCornersPlugin::RoundCornersPlugin(QWidget * parent, const QStringList &)
        : Plugin(parent)
{
    KAction *actionRoundCorners  = new KAction(KIcon("14_roundcorners"), i18n("&Round Corners..."), this);
    actionCollection()->addAction("path_round_corners", actionRoundCorners);
    connect(actionRoundCorners, SIGNAL(triggered()), this, SLOT(slotRoundCorners()));

    m_roundCornersDlg = new RoundCornersDlg();
    m_roundCornersDlg->setRadius(10.0);
}

RoundCornersPlugin::~RoundCornersPlugin()
{
}

void RoundCornersPlugin::slotRoundCorners()
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

    m_roundCornersDlg->setUnit(canvasController->canvas()->unit());
    if (QDialog::Rejected == m_roundCornersDlg->exec())
        return;

    QUndoCommand * cmd = new QUndoCommand(i18n("Round Corners"));

    // convert to path before if we have a parametric shape
    KoParameterShape * ps = dynamic_cast<KoParameterShape*>(shape);
    if (ps && ps->isParametricShape())
        new KoParameterToPathCommand(ps, cmd);

    new RoundCornersCommand(path, m_roundCornersDlg->radius(), cmd);
    canvasController->canvas()->addCommand(cmd);
}


RoundCornersDlg::RoundCornersDlg(QWidget* parent, const char* name)
        : KDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setCaption(i18n("Round Corners"));
    setButtons(Ok | Cancel);

    // add input:
    QGroupBox* group = new QGroupBox(i18n("Properties"), this);

    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(new QLabel(i18n("Radius:")));
    m_radius = new KoUnitDoubleSpinBox(group);
    m_radius->setMinimum(1.0);
    layout->addWidget(m_radius);

    group->setLayout(layout);
    group->setMinimumWidth(300);

    // signals and slots:
    connect(this, SIGNAL(okClicked()), this, SLOT(accept()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(reject()));

    setMainWidget(group);
}

qreal RoundCornersDlg::radius() const
{
    return m_radius->value();
}

void RoundCornersDlg::setRadius(qreal value)
{
    m_radius->setValue(value);
}

void RoundCornersDlg::setUnit(const KoUnit &unit)
{
    m_radius->setUnit(unit);
}

#include "RoundCornersPlugin.moc"

