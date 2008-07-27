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

#ifndef KEXIWEBFORMS_VIEW_VIEW_H
#define KEXIWEBFORMS_VIEW_VIEW_H

#include <QHash>

#include <google/template.h>
#include <pion/net/HTTPResponseWriter.hpp>

class QString;
class QVariant;

namespace KexiWebForms {
namespace View {

    class View {
    public:
        
        View(const char* name);
        virtual ~View();
        
        virtual void view(const QHash<QString, QString>&, pion::net::HTTPResponseWriterPtr);

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
        google::TemplateDictionary* m_dict;
    };
    
    
} // end namespace View
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_VIEW_VIEW_H */
