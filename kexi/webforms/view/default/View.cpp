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

#include <QHash>
#include <QString>
#include <QVariant>

#include <KDebug>

#include "TemplateProvider.h"
#include "View.h"

namespace KexiWebForms {
    namespace View {

        View::View(const char* name) {
            m_dict = initTemplate(name);
        }

        View::~View() {
            delete m_dict;
        }

        void View::view(const QHash<QString, QString>& data, pion::net::HTTPResponseWriterPtr ptr) {
            kError() << "Not implemented" << endl;
        }
        
        void View::setValue(const char* k, const QVariant& v) {
            setValue(k, v.toString());
        }
        void View::setValue(const char* k, const QString& v) {
            setValue(k, v.toUtf8().constData());
        }
        void View::setValue(const char* k, const char* v) {
            m_dict->SetValue(k, v);
        }
        
    }
}
