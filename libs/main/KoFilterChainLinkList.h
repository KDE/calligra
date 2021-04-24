/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOFILTERCHAINLINKLIST_H
#define KOFILTERCHAINLINKLIST_H

#include <QList>

namespace CalligraFilter {

    class ChainLink;


    class ChainLinkList
    {
    public:
        ChainLinkList();
        ~ChainLinkList();

        void deleteAll();
        int count() const;

        /**
         * Return a pointer to the current position in the chain.
         * @return pointer to the current ChainLink or 0 if the ChainLinkList is empty.
         **/
        ChainLink* current() const;

        /**
         * Move the position to the first position in the chain.
         * @return pointer to the first ChainLink or 0 if the ChainLinkList is empty.
         **/
        ChainLink* first();

        ChainLink* next();

        void prepend(ChainLink* link);

        void append(ChainLink* link);

    private:

        QList<ChainLink*> m_chainLinks;
        int m_current;

    };

}

#endif // KOFILTERCHAINLINKLIST_H
