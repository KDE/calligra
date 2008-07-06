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

#ifndef PION_NET_PATTERNSERVER_H
#define PION_NET_PATTERNSERVER_H

#include <pion/net/WebServer.hpp>

namespace pion {
namespace net {

    class PatternServer : public pion::net::WebServer {
    public:
        explicit PatternServer(const unsigned int tcp_port = 0) : WebServer(tcp_port) {}
        explicit PatternServer(const boost::asio::ip::tcp::endpoint& endpoint) : WebServer(endpoint) {}
        explicit PatternServer(PionScheduler& scheduler, const unsigned int tcp_port = 0) : WebServer(scheduler, tcp_port) {}
        PatternServer(PionScheduler& scheduler, const boost::asio::ip::tcp::endpoint& endpoint) : WebServer(scheduler, endpoint) {}
    protected:
        bool findRequestHandler(const std::string& resource,
                                 RequestHandler& request_handler) const;
    };

}
}

#endif
