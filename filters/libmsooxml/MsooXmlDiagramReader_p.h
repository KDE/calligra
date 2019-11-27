/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MSOOXMLDIAGRAMREADER_P_H
#define MSOOXMLDIAGRAMREADER_P_H

#include <cmath>
#include <QString>
#include <QList>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QRect>
#include <QExplicitlySharedDataPointer>
#include <QVariant>

extern QVariant val;
namespace MSOOXML {
    class MsooXmlDiagramReader;
}
class KoXmlWriter;
class KoGenStyles;
class QTextStream;

namespace MSOOXML { namespace Diagram {

/****************************************************************************************************
 * The following classes where designed after the way the dmg-namespace is described in the
 * MSOOXML-specs.
 *
 * Note that we cannot just translate the drawing1.xml cause there are cases where those file doesn't
 * contain the content or all of the content. A typical example where it's needed to eval the whole
 * data1.xml datamodel and the layout1.xml layout-definition are Venn diagrams. But seems it's also
 * possible to just turn any drawing1.xml into a "redirect" to data1.xml+layout1.xml. So, all in all
 * we cannot trust drawing1.xml to contain anything useful :-/
 *
 * See also;
 * - http://wiki.services.openoffice.org/wiki/SmartArt
 * - http://msdn.microsoft.com/en-us/magazine/cc163470.aspx
 * - http://msdn.microsoft.com/en-us/library/dd439435(v=office.12).aspx
 * - http://msdn.microsoft.com/en-us/library/dd439443(v=office.12).aspx
 * - http://msdn.microsoft.com/en-us/library/dd439454(v=office.12).aspx
 * - http://blogs.code-counsel.net/Wouter/Lists/Posts/Post.aspx?ID=36
 */

class AbstractNode;
class PointNode;
class PointListNode;
class ConnectionListNode;
class AbstractAtom;
class LayoutNodeAtom;
class PresentationOfAtom;
class ConstraintAtom;
class AdjustAtom;
class AlgorithmAtom;
class AbstractAlgorithm;
class ShapeAtom;

/// The evaluation context that is passed around and contains all kind of state-information.
class Context
{
    public:
        /// The "doc" root node.
        AbstractNode* m_rootPoint;
        /// A list of connections between nodes.
        ConnectionListNode* m_connections;
        /// The root layout node.
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_rootLayout;
        /// The current parent layout node. This will change during walking through the layout nodes.
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_parentLayout;
        /// A LayoutNodeAtom=>AbstractNode map used to know which layoutnode maps to which datapoint.
        QMultiMap<const LayoutNodeAtom*, AbstractNode*> m_layoutPointMap;
        /// A AbstractNode=>LayoutNodeAtom map used to know which datapoint maps to which layoutnode.
        QMultiMap<AbstractNode*, LayoutNodeAtom*> m_pointLayoutMap;
        /// A list of all visible shapes ordered in there appearance.
        QList<ShapeAtom*> m_shapeList;

