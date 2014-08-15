/* This file is part of the KDE project
   Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>

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

#ifndef CAUSECTIONRDF_H
#define CAUSECTIONRDF_H

#include <KoRdfSemanticItem.h>

#include "ui_CAuSectionRdfEditWidget.h"

class CAuSectionRdf : public KoRdfBasicSemanticItem
{
    Q_OBJECT
public:

    explicit CAuSectionRdf(QObject *parent, const KoDocumentRdf *m_rdf = 0);
    CAuSectionRdf(QObject *parent, const KoDocumentRdf *m_rdf, Soprano::QueryResultIterator &it);

    // inherited and reimplemented...
    virtual QWidget *createEditor(QWidget *parent);
    virtual void updateFromEditorData();
    virtual Soprano::Node linkingSubject() const;
    virtual QString className() const;
    virtual Soprano::Node context() const;

    // accessor methods...
    virtual QString name() const;

private:
    static const int STATUS_COUNT = 8;
    static const QString STATUS[STATUS_COUNT];

    static QString authorSectionPrefix();

    // Author section rdf template
    // s == m_uri
    // s -> <uri:section>; p -> <http://www.calligra.org/author/descr>; o -> "Some description"
    // s -> <uri:section>; p -> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type>; o -> <http://www.calligra.org/author/Section>
    QString m_uri;   // This is the subject in Rdf
    QString m_synop;
    QString m_magicId;
    QString m_status;
    QString m_badge;

    bool isTypeSet;

    Ui::CAuSectionRdfEditWidget m_editWidgetUI;
};

typedef QExplicitlySharedDataPointer<CAuSectionRdf> hCAuSectionRdf;

#endif //CAUSECTIONRDF_H
