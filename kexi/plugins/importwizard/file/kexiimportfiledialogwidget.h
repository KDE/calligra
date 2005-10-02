/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

//Based on koffice/lib/kofficeui/koTemplateChooseDia.cc

#ifndef KEXI_IMPORT_FILE_DIALOG_WIDGET_H
#define KEXI_IMPORT_FILE_DIALOG_WIDGET_H

#include <kfiledialog.h>
#include <kurl.h>
#include <filters/kexifilter.h>

class KexiFilterManager;
class KexiFileImportWizard;

class KexiImportFileDialogWidget : public KFileDialog
{
    Q_OBJECT
    public :
        KexiImportFileDialogWidget( KexiFilterManager *filterManager,KexiFileImportWizard *wiz,const 
		QString& startDir=0, const QString& filter =0, QWidget *parent=0,
		const char *name=0,bool modal=0);

        KURL currentURL();

        // Return true if the current URL exists, show msg box if not
        bool checkURL();
	

	void setMimePluginMap(const QMap<QString,QString>  map);
	void setLoad(bool load);
	void initiateLoading();

    protected:
        virtual void accept();
	virtual void reject();
	void loadPlugin(bool load);

    signals:
	void nextPage();
	void filterHasBeenLoaded(KexiFilter *filter,const KURL& url=KURL());
	void canceled();
    private:
	QMap<QString,QString> m_map;
	bool m_load;
	KexiFilterManager *m_filterManager;
	KexiFileImportWizard *m_wiz;
};

#endif
