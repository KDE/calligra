/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Stephan Binner <binner@kde.org>
   SPDX-FileCopyrightText: 2003, 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <nicolasg@snafu.de>
   SPDX-FileCopyrightText: 2005, 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FlattenPathPlugin.h"
#include "KarbonPathFlattenCommand.h"

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
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

K_PLUGIN_FACTORY_WITH_JSON(FlattenPathPluginFactory, "karbon_flattenpath.json", registerPlugin<FlattenPathPlugin>();)

FlattenPathPlugin::FlattenPathPlugin(QObject *parent, const QVariantList &)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "karbon/plugins/FlattenPathPlugin.rc"), true);

    QAction *actionFlattenPath = new QAction(koIcon("effect_flatten"), i18n("&Flatten Path..."), this);
    actionCollection()->addAction("path_flatten", actionFlattenPath);
    connect(actionFlattenPath, &QAction::triggered, this, &FlattenPathPlugin::slotFlattenPath);

    m_flattenPathDlg = new FlattenDlg(qobject_cast<QWidget *>(parent));
    m_flattenPathDlg->setFlatness(10.0);
}

void FlattenPathPlugin::slotFlattenPath()
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

    if (QDialog::Rejected == m_flattenPathDlg->exec())
        return;

    canvasController->canvas()->addCommand(new KarbonPathFlattenCommand(path, m_flattenPathDlg->flatness()));
}

FlattenDlg::FlattenDlg(QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Flatten Path"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    // add input fields on the left:
    QGroupBox *group = new QGroupBox(i18n("Properties"), this);

    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(new QLabel(i18n("Flatness:")));
    m_flatness = new QDoubleSpinBox(group);
    layout->addWidget(m_flatness);

    group->setLayout(layout);
    group->setMinimumWidth(300);

    // signals and Q_SLOTS:
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QDialog::reject);

    mainLayout->addWidget(group);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

qreal FlattenDlg::flatness() const
{
    return m_flatness->value();
}

void FlattenDlg::setFlatness(qreal value)
{
    m_flatness->setValue(value);
}

#include <FlattenPathPlugin.moc>
