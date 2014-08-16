/* This file is part of the KDE project
 *   Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CAUACTORRDF_H
#define CAUACTORRDF_H

#include <author/metadata/CAuSemanticItemBase.h>

#include "ui_CAuActorRdfEditWidget.h"

class CAuActorRdf : public CAuSemanticItemBase
{
    Q_OBJECT
public:
    static QString QUERY;

    explicit CAuActorRdf(QObject *parent, const KoDocumentRdf *rdf);
    CAuActorRdf(QObject *parent, const KoDocumentRdf *rdf, Soprano::QueryResultIterator &it);

    // inherited and reimplemented...
    virtual QWidget *createEditor(QWidget *parent);
    virtual void updateFromEditorData();
    virtual QString className() const;

    // accessor methods...
    virtual QString name() const;

private:
    static const int ROLE_COUNT = 5;
    static const QString ROLE[ROLE_COUNT];

    static const int TYPE_COUNT = 3;
    static const QString TYPE[TYPE_COUNT];

    virtual QList<QString> stringProps();
    virtual QList<QString> intProps();

    Ui::CAuActorRdfEditWidget m_editWidgetUI;
};

#endif //CAUACTORRDF_H
