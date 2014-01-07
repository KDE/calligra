/*
 * Kexi QuickForm Plugin
 * Copyright 2013  Adam Pigg <adam@piggz.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QUICKFORMPART_H
#define QUICKFORMPART_H

#include <core/kexipart.h>
#include <core/KexiWindowData.h>
#include <QDomDocument>

class QuickFormPart : public KexiPart::Part
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    QuickFormPart(QObject *parent, const QVariantList &l);
    
    /**
     * Default Destructor
     */
    virtual ~QuickFormPart();
    
    virtual KexiView* createView(QWidget *parent, KexiWindow* win,
                                 KexiPart::Item &item, Kexi::ViewMode = Kexi::DataViewMode, QMap<QString, QVariant>* staticObjectArgs = 0);
    virtual KexiWindowData* createWindowData(KexiWindow* window);
    
    virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);
    
    virtual void initPartActions();
    
    class TempData : public KexiWindowData
    {
    public:
        TempData(QObject* parent);
        QDomElement quickFormDefinition;
        QDomElement connectionDefinition;
        
        /*! true, if \a document member has changed in previous view. Used on view switching.
         Check t*his flag to see if we should refresh data for DataViewMode. */
        bool quickformSchemaChangedInPreviousView;
        QString name;
    };
    
    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow* window) const;
                                         
private slots:
    void slotToolboxActionTriggered(bool checked);
    //! Unchecks toolbox action for @a entity after it is used.
    void slotItemInserted(const QString& entity);
    
private:
    QString loadQuickForm(const QString&);
    class Private;
    Private* d;
};

#endif // QUICKFORMPART_H
