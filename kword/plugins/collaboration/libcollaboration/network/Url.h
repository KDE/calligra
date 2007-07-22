/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef URL_H
#define URL_H

#include <KUrl>
#include <QHostAddress>

namespace kcollaborate
{

/**
 * Collaboration url sample:
 *
 * collaborate://localhost:2131/text/3234235235/My%20document
 *
 * where
 * 'text' is subprotocol name,
 * '3234235235' is invitation number (simple auth),
 * and 'My%20document' is document name.
 *
 * This class adds subprotocol(), invitationNumber(), and documentName() accesors.
 *
 */
class Url : public KUrl
{
    public:
        Url( const QString &url );
        virtual ~Url();

        bool broken();

        const QString& subprotocol();
        const QString& invitationNumber();
        const QString& documentName();

        const QHostAddress& hostAddress();

        ///overrided function to reset parsed and hostConverted variables
        void setPath( const QString &path );
    private:
        bool broken_;

        bool parsed;
        QString subprotocol_;
        QString invitationNumber_;
        QString documentName_;

        bool hostConverted;
        QHostAddress hostAddress_;

        ///lazy parse function for subprotocol_, documentName_, and invitationNumber_
        void parse();
        ///lazy convert function for hostAddress_
        void hostConvert();
};

};

#endif
