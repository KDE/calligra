/***************************************************************************
 * kexidbmodule.h
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBMODULE_H
#define KROSS_KEXIDB_KEXIDBMODULE_H

#include <qstring.h>
#include <qvariant.h>

#include "../api/object.h"
#include "../api/variant.h"
//#include "../api/class.h"
#include "../api/module.h"

namespace Kross { namespace KexiDB {

    /**
     * The KexiDBModule wrapper around KexiDB.
     *
     * This class implementates a \a Kross::Api::Module to wrap
     * those parts of KexiDB that should be accessible for
     * the different scripting languages. All work will be done
     * and presented in the common Kross::Api and therefore
     * is independend of any used scripting-backend.
     */
    class KexiDBModule : public Kross::Api::Module<KexiDBModule>
    {
        public:

            /**
             * Constructor.
             */
            KexiDBModule();

            /**
             * Destructor.
             */
            virtual ~KexiDBModule();

            virtual const QString getClassName() const;
            virtual const QString getDescription() const;
    };

}}

#endif

