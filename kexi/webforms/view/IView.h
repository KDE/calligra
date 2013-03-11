/*
 * This file is part of the KDE project
 *
 * (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIWEBFORMS_VIEW_IVIEW_H
#define KEXIWEBFORMS_VIEW_IVIEW_H

#include <pion/net/HTTPResponseWriter.hpp>

class QString;
template <class Key, class T> class QHash;

/*!
 * The KexiWebForms namespace contains all the code related to the Kexi
 * Web Forms Daemon.
 * @author Lorenzo Villani <lvillani@binaryhelix.net>
 */
namespace KexiWebForms {
namespace View {

/*!
 * @brief Interface for all view classes
 *
 * Interface that must be implemented by all views in order to be
 * used from the Front Controller
 */
class IView {
public:
    /*!
     * Pure virtual function to be implemented by derived view classes
     * @param QHash<QString,QString>& a const reference to the data stash
     * @param pion::net::HTTPWriterPtr a pointer to the writer class
     */
    virtual void view(const QHash<QString, QString>&, pion::net::HTTPResponseWriterPtr) = 0;
    virtual ~IView() {}
};


} // end namespace View
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_VIEW_IVIEW_H */
