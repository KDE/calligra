/*
 * kmynuminput.h
 *
 *  Copyright (c) 1997 Patrick Dowler <dowler@morgul.fsh.uvic.ca>
 *  Copyright (c) 2000 Dirk A. Mueller <mueller@kde.org>
 *
 *  Requires the Qt widget libraries, available at no cost at
 *  http://www.troll.no/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#ifndef K_MYNUMINPUT_H
#define K_MYNUMINPUT_H

#include <qwidget.h>
#include <qspinbox.h>

class QLabel;
class QSlider;
class QLineEdit;
class QLayout;

/* ------------------------------------------------------------------------ */

/**
 * You need to inherit from this class if you want to implement K*NumInput
 * for a different variable type
 *
 */

class KMyNumInput : public QWidget
{
    Q_OBJECT
public:
    /**
     * Default constructor
     *
     */
    KMyNumInput(QWidget* parent=0, const char* name=0);

    /**
     * @param below A pointer to another @ref KNumInput.
     *
     */
    KMyNumInput(KMyNumInput* below, QWidget* parent=0, const char* name=0);
    ~KMyNumInput();

    /**
     * Set the text and alignment of the main description label.
     *
     * @param label The text of the label.
     *              Use @ref QString::null to remove an existing one.
     *
     * @param a one of @p AlignLeft, @p AlignHCenter, YAlignRight and
     *          @p AlignTop, @p AlignVCenter, @p AlignBottom.
     *          default is @p AlignLeft | @p AlignTop.
     *
     * The vertical alignment flags have special meaning with this
     * widget:
     *
     *     @li @p AlignTop     The label is placed above the edit/slider
     *     @li @p AlignVCenter The label is placed left beside the edit
     *     @li @p AlignBottom  The label is placed below the edit/slider
     *
     */
    virtual void setLabel(QString label, int a = AlignLeft | AlignTop);

    /**
     * Set the spacing of tickmarks for the slider.
     *
     * @param minor Minor tickmark separation.
     * @param major Major tickmark separation.
     */
    void setSteps(int minor, int major);

    /**
     * Specifie that this widget may stretch horizontally, but is
     * fixed vertically (like @ref QSpinBox itself).
     */
    QSizePolicy sizePolicy() const;

    /**
     * Return a size which fits the contents of the control.
     *
     * @return the preferred size necessary to show the control
     */
    virtual QSize sizeHint() const;

protected:
    /**
     * Call this function whenever you change something in the geometry
     * of your @ref KNumInput child.
     *
     */
    void layout(bool deep);

    /**
     * You need to overwrite this method and implement yourlayout
     * calculations there.
     *
     * See @ref KIntNumInput / @ref KDoubleNumInput implementation
     * for details.
     *
     */
    virtual void doLayout() = 0;

    KMyNumInput* m_prev, *m_next;
    int m_colw1, m_colw2;

    QLabel*  m_label;
    QSlider* m_slider;
    QSize    m_sizeSlider, m_sizeLabel;

    int      m_alignment;

private:
    void init();

    class KMyNumInputPrivate;
    KMyNumInputPrivate *d;
};


/* ------------------------------------------------------------------------ */

class KMyDoubleLine;

/**
 * KDoubleNumInput combines a @ref QSpinBox and optionally a @ref QSlider
 * with a label to make an easy to use control for setting some float
 * parameter. This is especially nice for configuration dialogs,
 * which can have many such combinated controls.
 *
 * The slider is created only when the user specifies a range
 * for the control using the setRange function with the slider
 * parameter set to "true".
 *
 * A special feature of KDoubleNumInput, designed specifically for
 * the situation when there are several KDoubleNumInputs in a column,
 * is that you can specify what portion of the control is taken by the
 * QSpinBox (the remaining portion is used by the slider). This makes
 * it very simple to have all the sliders in a column be the same size.
 *
 * It uses KDoubleValidator validator class. KDoubleNumInput enforces the
 * value to be in the given range.
 *
 * @see KIntNumInput
 * @short An input control for real numbers, consisting of a spinbox and a slider.
 */

