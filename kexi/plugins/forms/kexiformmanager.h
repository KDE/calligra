/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIFORMMANAGER_H
#define KEXIFORMMANAGER_H

#include <formmanager.h>
#include <kexipart.h>

//! @internal
//! Used to customize KFormDesigner::FormManager behaviour.
class KEXIFORMUTILS_EXPORT KexiFormManager : public KFormDesigner::FormManager
{
	Q_OBJECT

	public:
		KexiFormManager(KexiPart::Part *parent, const QStringList& supportedFactoryGroups,
			const char* name = 0);
		virtual ~KexiFormManager();

		virtual KAction* action( const char* name );
		virtual void enableAction( const char* name, bool enable );

	public slots:
		//! Receives signal from KexiDataSourcePage about changed form's data source
		void setFormDataSource(const QCString& mime, const QCString& name);

		//! Receives signal from KexiDataSourcePage about changed widget's data source
		void setDataSourceFieldOrExpression(const QString& string);

	protected:
		inline QString translateName( const char* name ) const;
//		virtual bool loadFormFromDomInternal(Form *form, QWidget *container, QDomDocument &inBuf);
//		virtual bool saveFormToStringInternal(Form *form, QString &dest, int indent = 0);

		KexiPart::Part* m_part;
};

QString KexiFormManager::translateName( const char* name ) const
{
	QString n( name );
	//translate to our name space:
	if (n.startsWith("align_") || n.startsWith("adjust_") || n.startsWith("layout_")
		|| n=="format_raise" || n=="format_raise" || n=="taborder" | n=="break_layout")
	{
		n.prepend("formpart_");
	}
	return n;
}

#endif
