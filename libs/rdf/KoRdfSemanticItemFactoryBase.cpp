/* This file is part of the Calligra project, made with-in the KDE community

   Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
   Copyright (C) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoRdfSemanticItemFactoryBase.h"

// #include <KoProperties.h>

#include <QStringList>

class KoRdfSemanticItemFactoryBase::Private
{
public:
    Private(const QString &identifier)
            : id(identifier) {
    }

    ~Private() {
//         foreach(const KoInlineObjectTemplate &t, templates)
//             delete t.properties;
//         templates.clear();
    }
    const QString id;
#if 0
    QString iconName;
    QString odfNameSpace;
    QStringList odfElementNames;
    QList<KoInlineObjectTemplate> templates;
#endif
};

KoRdfSemanticItemFactoryBase::KoRdfSemanticItemFactoryBase(const QString &id)
        : d(new Private(id))
{
//     d->type = type;
}

KoRdfSemanticItemFactoryBase::~KoRdfSemanticItemFactoryBase()
{
    delete d;
}

QString KoRdfSemanticItemFactoryBase::id() const
{
    return d->id;
}

#if 0
QList<KoInlineObjectTemplate> KoRdfSemanticItemFactoryBase::templates() const
{
    return d->templates;
}

void KoRdfSemanticItemFactoryBase::addTemplate(const KoInlineObjectTemplate &params)
{
    d->templates.append(params);
}

QStringList KoRdfSemanticItemFactoryBase::odfElementNames() const
{
    return d->odfElementNames;
}

QString KoRdfSemanticItemFactoryBase::odfNameSpace() const
{
    return d->odfNameSpace;
}

void KoRdfSemanticItemFactoryBase::setOdfElementNames(const QString & nameSpace, const QStringList &names)
{
    d->odfNameSpace = nameSpace;
    d->odfElementNames = names;
}

KoRdfSemanticItemFactoryBase::ObjectType KoRdfSemanticItemFactoryBase::type() const
{
    return d->type;
}
#endif