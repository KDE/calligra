/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2002-2003 Stephan Binner <binner@kde.org>
 * SPDX-FileCopyrightText: 2003, 2006 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2003, 2006 Dirk Mueller <mueller@kde.org>
 * SPDX-FileCopyrightText: 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2004 Nicolas Goutte <nicolasg@snafu.de>
 * SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "RoundCornersPlugin.h"
#include "RoundCornersCommand.h"

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoIcon.h>
#include <KoParameterShape.h>
#include <KoParameterToPathCommand.h>
#include <KoPathShape.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>
#include <kundo2command.h>

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
#include <QStandardPaths>
#include <QVBoxLayout>

K_PLUGIN_FACTORY_WITH_JSON(RoundCornersPluginFactory, "karbon_roundcorners.json", registerPlugin<RoundCornersPlugin>();)

RoundCornersPlugin::RoundCornersPlugin(QObject *parent, const QVariantList &)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "karbon/plugins/RoundCornersPlugin.rc"));

    QAction *actionRoundCorners = new QAction(koIcon("effect_roundcorners"), i18n("&Round Corners..."), this);
    actionCollection()->addAction("path_round_corners", actionRoundCorners);
    connect(actionRoundCorners, &QAction::triggered, this, &RoundCornersPlugin::slotRoundCorners);

    m_roundCornersDlg = new RoundCornersDlg(qobject_cast<QWidget *>(parent));
    m_roundCornersDlg->setRadius(10.0);
}

RoundCornersPlugin::~RoundCornersPlugin() = default;

void RoundCornersPlugin::slotRoundCorners()
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

    m_roundCornersDlg->setUnit(canvasController->canvas()->unit());
    if (QDialog::Rejected == m_roundCornersDlg->exec())
        return;

    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Round Corners"));

    // convert to path before if we have a parametric shape
    KoParameterShape *ps = dynamic_cast<KoParameterShape *>(shape);
    if (ps && ps->isParametricShape())
        new KoParameterToPathCommand(ps, cmd);

    new RoundCornersCommand(path, m_roundCornersDlg->radius(), cmd);
    canvasController->canvas()->addCommand(cmd);
}

RoundCornersDlg::RoundCornersDlg(QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Round Corners"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    // add input:
    QGroupBox *group = new QGroupBox(i18n("Properties"), this);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(new QLabel(i18n("Radius:")));
    m_radius = new KoUnitDoubleSpinBox(group);
    m_radius->setMinimum(1.0);
    layout->addWidget(m_radius);

    group->setLayout(layout);
    group->setMinimumWidth(300);

    // signals and Q_SLOTS:
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QDialog::reject);

    mainLayout->addWidget(group);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
    mainLayout->addWidget(buttonBox);
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

#include <RoundCornersPlugin.moc>
