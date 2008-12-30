/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
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

#include "kdgantttimescalezoomdialog.h"

#include <QLocale>
#include <QHBoxLayout>
#include <QToolButton>

namespace KDGantt {

TimeScaleZoomDialog::TimeScaleZoomDialog( QWidget *parent )
    : QDialog( parent )
{
    pane.setupUi( this );
    zoomIn = pane.zoomIn;
    zoomOut = pane.zoomOut;
}

void Ui_TimeScaleZoomPane::setupUi(QDialog *KDGantt__TimeScaleZoomPane)
{
    if (KDGantt__TimeScaleZoomPane->objectName().isEmpty())
        KDGantt__TimeScaleZoomPane->setObjectName(QString::fromUtf8("KDGantt__TimeScaleZoomPane"));
    KDGantt__TimeScaleZoomPane->resize(152, 55);
    KDGantt__TimeScaleZoomPane->setLocale(QLocale(QLocale::C, QLocale::AnyCountry));
    KDGantt__TimeScaleZoomPane->setModal(true);
    horizontalLayout = new QHBoxLayout(KDGantt__TimeScaleZoomPane);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    zoomIn = new QToolButton(KDGantt__TimeScaleZoomPane);
    zoomIn->setObjectName(QString::fromUtf8("zoomIn"));
    zoomIn->setAutoRepeat(true);
    zoomIn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    zoomIn->setAutoRaise(false);
    zoomIn->setArrowType(Qt::LeftArrow);

    horizontalLayout->addWidget(zoomIn);

    zoomOut = new QToolButton(KDGantt__TimeScaleZoomPane);
    zoomOut->setObjectName(QString::fromUtf8("zoomOut"));
    zoomOut->setAutoRepeat(true);
    zoomOut->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    zoomOut->setAutoRaise(false);
    zoomOut->setArrowType(Qt::RightArrow);

    horizontalLayout->addWidget(zoomOut);


    retranslateUi(KDGantt__TimeScaleZoomPane);

    QMetaObject::connectSlotsByName(KDGantt__TimeScaleZoomPane);
} // setupUi

void Ui_TimeScaleZoomPane::retranslateUi(QDialog *KDGantt__TimeScaleZoomPane)
{
    KDGantt__TimeScaleZoomPane->setWindowTitle(QDialog::tr("Zoom"));
    zoomOut->setText(QDialog::tr("Zoom out"));
    zoomIn->setText(QDialog::tr("Zoom in"));
    Q_UNUSED(KDGantt__TimeScaleZoomPane);
} // retranslateUi

} // namespace KDGantt

#include "moc_kdgantttimescalezoomdialog.cpp"

