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

#include <boost/regex.hpp>
#include "PatternServer.h"

namespace pion {
namespace net {

    bool PatternServer::findRequestHandler(const std::string& resource,
                                    RequestHandler& request_handler) const
    {
        // first make sure that HTTP resources are registered
        boost::mutex::scoped_lock resource_lock(m_resource_mutex);
        if (m_resources.empty())
            return false;

        PION_LOG_INFO(m_logger, "Requested resource: \"" << resource << "\"");
        
        // iterate through each resource entry that may match the resource
        ResourceMap::const_iterator i = m_resources.upper_bound(resource);
        while (i != m_resources.begin()) {
            --i;
            try {
                if (resource == "" && ((i->first == "/") || i->first == "")) {
                    request_handler = i->second;
                    return true;
                } else {
                    boost::regex expression(i->first);
                    if (boost::regex_match(resource, expression, boost::match_default | boost::match_partial)) {
                        request_handler = i->second;
                        return true;
                    }
                }
            } catch (boost::regex_error& re) {
                // do nothing
            }
        }

        return false;
    }

}
}
