/* This file is part of the KDE libraries
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koFilter.h>


KoFilter::KoFilter(KoFilter *parent, const char *name) : QObject(parent, name) {
}

bool KoFilter::filter(const QString &, const QString &,
                      const QString &, const QString &,
                      const QString &) {
    return false;
}

bool KoFilter::filter(const QString &,
                       const QString &, const QString &,
                       const QString &, const QString &,
                       const QString &) {
    return false;
}

bool KoFilter::supportsEmbedding() {
    return false;
}

bool KoFilter::filterImport(const QString &, const QString &,
                        QDomDocument &, const QString &,
                        const QString &) {
    return false;
}

bool KoFilter::filterImport(const QString &, KoDocument *,
                        const QString &, const QString &,
                        const QString &) {
    return false;
}

bool KoFilter::filterExport(const QString &, KoDocument * ,
                        const QString &, const QString &,
                        const QString &) {
    return false;
}

#include <koFilter.moc>
