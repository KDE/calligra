/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef PARAGRAPHSTYLESMODEL_H
#define PARAGRAPHSTYLESMODEL_H

#include <QModelIndex>

class ParagraphStylesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QObject* textEditor READ textEditor WRITE setTextEditor NOTIFY textEditorChanged)
    Q_PROPERTY(QFont cursorFont READ cursorFont NOTIFY cursorFontChanged)
    Q_PROPERTY(int currentStyle READ currentStyle NOTIFY cursorFontChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)

public:
    enum ParagraphStyleRoles {
        Name = Qt::UserRole + 1,
        Current,
        Font,
        FontFamily,
        FontPointSize,
        FontWeight,
        FontItalic,
        FontUnderline
    };
    ParagraphStylesModel();
    ~ParagraphStylesModel() override;
    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;

    QObject* document() const;
    void setDocument(QObject* newDocument);

    QObject* textEditor() const;
    void setTextEditor(QObject* newEditor);

    Q_SLOT void cursorPositionChanged();

    Q_INVOKABLE void activate(int index);

    QFont cursorFont() const;
    int currentStyle() const;

    qreal zoomLevel() const;
    void setZoomLevel(const qreal& newZoom);
Q_SIGNALS:
    void documentChanged();
    void textEditorChanged();
    void cursorFontChanged();
    void zoomLevelChanged();

private:
    class Private;
    Private* d;
};

#endif // PARAGRAPHSTYLESMODEL_H
