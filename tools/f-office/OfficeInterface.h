#ifndef OFFICEINTERFACE_H
#define OFFICEINTERFACE_H

#include <QtPlugin>
#include <QWidget>
#include <QStringList>

class OfficeInterface
{
public:

    virtual ~OfficeInterface() {}

    // This is used to give information about the currently active doc to the plugin
    // 0 if there is no document open
    virtual void setDocument(void *doc) = 0;

    // Asks the plugin to create it's view for the main program to use
    virtual QWidget *view() = 0;

    // Should return the name of the plugin
    virtual QString pluginName() = 0;

    // Should return list of document types in presence of which this plugin
    // should be shown in the menubar.
    // Possible values : All, presentation, document, spreadsheet
    //
    // NOTE: this is not actually used yet in the main program and may be extented
    // in the future.
    virtual QStringList pluginSupportTypes() = 0;
};

Q_DECLARE_INTERFACE(OfficeInterface, "com.office.Freoffice.OfficeInterface/1.0")

#endif
