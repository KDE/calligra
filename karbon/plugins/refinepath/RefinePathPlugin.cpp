/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2003, 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003 Dirk Mueller <mueller@kde.org>
   SPDX-FileCopyrightText: 2003 Stephan Binner <binner@kde.org>
   SPDX-FileCopyrightText: 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <nicolasg@snafu.de>
   SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "RefinePathPlugin.h"
#include "KarbonPathRefineCommand.h"

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoIcon.h>
#include <KoParameterShape.h>
#include <KoPathShape.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>

#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>

#include <KConfigGroup>
#include <QAction>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

K_PLUGIN_FACTORY_WITH_JSON(RefinePathPluginFactory, "karbon_refinepath.json", registerPlugin<RefinePathPlugin>();)

RefinePathPlugin::RefinePathPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "karbon/plugins/RefinePathPlugin.rc"), true);
    QAction *actionRefinePath = new QAction(koIcon("effect_refine"), i18n("&Refine Path..."), this);
    actionCollection()->addAction("path_refine", actionRefinePath);
    connect(actionRefinePath, &QAction::triggered, this, &RefinePathPlugin::slotRefinePath);

    m_RefinePathDlg = new RefinePathDlg(qobject_cast<QWidget *>(parent));
}

void RefinePathPlugin::slotRefinePath()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();
    if (!shape)
        return;

    // check if we have a path based shape
    KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
    if (!path)
        return;

    // check if it is no parametric shape
    KoParameterShape *ps = dynamic_cast<KoParameterShape *>(shape);
    if (ps && ps->isParametricShape())
        return;

    if (QDialog::Rejected == m_RefinePathDlg->exec())
        return;

    canvasController->canvas()->addCommand(new KarbonPathRefineCommand(path, m_RefinePathDlg->knots()));
}

RefinePathDlg::RefinePathDlg(QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Refine Path"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    QGroupBox *group = new QGroupBox(this);
    group->setTitle(i18n("Properties"));
    mainLayout->addWidget(group);

    QHBoxLayout *hbox = new QHBoxLayout(group);
    hbox->addWidget(new QLabel(i18n("Subdivisions:"), group));

    m_knots = new QSpinBox(group);
    mainLayout->addWidget(m_knots);
    m_knots->setMinimum(1);
    hbox->addWidget(m_knots);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
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
