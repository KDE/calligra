/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOODFFORM_H
#define KOODFFORM_H

#include "koodf_export.h"
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QVector>
#include <memory>

class KoXmlElement;
class KoXmlWriter;

/** Value model for the ODF form: module. */
class KOODF_EXPORT KoOdfForm
{
public:
    enum class ControlKind {
        Unknown,
        Text,
        Textarea,
        FormattedText,
        Number,
        Date,
        Time,
        Button,
        Checkbox,
        Radio,
        Combobox,
        Listbox,
        Password,
        Hidden,
        File,
        FixedText,
        ValueRange,
        Image,
        ImageFrame,
        Frame,
        Grid,
        GenericControl,
    };

    static ControlKind controlKindFromString(const QString &kind);
    static QString controlKindName(ControlKind kind);

    class KOODF_EXPORT Control
    {
    public:
        virtual ~Control() = default;
        virtual bool loadOdf(const KoXmlElement &element);
        virtual void saveOdf(KoXmlWriter &writer) const = 0;
        QString name() const;
        QString id() const;
        QString title() const;
        QString label() const;
        void setLabel(const QString &label);
        /** Additional form attributes, including control-specific appearance and state. */
        QString formAttribute(const QString &name) const;
        void setFormAttribute(const QString &name, const QString &value);
        struct Entry {
            QString label;
            QString value;
            bool selected = false;
            QString element = QStringLiteral("option");
        };
        /** List options, combo-box items, or grid column headings, in document order. */
        QVector<Entry> entries() const;
        void setEntries(const QVector<Entry> &entries);
        QString value() const;
        QString currentValue() const;
        QString dataField() const;
        QString linkedCell() const;
        QString xformsBind() const;
        QString eventHandler(const QString &event) const;
        QMap<QString, QString> eventHandlers() const;
        bool disabled() const;
        bool printable() const;
        bool readOnly() const;
        bool inputRequired() const;
        int tabIndex() const;
        bool tabStop() const;
        void setName(const QString &);
        void setId(const QString &);
        void setTitle(const QString &);
        void setValue(const QString &);
        void setCurrentValue(const QString &);
        void setDataField(const QString &);
        void setLinkedCell(const QString &);
        void setXformsBind(const QString &);
        void setEventHandler(const QString &event, const QString &handler);
        void setDisabled(bool);
        void setPrintable(bool);
        void setReadOnly(bool);
        void setInputRequired(bool);
        void setTabIndex(int);
        void setTabStop(bool);

    protected:
        void saveChildren(KoXmlWriter &) const;
        void saveCommonAttributes(KoXmlWriter &) const;
        QString m_name, m_id, m_title, m_value, m_currentValue, m_dataField, m_linkedCell, m_xformsBind;
        bool m_disabled = false, m_printable = true, m_readOnly = false, m_inputRequired = false, m_tabStop = true;
        int m_tabIndex = 0;
        QMap<QString, QString> m_formAttributes;
        QVector<Entry> m_entries;
        QByteArray m_childrenXml;
    };

    class KOODF_EXPORT Text : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        QString maxLength() const;
        bool multiLine() const;
        QString echoChar() const;
        void setMaxLength(const QString &);
        void setMultiLine(bool);
        void setEchoChar(const QString &);

