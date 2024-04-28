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
   SPDX-FileCopyrightText: 2006 Andreas Hartmetz <ahartmetz@gmail.com>
   SPDX-FileCopyrightText: 2006 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "WhirlPinchPlugin.h"
#include "KarbonWhirlPinchCommand.h"

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoIcon.h>
#include <KoParameterShape.h>
#include <KoPathShape.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>

#include <KConfigGroup>
#include <QAction>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

K_PLUGIN_FACTORY_WITH_JSON(WhirlPinchPluginFactory, "karbon_whirlpinch.json", registerPlugin<WhirlPinchPlugin>();)

WhirlPinchPlugin::WhirlPinchPlugin(QObject *parent, const QVariantList &)
{
    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "karbon/plugins/WhirlPinchPlugin.rc"), true);
    QAction *a = new QAction(koIcon("effect_whirl"), i18n("&Whirl/Pinch..."), this);
    actionCollection()->addAction("path_whirlpinch", a);
    connect(a, &QAction::triggered, this, &WhirlPinchPlugin::slotWhirlPinch);

    m_whirlPinchDlg = new WhirlPinchDlg(qobject_cast<QWidget *>(parent));
    m_whirlPinchDlg->setAngle(180.0);
    m_whirlPinchDlg->setPinch(0.0);
    m_whirlPinchDlg->setRadius(100.0);
}

void WhirlPinchPlugin::slotWhirlPinch()
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

    m_whirlPinchDlg->setUnit(canvasController->canvas()->unit());

    if (QDialog::Rejected == m_whirlPinchDlg->exec())
        return;

    canvasController->canvas()->addCommand(new KarbonWhirlPinchCommand(path, m_whirlPinchDlg->angle(), m_whirlPinchDlg->pinch(), m_whirlPinchDlg->radius()));
}

WhirlPinchDlg::WhirlPinchDlg(QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Whirl Pinch"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *info = new QGroupBox(i18n("Info"), mainWidget);
    QVBoxLayout *infoLayout = new QVBoxLayout(info);
    QString infoText = i18n("The result of the Whirlpinch effect can be improved by refining the path shape beforehand.");
    QLabel *infoLabel = new QLabel(infoText, info);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    // add input fields:
    QGroupBox *group = new QGroupBox(i18n("Properties"), mainWidget);

    QGridLayout *layout = new QGridLayout(group);

    layout->addWidget(new QLabel(i18n("Angle:")), 0, 0);
    m_angle = new QDoubleSpinBox(group);
    layout->addWidget(m_angle, 0, 1);

    layout->addWidget(new QLabel(i18n("Pinch:")), 1, 0);
    m_pinch = new QDoubleSpinBox(group);
    m_pinch->setRange(-1, 1);
    m_pinch->setSingleStep(0.01);
    // QT5TODO: m_pinch had slider util when it was a KDoubleNumInput
    layout->addWidget(m_pinch, 1, 1);

    layout->addWidget(new QLabel(i18n("Radius:")), 2, 0);
    m_radius = new KoUnitDoubleSpinBox(group);
    m_radius->setMinimum(0.0);
    m_radius->setLineStepPt(0.1);
    layout->addWidget(m_radius, 2, 1);

    // signals and Q_SLOTS:
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QDialog::reject);

    mainLayout->addWidget(info);
    mainLayout->addWidget(group);

    mainLayout->addWidget(mainWidget);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
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

#include <WhirlPinchPlugin.moc>