        explicit Context();
        ~Context();
        AbstractNode* currentNode() const;
        void setCurrentNode(AbstractNode* node);
    private:
        /// the moving context node
        AbstractNode* m_currentNode;
};

/// The variable-values that can be attached to a LayoutNodeAtom.
class ValueCache
{
    public:
        class ResultWrapper
        {
            public:
                ResultWrapper( ValueCache* parent, const QString& name ): m_parent( parent ), m_name( name ) {}
                ResultWrapper& operator= ( qreal value ) { m_parent->setValue( m_name, value ); return *this; }
                operator qreal() const { return m_parent->value( m_name ); }
            private:
                ValueCache* m_parent;
                const QString m_name;
        };
        ValueCache();
        bool hasNegativeWidth() const;
        bool hasNegativeHeight() const;
        qreal value( const QString& name, bool *valid = 0 ) const;
        bool valueExists( const QString& name );
        void setValue( const QString& name, qreal value );
        qreal operator[]( const QString& name ) const ;    
        ResultWrapper operator[]( const char* name );
        ResultWrapper operator[]( const QString& name );
        operator QMap< QString, qreal >() const;
    private:
        bool isRectValue( const QString& name ) const;
        qreal rectValue( const QString& name ) const;
        void setRectValue( const QString& name, qreal value );
        QMap< QString, qreal > m_mapping;
        QRectF m_rect;
        bool m_unmodified;
        bool m_negativeWidth, m_negativeHeight;
};

/****************************************************************************************************
 * It follows the classes used within the data-model to build up a tree of data-nodes.
 */

/// The AbstractNode is the base class to handle the diagram data-model (content of data1.xml).
class AbstractNode
{
    public:
        const QString m_tagName;
        explicit AbstractNode(const QString &tagName);
        virtual ~AbstractNode();
        virtual void dump(Context* context, int level);
        virtual void dump( QTextStream& device );
        virtual void readElement(Context*, MsooXmlDiagramReader*);
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader);
        AbstractNode* parent() const;
        QList<AbstractNode*> children() const;
        void insertChild(int index, AbstractNode* node);
        void addChild(AbstractNode* node);
        void removeChild(AbstractNode* node);
        QList<AbstractNode*> descendant() const;
        QList<AbstractNode*> peers() const;
    private:
        AbstractNode* m_parent;
        mutable QList<AbstractNode*> m_cachedChildren;
        QMap<int,QList<AbstractNode*> > m_orderedChildren;
        QMap<AbstractNode*,int> m_orderedChildrenReverse;
        QList<AbstractNode*> m_appendedChildren;
};

/// A point in the data-model.
class PointNode : public AbstractNode
{
    public:
        QString m_modelId;
        QString m_type;
        QString m_cxnId;
        QString m_text;
        QMap< QString, QString > prSet;
        explicit PointNode() : AbstractNode("dgm:pt") {}
        ~PointNode() override {}
        void dump(Context* context, int level) override;
        void dump( QTextStream& device ) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
    private:
        void readTextBody(Context*, MsooXmlDiagramReader* reader);
};

/// A list of points in the data-model.
class PointListNode : public AbstractNode
{
    public:
        explicit PointListNode() : AbstractNode("dgm:ptLst") {}
        ~PointListNode() override {}
        void dump(Context* context, int level) override;
        void dump( QTextStream& device ) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
};

/// A connection between two nodes in the data-model.
class ConnectionNode : public AbstractNode
{
    public:
        QString m_modelId;
        QString m_type;
        QString m_srcId;
        QString m_destId;
        QString m_presId;
        QString m_parTransId;
        QString m_sibTransId;
        int m_srcOrd;
        int m_destOrd;
        explicit ConnectionNode() : AbstractNode("dgm:cxn"), m_srcOrd(0), m_destOrd(0) {}
        ~ConnectionNode() override {}
        void dump(Context*, int level) override;
        void dump( QTextStream& device ) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
};

/// A list of connections in the data-model.
class ConnectionListNode : public AbstractNode
{
    public:
        explicit ConnectionListNode() : AbstractNode("dgm:cxnLst") {}
        ~ConnectionListNode() override {}
        void dump(Context* context, int level) override;
        void dump( QTextStream& device ) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
};

/****************************************************************************************************
 * So much for the nodes. Now the atoms are following which are used to add some logic to the
 * data-model and they do provide the functionality to build up a hierarchical layout tree.
 */

/// Base class for layout-operations (content of layout1.xml)
class AbstractAtom : public QSharedData
{
    public:
        const QString m_tagName;
        explicit AbstractAtom(const QString &tagName);
        virtual ~AbstractAtom();
        virtual AbstractAtom* clone(Context* context) = 0;
        virtual void dump(Context* context, int level);
        virtual void readElement(Context* context, MsooXmlDiagramReader* reader);
        virtual void readAll(Context* context, MsooXmlDiagramReader* reader);
        virtual void build(Context* context); // handles ForEachAtom, ChooseAtom, etc.
        virtual void finishBuild(Context* context); // moves constraints around and does other things that can only be done once build() completed.
        virtual void layoutAtom(Context* context);
        virtual void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles);
        QExplicitlySharedDataPointer<LayoutNodeAtom> parentLayout() const;
        QExplicitlySharedDataPointer<AbstractAtom> parent() const;
        QVector< QExplicitlySharedDataPointer<AbstractAtom> > children() const;
        