    private:
        QString m_maxLength, m_echoChar;
        bool m_multiLine = false;
    };
    class KOODF_EXPORT Textarea : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT FormattedText : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Password : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Hidden : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT File : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT FixedText : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Number : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        QString minValue() const;
        QString maxValue() const;
        QString stepSize() const;
        void setMinValue(const QString &);
        void setMaxValue(const QString &);
        void setStepSize(const QString &);

    private:
        QString m_minValue, m_maxValue, m_stepSize;
    };
    class KOODF_EXPORT Date : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Time : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT ValueRange : public Number
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Button : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        bool defaultButton() const;
        bool toggle() const;
        void setDefaultButton(bool);
        void setToggle(bool);

    private:
        bool m_defaultButton = false, m_toggle = false;
    };
    class KOODF_EXPORT Checkbox : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        bool selected() const;
        bool tristate() const;
        void setSelected(bool);
        void setTristate(bool);

    private:
        bool m_selected = false, m_tristate = false;
    };
    class KOODF_EXPORT Radio : public Checkbox
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Combobox : public Text
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        bool autoComplete() const;
        void setAutoComplete(bool);

    private:
        bool m_autoComplete = false;
    };
    class KOODF_EXPORT Listbox : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        bool multiple() const;
        bool dropdown() const;
        QString listSource() const;
        void setMultiple(bool);
        void setDropdown(bool);
        void setListSource(const QString &);

    private:
        bool m_multiple = false, m_dropdown = false;
        QString m_listSource;
    };
    class KOODF_EXPORT Image : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
        QString imageData() const;
        QString imagePosition() const;
        QString imageAlign() const;
        void setImageData(const QString &);
        void setImagePosition(const QString &);
        void setImageAlign(const QString &);

    private:
        QString m_imageData, m_imagePosition, m_imageAlign;
    };
    class KOODF_EXPORT ImageFrame : public Image
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Frame : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT Grid : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };
    class KOODF_EXPORT GenericControl : public Control
    {
    public:
        bool loadOdf(const KoXmlElement &) override;
        void saveOdf(KoXmlWriter &) const override;
    };

    class KOODF_EXPORT Property
    {
    public:
        bool loadOdf(const KoXmlElement &);
        void saveOdf(KoXmlWriter &) const;
        QString name() const;
        QString value() const;
        void setName(const QString &);
        void setValue(const QString &);

    private:
        QString m_name, m_value;
    };
    using ListProperty = Property;
    using ListValue = Property;
    using Option = Property;
    using Item = Property;
    using Column = Property;
    using ConnectionResource = Property;

    /**
     * An xforms:model is intentionally open-ended in the ODF schema.  Keep
     * its complete XML representation so extensions and unknown children can
     * be round-tripped without retaining a KoXmlElement from the source tree.
     */
    class KOODF_EXPORT Model
    {
    public:
        bool loadOdf(const KoXmlElement &element);
        void saveOdf(KoXmlWriter &writer) const;
        QByteArray xml() const;
        void setXml(const QByteArray &xml);

    private:
        QByteArray m_xml;
    };

    bool loadOdf(const KoXmlElement &element);
    void saveOdf(KoXmlWriter &writer) const;
    bool isEmpty() const;
    QString controlKind(const QString &id) const;
    ControlKind controlKindEnum(const QString &id) const;
    QString addControl(const QString &kind);
    QString addControl(ControlKind kind, const QString &id = {});
    /** Removes a control and returns whether a control with this ID existed. */
    bool removeControl(const QString &id);
    /** Returns an owned copy of the referenced control, or nullptr if it is unknown. */
    std::unique_ptr<Control> controlById(const QString &id) const;
    QString labelForControl(const QString &id) const;
    QVector<QString> radioGroup(const QString &name) const;
    /** Updates common properties without replacing the concrete control type or ID. */
    bool setControlProperties(const QString &id, const Control &properties);
    QString name() const;
    QString command() const;
    QString datasource() const;
    QString method() const;
    QString xformsSubmission() const;
    QVector<Text> texts() const;
    QVector<Textarea> textareas() const;
    QVector<FormattedText> formattedTexts() const;
    QVector<Number> numbers() const;
    QVector<Date> dates() const;
    QVector<Time> times() const;
    QVector<Button> buttons() const;
    QVector<Checkbox> checkboxes() const;
    QVector<Radio> radios() const;
    QVector<Combobox> comboboxes() const;
    QVector<Listbox> listboxes() const;
    QVector<Model> models() const;

private:
    QString m_name, m_command, m_datasource, m_method, m_xformsSubmission;
    QVector<Text> m_texts;
    QVector<Textarea> m_textareas;
    QVector<FormattedText> m_formattedTexts;
    QVector<Number> m_numbers;
    QVector<Date> m_dates;
    QVector<Time> m_times;
    QVector<Button> m_buttons;
    QVector<Checkbox> m_checkboxes;
    QVector<Radio> m_radios;
    QVector<Combobox> m_comboboxes;
    QVector<Listbox> m_listboxes;
    QVector<Password> m_passwords;
    QVector<Hidden> m_hiddenControls;
    QVector<File> m_files;
    QVector<FixedText> m_fixedTexts;
    QVector<ValueRange> m_valueRanges;
    QVector<Image> m_images;
    QVector<ImageFrame> m_imageFrames;
    QVector<Frame> m_frames;
    QVector<Grid> m_grids;
    QVector<GenericControl> m_genericControls;
    QVector<Model> m_models;
};

#endif
