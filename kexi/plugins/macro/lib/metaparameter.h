/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KOMACRO_METAPARAMETER_H
#define KOMACRO_METAPARAMETER_H

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
#include <ksharedptr.h>

#include "komacro_export.h"

namespace KoMacro {

	/**
	* Class to provide abstract methods for the undocumented 
	* Qt3 QUObject-API functionality.
	*
	* The design tried to limit future porting to Qt4 by providing a
	* somewhat similar API to the Qt4 QMeta* stuff.
	*/
	class KOMACRO_EXPORT MetaParameter : public KShared
	{

			/**
			* Property to get the type of the variable.
			*/
			Q_PROPERTY(Type type READ type)

			/**
			* Property to get the type of the variable as string.
			*/
			Q_PROPERTY(QString typeName READ typeName)

		public:

			/**
			* Shared pointer to implement reference-counting.
			*/
			typedef KSharedPtr<MetaParameter> Ptr;

			/**
			* List of @a MetaParameter instances.
			*/
			typedef QValueList<MetaParameter::Ptr> List;

			/**
			* Constructor.
			*
			* @param signatureargument The signatures argument
			* that will be used to determinate the arguments
			* type. This could be something like "const QString&",
			* "int" or "QMap &lt; QString, QVariant &gt; ".
			*/
			explicit MetaParameter(const QString& signatureargument = QString::null);

			/**
			* Destructor.
			*/
			~MetaParameter();

			/**
			* Possible types the @a MetaParameter could provide.
			*/
			enum Type {
				TypeNone = 0, /// None type, the @a MetaParameter is empty.
				TypeVariant, /// The @a MetaParameter is a QVariant.
				TypeObject /// The @a MetaParameter is a QObject.
			};

			/**
			* @return the @a MetaParameter::Type this variable has.
			*/
			Type type() const;

			/**
			* @return the @a MetaParameter::Type as string. The typename
			* could be "None", "Variant" or "Object".
			*/
			const QString typeName() const;

			/**
			* Set the @a MetaParameter::Type this variable is.
			*/
			void setType(Type type);

			/**
			* @return the @a MetaParameter::Type this variable is.
			*/
			QVariant::Type variantType() const;

			/**
			* Set the @a MetaParameter::Type this variable is.
			*/
			void setVariantType(QVariant::Type varianttype);

		protected:

			/**
			* @internal used method to set the signature argument. Those
			* argument will be used to determinate the arguments type.
			*/
			void setSignatureArgument(const QString& signatureargument);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