        int indexOfChild(AbstractAtom* node) const;
        void addChild(AbstractAtom* node);
        void addChild(QExplicitlySharedDataPointer<AbstractAtom> node);
        void insertChild(int index, AbstractAtom* node);
        void insertChild(int index, QExplicitlySharedDataPointer<AbstractAtom> node);
        void removeChild(QExplicitlySharedDataPointer<AbstractAtom> node);
    protected:
        QExplicitlySharedDataPointer<AbstractAtom> m_parent;
        QVector< QExplicitlySharedDataPointer<AbstractAtom> > m_children;
        QList<AbstractNode*> fetchAxis(Context* context, const QString& _axis, const QString &_ptType, const QString& _start, const QString& _count, const QString& _step) const;
    private:
        QList<AbstractNode*> fetchAxis(Context* context, QList<AbstractNode*> list, const QString& axis, const QString &ptType, const QString& start, const QString& count, const QString& step) const;
        QList<AbstractNode*> foreachAxis(Context*, const QList<AbstractNode*> &list, int start, int count, int step) const;
};

/// The algorithm used by the containing layout node. The algorithm defines the behavior of the layout node along with the behavior and layout of the nested layout nodes.
class AlgorithmAtom : public AbstractAtom
{
    public:
        /// The used layout-algorithm.
        enum Algorithm {
            UnknownAlg, ///< Unknown algorithm. This should happen...
            CompositeAlg, ///< The composite algorithm specifies the size and position for all child layout nodes. You can use it to create graphics with a predetermined layout or in combination with other algorithms to create more complex shapes.
            ConnectorAlg, ///< The connector algorithm lays out and routes connecting lines, arrows, and shapes between layout nodes.
            CycleAlg, ///< The cycle algorithm lays out child layout nodes around a circle or portion of a circle using equal angle spacing.
            HierChildAlg, ///< The hierarchy child algorithm works with the hierRoot algorithm to create hierarchical tree layouts. This algorithm aligns and positions its child layout nodes in a linear path under the hierRoot layout node.
            HierRootAlg, ///< The hierarchy root algorithm works with the hierChild algorithm to create hierarchical tree layouts. The hierRoot algorithm aligns and positions the hierRoot layout node in relation to the hierChild layout nodes.
            LinearAlg, ///< The linear algorithm lays out child layout nodes along a linear path.
            PyramidAlg, ///< The pyramid algorithm lays out child layout nodes along a vertical path and works with the trapezoid shape to create a pyramid.
            SnakeAlg, ///< The snake algorithm lays out child layout nodes along a linear path in two dimensions, allowing the linear flow to continue across multiple rows or columns.
            SpaceAlg, ///< The space algorithm is used to specify a minimum space between other layout nodes or as an indication to do nothing with the layout node’s size and position.
            TextAlg ///< The text algorithm sizes text to fit inside a shape and controls its margins and alignment.
        };
        Algorithm m_type;
        QMap<QString, QString> m_params; // list of type=value parameters that modify the default behavior of the algorithm.
        explicit AlgorithmAtom() : AbstractAtom("dgm:alg"), m_type(UnknownAlg) {}
        ~AlgorithmAtom() override {}
        AlgorithmAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void readElement(Context*, MsooXmlDiagramReader* reader) override;
    private:
        QString typeAsString() const;
};

