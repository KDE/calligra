/* This file is part of the KDE project
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2003,2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2003 Stephan Binner <binner@kde.org>
   Copyright (C) 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005,2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Andreas Hartmetz <ahartmetz@gmail.com>
   Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>

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

#include "WhirlPinchPlugin.h"
#include "KarbonWhirlPinchCommand.h"

#include <KoUnitDoubleSpinBox.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoPathShape.h>
#include <KoParameterShape.h>
#include <KoSelection.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kicon.h>
#include <knuminput.h>
#include <kactioncollection.h>

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>


typedef KGenericFactory<WhirlPinchPlugin, QWidget> WhirlPinchPluginFactory;
K_EXPORT_COMPONENT_FACTORY(karbon_whirlpinchplugin, WhirlPinchPluginFactory("karbonwhirlpinchplugin"))

WhirlPinchPlugin::WhirlPinchPlugin(QWidget *parent, const QStringList &)
        : Plugin(parent)
{
    QAction *a = new KAction(KIcon("14_whirl"), i18n("&Whirl/Pinch..."), this);
    actionCollection()->addAction("path_whirlpinch", a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotWhirlPinch()));

    m_whirlPinchDlg = new WhirlPinchDlg(parent);
    m_whirlPinchDlg->setAngle(180.0);
    m_whirlPinchDlg->setPinch(0.0);
    m_whirlPinchDlg->setRadius(100.0);
}

void WhirlPinchPlugin::slotWhirlPinch()
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

    m_whirlPinchDlg->setUnit(canvasController->canvas()->unit());

    if (QDialog::Rejected == m_whirlPinchDlg->exec())
        return;

    canvasController->canvas()->addCommand(
        new KarbonWhirlPinchCommand(path, m_whirlPinchDlg->angle(), m_whirlPinchDlg->pinch(), m_whirlPinchDlg->radius()));
}

WhirlPinchDlg::WhirlPinchDlg(QWidget* parent, const char* name)
        : KDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setCaption(i18n("Whirl Pinch"));
    setButtons(Ok | Cancel);

    QWidget * mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* info = new QGroupBox(i18n("Info"), mainWidget);
    QVBoxLayout * infoLayout = new QVBoxLayout(info);
    QString infoText = i18n("The result of the Whirlpinch effect can be improved by refining the path shape beforehand.");
    QLabel * infoLabel = new QLabel(infoText, info);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);

    // add input fields:
    QGroupBox* group = new QGroupBox(i18n("Properties"), mainWidget);

    QGridLayout* layout = new QGridLayout(group);

    layout->addWidget(new QLabel(i18n("Angle:")), 0, 0);
    m_angle = new KDoubleNumInput(group);
    layout->addWidget(m_angle, 0, 1);

    layout->addWidget(new QLabel(i18n("Pinch:")), 1, 0);
    m_pinch = new KDoubleNumInput(group);
    m_pinch->setRange(-1, 1, 0.01, true);
    layout->addWidget(m_pinch, 1, 1);


    layout->addWidget(new QLabel(i18n("Radius:")), 2, 0);
    m_radius = new KoUnitDoubleSpinBox(group);
    m_radius->setMinimum(0.0);
    m_radius->setLineStepPt(0.1);
    layout->addWidget(m_radius, 2, 1);

    // signals and slots:
    connect(this, SIGNAL(okClicked()), this, SLOT(accept()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(reject()));

    mainLayout->addWidget(info);
    mainLayout->addWidget(group);

    setMainWidget(mainWidget);
}

qreal WhirlPinchDlg::angle() const
{
    return m_angle->value();
}

qreal WhirlPinchDlg::pinch() const
{
    return m_pinch->value();
}

qreal WhirlPinchDlg::radius() const
{
    return m_radius->value();
}

void WhirlPinchDlg::setAngle(qreal value)
{
    m_angle->setValue(value);
}

void WhirlPinchDlg::setPinch(qreal value)
{
    m_pinch->setValue(value);
}

void WhirlPinchDlg::setRadius(qreal value)
{
    m_radius->setValue(value);
}

void WhirlPinchDlg::setUnit(const KoUnit &unit)
{
    m_radius->setUnit(unit);
}

#include "WhirlPinchPlugin.moc"

