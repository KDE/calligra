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

#ifndef UI_KDGANTTTIMESCALEZOOMDIALOG_H
#define UI_KDGANTTTIMESCALEZOOMDIALOG_H

#include "kdgantt_export.h"

#include <QDialog>

class QHBoxLayout;
class QToolButton;

namespace KDGantt {

class Ui_TimeScaleZoomPane
{
public:
    QHBoxLayout *horizontalLayout;
    QToolButton *zoomOut;
    QToolButton *zoomIn;

    void setupUi(QDialog *KDGantt__TimeScaleZoomPane);

    void retranslateUi(QDialog *KDGantt__TimeScaleZoomPane);

};

} // namespace KDGantt

namespace KDGantt {
namespace Ui {
    class TimeScaleZoomPane: public Ui_TimeScaleZoomPane {};
} // namespace Ui

class KDGANTT_EXPORT TimeScaleZoomDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TimeScaleZoomDialog( QWidget *parent = 0 );
    
    QToolButton *zoomIn;
    QToolButton *zoomOut;
    
private:
    Ui::TimeScaleZoomPane pane;
};

} // namespace KDGantt

#endif // KDGANTTTIMESCALEZOOMDIALOG_H