/// The layout node is the basic building block of diagrams. The layout node is responsible for defining how shapes are arranged in a diagram and how the data maps to a particular shape in a diagram.
class LayoutNodeAtom : public AbstractAtom
{
    public:
        QString m_name;
        ValueCache m_values;
        QMap<QString, qreal> m_factors;
        QMap<QString, int> m_countFactors;
        int m_rotateAngle;
        bool m_needsReinit, m_needsRelayout, m_childNeedsRelayout;
        explicit LayoutNodeAtom() : AbstractAtom("dgm:layoutNode"), m_rotateAngle(0), m_needsReinit(true), m_needsRelayout(true), m_childNeedsRelayout(true), m_firstLayout(true), m_algorithmImpl(0) {}
        ~LayoutNodeAtom() override {}
        LayoutNodeAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
        void finishBuild(Context* context) override;
        void layoutAtom(Context* context) override;
        void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) override;

        QList< QExplicitlySharedDataPointer<ConstraintAtom> > constraints() const;
        QList< QExplicitlySharedDataPointer<AdjustAtom> > adjustments() const;

        QExplicitlySharedDataPointer<AlgorithmAtom> algorithm() const;
        void setAlgorithm(QExplicitlySharedDataPointer<AlgorithmAtom> algorithm);

        QList<AbstractNode*> axis(Context* context) const;
        void setAxis(Context* context, const QList<AbstractNode*> &axis);
        void setAxis(Context* context, PresentationOfAtom* atom);

        void setNeedsReinit(bool needsReinit);
        void setNeedsRelayout(bool needsRelayout);

        QList< QExplicitlySharedDataPointer<ShapeAtom> > shapes() const;
        AbstractAlgorithm* algorithmImpl() const;
        AlgorithmAtom::Algorithm algorithmType() const;
        QMap<QString,QString> algorithmParams() const;
        QString algorithmParam(const QString &name, const QString &defaultValue = QString()) const;

        QString variable(const QString &name, bool checkParents = false) const;
        QMap<QString, QString> variables() const;
        void setVariable(const QString &name, const QString &value);
        QMap<QString, qreal> finalValues() const;
        
        QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > fetchLayouts(Context* context, const QString &forAxis, const QString &forName, const QString &ptType) const;
        QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > childrenLayouts() const;
        QVector< QExplicitlySharedDataPointer<LayoutNodeAtom> > descendantLayouts() const;
        QPair<LayoutNodeAtom*,LayoutNodeAtom*> neighbors() const;

        QSizeF childrenUsedSize() const;
        QSizeF childrenTotalSize() const;
        qreal distanceTo(LayoutNodeAtom* otherAtom) const;

    private:
        QMap<QString, QString> m_variables;
        bool m_firstLayout;
        AbstractAlgorithm* m_algorithmImpl;
};

/// Specify size and position of nodes, text values, and layout dependencies between nodes in a layout definition.
class ConstraintAtom : public AbstractAtom
{
    public:
        /// Factor used in a reference constraint or a rule in order to modify a referenced value by the factor defined.
        QString m_fact;
        /// Specifies the axis of layout nodes to apply a constraint or rule to.
        QString m_for;
        /// Specifies the name of the layout node to apply a constraint or rule to.
        QString m_forName;
        /// The operator constraint used to evaluate the condition.
        QString m_op;
        /// Specifies the type of data point to select.
        QString m_ptType;
        /// The point type used in the referenced constraint.
        QString m_refPtType;
        /// Specifies the type of a reference constraint.
        QString m_refType;
        /// The for value of the referenced constraint.
        QString m_refFor;
        /// The name of the layout node referenced by a reference constraint.
        QString m_refForName;
        /// Specifies the constraint to apply to this layout node.
        QString m_type;
        /// Specifies an absolute value instead of reference another constraint.
        QString m_value;
        /// The referenced layout-nodes where we should fetch the values from.
        QList< QExplicitlySharedDataPointer<LayoutNodeAtom> > m_referencedLayouts;
        
