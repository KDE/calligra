/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _KEXIREPORTPART_H_
#define _KEXIREPORTPART_H_

#include <core/kexipart.h>
#include <core/KexiWindowData.h>

#include <QDomDocument>
#include <KoReportData.h>

/**
 * @short Application Main Window
 * @author Adam Pigg <adam@piggz.co.uk>
 * @version 0.1
 */
class KexiReportPart : public KexiPart::Part
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KexiReportPart(QObject *parent, const QVariantList &l);

    /**
     * Default Destructor
     */
    virtual ~KexiReportPart();

    virtual KexiView* createView(QWidget *parent, KexiWindow* win,
                                 KexiPart::Item &item, Kexi::ViewMode = Kexi::DataViewMode, QMap<QString, QVariant>* staticObjectArgs = 0);
    virtual KexiWindowData* createWindowData(KexiWindow* window);

    virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);

    virtual void initPartActions();

    class TempData : public KexiWindowData
    {
    public:
        TempData(QObject* parent);
        QDomElement reportDefinition;
        QDomElement connectionDefinition;

        /*! true, if \a document member has changed in previous view. Used on view switching.
        Check this flag to see if we should refresh data for DataViewMode. */
        bool reportSchemaChangedInPreviousView;
        QString name;
    };

private slots:
    void slotToolboxActionTriggered(bool checked);
    //! Unchecks toolbox action for @a entity after it is used.
    void slotItemInserted(const QString& entity);

private:
    QString loadReport(const QString&);
    class Private;
    Private* d;
};

#endif // _KEXIREPORTPART_H_
