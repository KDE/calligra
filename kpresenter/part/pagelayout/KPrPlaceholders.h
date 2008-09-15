/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRPLACEHOLDERS_H
#define KPRPLACEHOLDERS_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <QString>

class QSizeF;
class KoShape;
class KoPADocument;
class KPrPageLayout;

struct Placeholder
{
    Placeholder( const QString & presentationClass, KoShape * shape, bool isPlaceholder )
    : presentationClass( presentationClass )
    , shape( shape )
    , isPlaceholder( isPlaceholder )
    {}

    QString presentationClass;
    KoShape * shape;
    bool    isPlaceholder;
};

typedef boost::multi_index_container<
    Placeholder,
    boost::multi_index::indexed_by<
        boost::multi_index::sequenced<>,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::member<Placeholder, QString, &Placeholder::presentationClass>
        >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::member<Placeholder, KoShape *, &Placeholder::shape>
        >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::member<Placeholder, bool, &Placeholder::isPlaceholder>
        >
    >
> Placeholders;

typedef boost::multi_index::nth_index<Placeholders,2>::type PlaceholdersByShape;

class KPrPlaceholders
{
public:
    KPrPlaceholders();
    ~KPrPlaceholders();

    /**
     * @param layout the layout that should be used from now. 
     *        If 0 no layout will be used.
     * @param document The document where the shapes are located in
     * @param shapes list of first level shapes
     */
    void setLayout( KPrPageLayout * layout, KoPADocument * document, const QList<KoShape *> & shapes, const QSizeF & pageSize );

    /**
     * This function should only be used during loading
     * @param layout the layout that should be used from now. 
     *        If 0 no layout will be used.
     * TODO tz: maybe make privat and the classes that need to call it a friend
     */
    void init( KPrPageLayout * layout, const QList<KoShape *> & shapes );

    KPrPageLayout * layout() const;

    void shapeAdded( KoShape * shape );

    void shapeRemoved( KoShape * shape );

private:
    void add( const QList<KoShape *> & shapes );

    KPrPageLayout * m_layout;
    // that is set to true when the m_placeholders is initialized
    bool m_initialized;
    Placeholders m_placeholders;
};

#endif /* KPRPLACEHOLDERS_H */
