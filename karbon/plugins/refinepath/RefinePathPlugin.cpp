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


#include "RefinePathPlugin.h"
#include "KarbonPathRefineCommand.h"

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoPathShape.h>
#include <KoParameterShape.h>
#include <KoIcon.h>

#include <kpluginfactory.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>

#include <QSpinBox>
#include <QStandardPaths>
#include <QAction>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

K_PLUGIN_FACTORY_WITH_JSON(RefinePathPluginFactory, "karbon_refinepath.json",
                           registerPlugin<RefinePathPlugin>();)

RefinePathPlugin::RefinePathPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "karbon/plugins/RefinePathPlugin.rc"), true);
    QAction *actionRefinePath  = new QAction(koIcon("effect_refine"), i18n("&Refine Path..."), this);
    actionCollection()->addAction("path_refine", actionRefinePath);
    connect(actionRefinePath, SIGNAL(triggered()), this, SLOT(slotRefinePath()));

    m_RefinePathDlg = new RefinePathDlg(qobject_cast<QWidget*>(parent));
}

void RefinePathPlugin::slotRefinePath()
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

    if (QDialog::Rejected == m_RefinePathDlg->exec())
        return;

    canvasController->canvas()->addCommand(new KarbonPathRefineCommand(path, m_RefinePathDlg->knots()));
}

RefinePathDlg::RefinePathDlg(QWidget* parent, const char* name)
        : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Refine Path"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    QGroupBox * group = new QGroupBox(this);
    group->setTitle(i18n("Properties"));
    mainLayout->addWidget(group);

    QHBoxLayout * hbox = new QHBoxLayout(group);
    hbox->addWidget(new QLabel(i18n("Subdivisions:"), group));

    m_knots = new QSpinBox(group);
    mainLayout->addWidget(m_knots);
    m_knots->setMinimum(1);
    hbox->addWidget(m_knots);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);
}

uint RefinePathDlg::knots() const
{
    return m_knots->value();
}

void RefinePathDlg::setKnots(uint value)
{
    m_knots->setValue(value);
}

#include <RefinePathPlugin.moc>

