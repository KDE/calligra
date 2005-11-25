/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIMIGRATIONOPTIONSDIALOG_H
#define KEXIMIGRATIONOPTIONSDIALOG_H

#include <kdialogbase.h>

class KexiCharacterEncodingComboBox;

namespace KexiMigration {

//! @short Import Options dialog.
//! It is currently used for MDB driver only
//! @todo Hardcoded. Move such code to KexiMigrate drivers.
class OptionsDialog : public KDialogBase
{
	public:
		OptionsDialog( const QString& databaseFile, const QString& selectedEncoding, QWidget* parent = 0 );
		virtual ~OptionsDialog();

		KexiCharacterEncodingComboBox* encodingComboBox() const;

	protected:
		KexiCharacterEncodingComboBox *m_encodingComboBox;
};
}

#endif
