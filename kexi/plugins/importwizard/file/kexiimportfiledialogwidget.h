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
	void loadPlugin(bool load);

    signals:
	void nextPage();
	void filterHasBeenLoaded(KexiFilter *filter,const KURL& url=KURL());

    private:
	QMap<QString,QString> m_map;
	bool m_load;
	KexiFilterManager *m_filterManager;
	KexiFileImportWizard *m_wiz;
};

#endif