        explicit ConstraintAtom() : AbstractAtom("dgm:constr") {}
        ~ConstraintAtom() override {}
        ConstraintAtom* clone(Context* context) override;
        void dump(Context*, int level) override;        
        void readAll(Context*, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
        void finishBuild(Context* context) override;
        void applyConstraint(Context* context, LayoutNodeAtom* atom);
    private:
        QString dump() const;
};

/// Shape adjust value. These can be used to modify the adjust handles supported on various auto shapes. It is only possible to set the initial value, not to modify it using constraints and rules.
class AdjustAtom : public AbstractAtom
{
    public:
        int m_index;
        qreal m_value;
        explicit AdjustAtom() : AbstractAtom("dgm:adj"), m_index(-1) {}
        ~AdjustAtom() override {}
        AdjustAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void applyAdjustment(Context* context, LayoutNodeAtom* atom);
};

/// Rules indicate the ranges of values that a layout algorithm can use to modify the constraint values if it cannot lay out the graphic by using the constraints.
class RuleAtom : public AbstractAtom
{
    public:
        QString m_fact;
        QString m_for;
        QString m_forName;
        QString m_max;
        QString m_ptType;
        QString m_type;
        QString m_value;
        explicit RuleAtom() : AbstractAtom("dgm:rule") {}
        ~RuleAtom() override {}
        RuleAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
};

/// List of atoms.
class ListAtom : public AbstractAtom
{
    public:
        explicit ListAtom(const QString &tagName) : AbstractAtom(tagName) {}
        explicit ListAtom(const QStringRef &tagName) : AbstractAtom(tagName.toString()) {}
        ~ListAtom() override {}
        ListAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
};

/// The shape displayed by the containing layout node. Not all layout nodes display shapes.
class ShapeAtom : public AbstractAtom
{
    public:
        QString m_type;
        QString m_blip;
        bool m_hideGeom;
        explicit ShapeAtom() : AbstractAtom("dgm:shape"), m_hideGeom(false) {}
        ~ShapeAtom() override {}
        QList< QExplicitlySharedDataPointer<AdjustAtom> > adjustments() const;
        ShapeAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
        void writeAtom(Context* context, KoXmlWriter* xmlWriter, KoGenStyles* styles) override;
        //virtual void build(Context* context);
};

/// This element specifies a particular data model point which is to be mapped to the containing layout node.
class PresentationOfAtom : public AbstractAtom
{
    public:
        QString m_axis; // This determines how to navigate through the data model, setting the context node as it moves. 
        QString m_ptType; // dataPointType
        QString m_count;
        QString m_hideLastTrans;
        QString m_start;
        QString m_step;
        explicit PresentationOfAtom() : AbstractAtom("dgm:presOf") {}
        ~PresentationOfAtom() override {}
        PresentationOfAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
    private:
        QString dump() const;
        bool isEmpty() const;
};

/// The if element represents a condition that applies to all it's children.
class IfAtom : public AbstractAtom
{
    public:
        QString m_argument;
        QString m_axis;
        QString m_function;
        QString m_hideLastTrans;
        QString m_name;
        QString m_operator;
        QString m_ptType;
        QString m_start;
        QString m_step;
        QString m_count;
        QString m_value;
        explicit IfAtom(bool isTrue) : AbstractAtom(isTrue ? "dgm:if" : "dgm:else"), m_isTrue(isTrue) {}
        ~IfAtom() override {}
        IfAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        bool isTrue() const;
        bool testAtom(Context* context);
    private:
        bool m_isTrue;
};

/// The choose element wraps if/else blocks into a choose block.
class ChooseAtom : public AbstractAtom
{
    public:
        QString m_name;
        explicit ChooseAtom() : AbstractAtom("dgm:choose") {}
        ~ChooseAtom() override {}
        ChooseAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void readElement(Context* context, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
};

/// A looping structure, similar to a for loop in a programming language, which defines what data model points will use this layout node.
class ForEachAtom : public AbstractAtom
{
    public:
        QString m_axis;
        QString m_hideLastTrans;
        QString m_name;
        QString m_ptType;
        QString m_reference;
        QString m_start;
        QString m_step;
        QString m_count;
        explicit ForEachAtom() : AbstractAtom("dgm:forEach") {}
        ~ForEachAtom() override {}
        ForEachAtom* clone(Context* context) override;
        void dump(Context* context, int level) override;
        void readAll(Context* context, MsooXmlDiagramReader* reader) override;
        void build(Context* context) override;
    private:
        QString dump() const;
};

/// The base class for layout-algorithms.
class AbstractAlgorithm {
    public:
        explicit AbstractAlgorithm();
        virtual ~AbstractAlgorithm();
        virtual QString name() const = 0;
        Context* context() const;
        LayoutNodeAtom* layout() const;
        LayoutNodeAtom* parentLayout() const;
        QList<LayoutNodeAtom*> childLayouts() const;
        qreal defaultValue(const QString& type, const QMap<QString, qreal>& values);
        void doInit(Context* context, QExplicitlySharedDataPointer<LayoutNodeAtom> layout);
        void doLayout();
        void doLayoutChildren();
    protected:
        void setNodePosition(LayoutNodeAtom* l, qreal x, qreal y, qreal w, qreal h);
        virtual qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values);
        virtual void virtualDoInit();
        virtual void virtualDoLayout();
        virtual void virtualDoLayoutChildren();
        QList<Context*> doubleLayoutContext;
    private:
        Context* m_context;        
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_layout;
        QExplicitlySharedDataPointer<LayoutNodeAtom> m_parentLayout;
        AbstractNode* m_oldCurrentNode;
};

/// The composite algorithm specifies the size and position for all child layout nodes. You can use it to create graphics with a predetermined layout or in combination with other algorithms to create more complex shapes.
class CompositeAlgorithm : public AbstractAlgorithm {
    public:
        explicit CompositeAlgorithm() : AbstractAlgorithm() {}
        ~CompositeAlgorithm() override {}
        QString name() const override { return "Composite"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
};

/// The connector algorithm lays out and routes connecting lines, arrows, and shapes between layout nodes.
class ConnectorAlgorithm : public AbstractAlgorithm {
    public:
        explicit ConnectorAlgorithm() : AbstractAlgorithm() {}
        ~ConnectorAlgorithm() override {}
        QString name() const override { return "Connector"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayoutChildren() override;
    private:
        qreal connectorDistance() const;
};

/// The cycle algorithm lays out child layout nodes around a circle or portion of a circle using equal angle spacing.
class CycleAlgorithm : public AbstractAlgorithm {
    public:
        explicit CycleAlgorithm() : AbstractAlgorithm() {}
        ~CycleAlgorithm() override {}
        QString name() const override { return "Cycle"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

/// The linear algorithm lays out child layout nodes along a horizontal or vertical linear path.
class LinearAlgorithm : public AbstractAlgorithm {
    public:
        explicit LinearAlgorithm() : AbstractAlgorithm() {}
        ~LinearAlgorithm() override {}
        QString name() const override { return "Linear"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

/// The snake algorithm lays out child layout nodes along a linear path in two dimensions, allowing the linear flow to continue across multiple rows or columns.
class SnakeAlgorithm : public AbstractAlgorithm {
    public:
        explicit SnakeAlgorithm() : AbstractAlgorithm() {}
        ~SnakeAlgorithm() override {}
        QString name() const override { return "Snake"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

/// The hierarchy root algorithm works with the hierChild algorithm to create hierarchical tree layouts.
class HierarchyAlgorithm : public AbstractAlgorithm {
    public:
        explicit HierarchyAlgorithm(bool isRoot) : AbstractAlgorithm(), m_isRoot(isRoot) {}
        ~HierarchyAlgorithm() override {}
        QString name() const override { return "Hierarchy"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
    private:
        bool m_isRoot; // root or child?
};

/// The pyramid algorithm lays out child layout nodes along a vertical path and works with the trapezoid shape to create a pyramid.
class PyramidAlgorithm : public AbstractAlgorithm {
    public:
        explicit PyramidAlgorithm() : AbstractAlgorithm() {}
        ~PyramidAlgorithm() override {}
        QString name() const override { return "Pyramid"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

/// The space algorithm is used to specify a minimum space between other layout nodes or as an indication to do nothing with the layout node’s size and position.
class SpaceAlg : public AbstractAlgorithm {
    public:
        explicit SpaceAlg() : AbstractAlgorithm() {}
        ~SpaceAlg() override {}
        QString name() const override { return "Space"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

/// The text algorithm sizes text to fit inside a shape and controls its margins and alignment.
class TextAlgorithm : public AbstractAlgorithm {
    public:
        explicit TextAlgorithm() : AbstractAlgorithm() {}
        ~TextAlgorithm() override {}
        QString name() const override { return "Text"; }
    protected:
        qreal virtualGetDefaultValue(const QString& type, const QMap<QString, qreal>& values) override;
        void virtualDoLayout() override;
};

}} // namespace MSOOXML::Diagram

#endif
