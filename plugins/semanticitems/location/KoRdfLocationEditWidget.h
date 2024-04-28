/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfLocationEditWidget_h__
#define __rdf_KoRdfLocationEditWidget_h__

#include <QWidget>

class KoRdfLocationEditWidgetPrivate;

namespace Marble
{
class MarbleWidget;
class LatLonEdit;
}
namespace Ui
{
class KoRdfLocationEditWidget;
}

/**
 * This class allows the map to adjust the LatLonEdit widgets as the
 * user drags the map
 */
class KoRdfLocationEditWidget : public QWidget
{
    Q_OBJECT
    KoRdfLocationEditWidgetPrivate *const d;

public:
    KoRdfLocationEditWidget(QWidget *parent, Ui::KoRdfLocationEditWidget *ew);
    ~KoRdfLocationEditWidget();

#ifdef CAN_USE_MARBLE
    void setupMap(Marble::MarbleWidget *map, Marble::LatLonEdit *xlat, Marble::LatLonEdit *xlong);

private Q_SLOTS:
    void mouseMoveGeoPosition();
#endif
};

#endif