class KMyDoubleNumInput : public KMyNumInput
{
    Q_OBJECT
    Q_PROPERTY( double value READ value WRITE setValue )
    Q_PROPERTY( QString suffix READ suffix WRITE setSuffix )
    Q_PROPERTY( QString prefix READ prefix WRITE setPrefix )
    Q_PROPERTY( QString specialValueText READ specialValueText WRITE setSpecialValueText )

public:
    /**
     * Constructs an input control for double values
     * with initial value 0.0.
     */
    KMyDoubleNumInput(QWidget *parent=0, const char *name=0);
    /**
     * Constructor
     *
     * @param value  initial value for the control
     * @param parent parent QWidget
     * @param name   internal name for this widget
     */
    KMyDoubleNumInput(double value, QWidget *parent=0, const char *name=0);

    /**
     * destructor
     */
    virtual ~KMyDoubleNumInput();

    /**
     * Constructor
     *
     * put it below other KNumInput
     *
     * @param  below
     * @param  value  initial value for the control
     * @param  parent parent QWidget
     * @param  name   internal name for this widget
     **/
    KMyDoubleNumInput(KMyNumInput* below, double value, QWidget* parent=0, const char* name=0);

    /**
     * @return the current value.
     */
    double value(bool forceUpdate=false) const;
    /**
     * @return the suffix.
     * @see #setSuffix()
     */
    QString suffix() const;
    /**
     * @return the prefix.
     * @see #setPrefix()
     */
    QString prefix() const;
    /**
     * @return the format.
     * @see #setFormat()
     */
    const char *format() const;
    /**
     * @return the string displayed for a special value.
     * @see #setSpecialValueText()
     */
    QString specialValueText() const { return m_specialvalue; }

     /**
     * @param lower  lower bound on range
     * @param upper  upper bound on range
     * @param step   step size for the QSlider
     */
    void setRange(double lower, double upper, double step=1, bool slider=true);

    /**
     * Sets the format string that should be used to display the double value.
     * The format string follows the same rules as the printf() function or
     * @ref QString::sprintf(). You can use this if you want to set the number
     * of digits to be displayed, etc.
     */
    void setFormat(const char* format);

    /**
     * Sets the special value text. If set, the spin box will display
     * this text instead of the numeric value whenever the current
     * value is equal to @ref #minVal(). Typically this is used for indicating
     * that the choice has a special (default) meaning.
     */
    void setSpecialValueText(const QString& text);

    /**
     * @reimplemented
     */
    virtual void setLabel(QString label, int a = AlignLeft | AlignTop);
    /**
     * @reimplemented
     */
    virtual QSize minimumSizeHint() const;
    /**
     * @reimplemented
     */
    virtual bool eventFilter(QObject*, QEvent*);

public slots:
    /**
     * Sets the value of the control.
     */
    void setValue(double);

    /**
     * Sets the suffix to be displayed to @p suffix. Use QString::null to disable
     * this feature. Note that the suffix is attached to the value without any
     * spacing. So if you prefer to display a space separator, set suffix
     * to something like " cm".
     * @see #setSuffix()
     */
    void setSuffix(const QString &suffix);

    /**
     * Sets the prefix to be displayed to @p prefix. Use QString::null to disable
     * this feature. Note that the prefix is attached to the value without any
     * spacing.
     * @see #setPrefix()
     */
    void setPrefix(const QString &prefix);

signals:
    /**
     * Emitted every time the value changes (by calling @ref setValue() or
     * by user interaction).
     */
    void valueChanged(double);

private slots:
    void sliderMoved(int);

protected:

    /**
     * @reimplemented
     */
    virtual void doLayout();
    /**
     * @reimplemented
     */
    void resizeEvent ( QResizeEvent * );
    virtual void resetEditBox();

    KMyDoubleLine*   edit;

    bool     m_range;
    double   m_lower, m_upper, m_step;

    QSize    m_sizeEdit;

    friend class KMyDoubleLine;

private:
    void init(double value);
    QString  m_units, m_specialvalue, m_prefix, m_suffix;
    char     *m_format;
    double   m_value;
    int      m_sliderstep; // currently unused

    class KMyDoubleNumInputPrivate;
    KMyDoubleNumInputPrivate *d;
};

/* --------------------------------------------------------------------------- */


#endif // K_MYNUMINPUT_H
