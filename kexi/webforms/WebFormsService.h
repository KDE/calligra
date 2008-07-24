/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_WEBFORMSSERVICE_H
#define KEXIWEBFORMS_WEBFORMSSERVICE_H

#include <pion/net/WebService.hpp>

class Controller;
class QString;
class QVariant;
namespace google {
    class TemplateDictionary;
}
namespace KexiDB {
    class Cursor;
}

namespace KexiWebForms {

    /**
     * @brief Kexi Web Forms specific Service
     *
     * Services are used to handle requests on specific URIs, for example you
     * can bind a WebService to listen for requests on '/query'. Refer to
     * pion-network-library documentation for more details.
     *
     * In particular this WebService implementation is used to intialize a
     * google::TemplateDictionary context for the specified webservice and
     * provides binding to google::TemplateDictionary::SetValue accepting QStrings
     * and QVariants, thus simplifying the code in resources handlers a lot.
     */
    class WebFormsService : public pion::net::WebService {
        public:
            /**
             * Constructor
             */
            WebFormsService();
            
            /**
             * Constructor
             * @param char* name of a template file
             */
            WebFormsService(Controller*, const char*);
            virtual ~WebFormsService();

            /**
             * Handle request, refer to pion::net::WebService documentation
             */
            virtual void operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) = 0;

            /**
             * A binding to local google::TemplateDictionary::SetValue accepting a
             * QVariant
             * @todo Move this stuff in a derived class of google::TemplateDictionary
             */
            void setValue(const char*, const QVariant&);

            /**
             * A binding to local google::TemplateDictionary::SetValue accepting a
             * QString
             * @todo Move this stuff in a derived class of google::TemplateDictionary
             */
            void setValue(const char*, const QString&);

            /**
             * This is not really needed but it's provided for convenience
             * @todo Move this stuff in a derived class of google::TemplateDictionary
             */
            void setValue(const char*, const char*);
        protected:
        
            Controller* m_controller;
            /**
             * A local instance of google::TemplateDictionary, initialized by ctor
             */
            google::TemplateDictionary* m_dict;

            /**
             * Since it is common for KWebForms handlers to have a KexiDB::Cursor somewhere
             * in the code, provide it as local with facilities to close and delete the pointer
             * when class is destroyed
             */
            KexiDB::Cursor* m_cursor;
    };
    
}

#endif
