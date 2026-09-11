/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include <KoSignalCompressor.h>
#include <KoToolBase.h>
#include <KoToolFactoryBase.h>
#include <QHash>
#include <QPointer>

class KoFormShape;
class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QFormLayout;
class QWidget;
class QListWidget;

class KoFormTool : public KoToolBase
{
public:
    explicit KoFormTool(KoCanvasBase *canvas);
    void activate(ToolActivation activation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void mousePressEvent(KoPointerEvent *) override
    {
    }
    void mouseMoveEvent(KoPointerEvent *) override
    {
    }
    void mouseReleaseEvent(KoPointerEvent *event) override;

protected:
    QWidget *createOptionWidget() override;

private:
    void updateProperties();
    void commitProperties();
    void shapeSelectionChanged();
    void rebuildSpecificProperties();
    KoFormShape *m_shape = nullptr;
    QPointer<QWidget> m_options;
    QLabel *m_type = nullptr;
    QLineEdit *m_name = nullptr;
    QLineEdit *m_title = nullptr;
    QComboBox *m_enabled = nullptr;
    QComboBox *m_readOnly = nullptr;
    QComboBox *m_printable = nullptr;
    QComboBox *m_tabStop = nullptr;
    QSpinBox *m_tabIndex = nullptr;
    QFormLayout *m_specificForm = nullptr;
    int m_specificStartRow = 0;
    QHash<QString, QWidget *> m_specificProperties;
    QListWidget *m_entries = nullptr;
    KoSignalCompressor m_previewCompressor;
};

class KoFormToolFactory : public KoToolFactoryBase
{
public:
    KoFormToolFactory();
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
