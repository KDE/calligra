/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_TOOL_FACTORY_H
#define KO_TOOL_FACTORY_H

#include "flake_export.h"

#include <QString>

class KoCanvasBase;
class KoToolBase;
class QKeySequence;

/**
 * A factory for KoToolBase objects.
 * The baseclass for all tool plugins. Each plugin that ships a KoToolBase should also
 * ship a factory. That factory will extend this class and set variable data like
 * a toolTip and icon in the constructor of that extending class.
 *
 * An example usage would be:<pre>
 * class MyToolFactory : public KoToolFactoryBase {
 * public:
 *    MyToolFactory(const QStringList&)
 *        : KoToolFactoryBase("MyTool") {
 *        setToolTip(i18n("Create object"));
 *        setToolType("dynamic");
 *        setPriority(5);
 *    }
 *    ~MyToolFactory() {}
 *    KoToolBase *createTool(KoCanvasBase *canvas);
 * };
 * K_PLUGIN_FACTORY_WITH_JSON((MyToolFactoryFactory, "mytool.json", registerPlugin<MyToolFactory>();)
 * </pre>
 *
 */
class FLAKE_EXPORT KoToolFactoryBase
{
public:
    /**
     * Create the new factory
     * @param id a string that will be used internally for referencing the tool, for
     *   example for use by the KoToolBase::activateTemporary.
     */
    explicit KoToolFactoryBase(const QString &id);
    virtual ~KoToolFactoryBase();

    /**
     * Instantiate a new tool
     * @param canvas the canvas that the new tool will work on. Should be passed
     *    to the constructor of the tool.
     * @return a new KoToolBase instance, or zero if the tool doesn't want to show up.
     */
    virtual KoToolBase *createTool(KoCanvasBase *canvas) = 0;

    /**
     * return the id for the tool this factory creates.
     * @return the id for the tool this factory creates.
     */
    QString id() const;
    /**
     * Returns The priority of this tool in its section in the toolbox
     * @return The priority of this tool.
     */
    int priority() const;
    /**
     * returns the type of tool, used to group tools in the toolbox
     * @return the type of tool
     */
    QString toolType() const;
    /**
     * return a translated tooltip Text
     * @return a translated tooltip Text
     */
    QString toolTip() const;
    /**
     * return the basename of the icon for this tool
     * @return the basename of the icon for this tool
     */
    QString iconName() const;

    /**
     * Return the id of the shape we can process.
     * This is the shape Id the tool we create is associated with.  So a TextTool for a TextShape.
     * In combination with the toolType the following situations can occur;
     <table><tr><th>Type</th><th>shapeId</th><th>Result</th></tr>
     <tr>
        <td>'main'</td>
        <td>Foo</td>
        <td>Tool will always be visible, but only active when shape with shapeId 'Foo' is in the selection.</td></tr>
     <tr>
        <td>'main'</td>
        <td>''</td>
        <td>Tool will always be visible, but only active when at least one shape is selected</td></tr>
     <tr>
        <td>'main'</td>
        <td>'flake/always'</td>
        <td>Tool will always be visible and enabled.</td></tr>
     <tr>
        <td>'main'</td>
        <td>'flake/edit'</td>
        <td>Tool will be visible no matter which shape is selected (if any), but only
            be enabled when the current layer is editable.</td></tr>
     <tr>
        <td>'dynamic'</td>
        <td>Foo</td>
        <td>Tool will only be visible when shape with shapeId 'Foo' is in the selection.</td></tr>
     <tr>
        <td>'dynamic'</td>
        <td>''</td>
        <td>Tool will always be visible. We recommend you don't use this one.</td></tr>
     <tr>
        <td>"comma separated list of application names"</td>
        <td>see main type</td>
        <td>Similar to the 'main' item if the application name matches with the current application. Otherwise it's similar to 'dynamic', but segmented in its
     own section. If the list includes 'dynamic' it's even added to the dynamic section, when not matching the application name</td></tr> <tr> <td>'other'</td>
        <td>any</td>
        <td>similar to the 'dynamic' items, but segmented in its own section.</td></tr>
     <tr>
        <td>n/a</td>
        <td>/always</td>
        <td>An activation shape id ending with '/always' will make the tool always visible and enabled.</td></tr>
     </table>
     * @see KoShapeFactoryBase::shapeId()
     * @see setActivationShapeId()
     * @return the id of a shape, or an empty string for all shapes.
     */
    QString activationShapeId() const;

    /**
     * Return the default keyboard shortcut for activation of this tool (if
     * the shape this tool belongs to is active).
     *
     * @return the shortcut
     */
    QKeySequence shortcut() const;

    /**
     * Returns the main toolType
     * Each tool has a toolType which it uses to be grouped in the toolbox.
     * The predefined areas are main and dynamic. "main" tools are always
     * shown.
     *
     * @see toolType()
     * @see setToolType()
     */
    static QString mainToolType()
    {
        return "main";
    }
    /**
     * Returns the navigation toolType
     * Each tool has a toolType which it uses to be grouped in the toolbox.
     * The predefined areas are main and dynamic. "navigation" tools are always
     * shown and are for tools that change the settings of the canvas, zoom, pan...
     *
     * @see toolType()
     * @see setToolType()
     */
    static QString navigationToolType()
    {
        return "navigation";
    }
    /**
     * Returns the dynamic toolType
     * Each tool has a toolType which it uses to be grouped in the toolbox.
     * The predefined areas are main and dynamic. Dynamic tools are hidden
     * until the shape they belong to is activated.
     *
     * @see toolType()
     * @see setToolType()
     */
    static QString dynamicToolType()
    {
        return "dynamic";
    }

    /**
     * Set the default shortcut for activation of this tool.
     */
    void setShortcut(const QKeySequence &shortcut);

protected:
    /**
     * Set the tooltip to be used for this tool
     * @param tooltip the tooltip
     */
    void setToolTip(const QString &tooltip);
    /**
     * Set the toolType. used to group tools in the toolbox
     * @param toolType the toolType
     */
    void setToolType(const QString &toolType);
    /**
     * Set an icon to be used in the toolBox.
     * @param iconName the name of the icon per icon theme spec
     */
    void setIconName(const QString &iconName);
    /**
     * Set the priority of this tool, as it is shown in the toolBox; lower number means
     * it will be show more to the front of the list.
     * @param newPriority the priority
     */
    void setPriority(int newPriority);
    /**
     * Set the id of the shape we can process.
     * This is the Id, as passed to the constructor of a KoShapeFactoryBase, that the tool
     * we create is associated with. This means that if a KoTextShape is selected, then
     * all tools that have its id set here will be added to the dynamic part of the toolbox.
     * @param activationShapeId the Id of the shape
     * @see activationShapeId()
     */
    void setActivationShapeId(const QString &activationShapeId);

private:
    class Private;
    Private *const d;
};

#endif
