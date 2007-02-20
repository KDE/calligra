#!/usr/bin/env kross

begin
    require 'Qt'
rescue LoadError
    raise "Failed to load the required QtRuby module. Please install QtRuby."
end

begin
    require 'KWord'
rescue LoadError
    require 'Kross'
    KWord = Kross.module('KWord')

    # testcase to fill KWord with some content ;)
    mytextframeset = KWord.addTextFrameSet("myTextFrame")
    mytextframe = mytextframeset.addTextFrame()
    textdoc = mytextframeset.textDocument()
    textdoc.setHtml("<h1>The Header</h1><p>Some text</p>")
end

class Item

    def initialize(parentitem = nil, data = nil)
        @parentitem = parentitem
        @data = data
        @childitems = []
    end

    def parent
        return @parentitem
    end

    def child(row)
        return @childitems[row]
    end

    def childCount
        return @childitems.length()
    end

    def row
        #if !@parentitem.nil?
        #    return @parentitem.childitems.index(self)
        #end
        return 0
    end

    def createEditorWidget(parent)
    end
end

class FrameItem < Item

    def initialize(framesetitem, frame)
        super(framesetitem, frame)
    end

    def data(column)
        return Qt::Variant.new( @data.shapeId().to_s() )
    end

    def createEditorWidget(parent)
        widget = Qt::Widget.new(parent)
        layout = Qt::VBoxLayout.new
        layout.setMargin(0)
        widget.setLayout(layout)

        visible = @data.isVisible()
        layout.addWidget( Qt::Label.new("visible: #{visible}") )

        scaleX = @data.scaleX()
        layout.addWidget( Qt::Label.new("scaleX: #{scaleX}") )
        scaleY = @data.scaleY()
        layout.addWidget( Qt::Label.new("scaleY: #{scaleY}") )

        rotation = @data.rotation()
        layout.addWidget( Qt::Label.new("rotation: #{rotation}") )

        shearX = @data.shearX()
        layout.addWidget( Qt::Label.new("shearX: #{shearX}") )
        shearY = @data.shearY()
        layout.addWidget( Qt::Label.new("shearY: #{shearY}") )

        width = @data.width()
        layout.addWidget( Qt::Label.new("width: #{width}") )
        height = @data.height()
        layout.addWidget( Qt::Label.new("height: #{height}") )

        positionX = @data.positionX()
        layout.addWidget( Qt::Label.new("positionX: #{positionX}") )
        positionY = @data.positionY()
        layout.addWidget( Qt::Label.new("positionY: #{positionY}") )
        zIndex = @data.zIndex()
        layout.addWidget( Qt::Label.new("zIndex: #{zIndex}") )

        layout.addStretch(1)
        return widget
    end
end

class FramesetItem < Item

    def initialize(rootitem, frameset)
        super(rootitem, frameset)
        for i in 0..(frameset.frameCount() - 1)
            frame = frameset.frame(i)
            @childitems.push( FrameItem.new(self, frame) )
        end
    end

    def data(column)
        return Qt::Variant.new( @data.name().to_s() )
    end

    def createEditorWidget(parent)
        widget = Qt::Widget.new(parent)
        layout = Qt::VBoxLayout.new
        layout.setMargin(0)
        widget.setLayout(layout)

        frameCount = @data.frameCount()
        layout.addWidget( Qt::Label.new("frameCount: #{frameCount}") )

        isText = @data.textDocument() != nil
        layout.addWidget( Qt::Label.new("isText: #{isText}") )

        layout.addStretch(1)
        return widget
    end
end

class RootItem < Item

    def initialize()
        super(nil, nil)
        for i in 0..(KWord.frameSetCount() - 1)
            frameset = KWord.frameSet(i)
            @childitems.push( FramesetItem.new(self, frameset) )
        end
    end

    def data(column)
        return Qt::Variant.new()
    end

end

class TreeModel < Qt::AbstractItemModel

    def initialize(parent)
        super(parent)
        @rootItem = RootItem.new()
    end

    def columnCount(parent)
        return 1
    end

    def flags(index)
        if !index.valid?
            return Qt::ItemIsEnabled
        end
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable
    end

    #def headerData(section, orientation, role)
    #    if orientation == Qt::Horizontal && role == Qt::DisplayRole
    #        return Qt::Variant.new(@rootItem.data(section))
    #    end
    #    return Qt::Variant.new()
    #end

    def index(row, column, parent)
        if !parent.valid?
            parentItem = @rootItem
        else
            parentItem = parent.internalPointer()
        end
        childItem = parentItem.child(row)
        if !childItem.nil?
            return createIndex(row, column, childItem)
        end
        return Qt::ModelIndex.new()
    end

    def parent(index)
        if index.valid?
            childItem = index.internalPointer()
            parentItem = childItem.parent()
            if parentItem != @rootItem
                return createIndex(parentItem.row(), 0, parentItem)
            end
        end
        return Qt::ModelIndex.new()
    end

    def data(index, role)
        if !index.valid?
            return Qt::Variant.new()
        end
        if role == Qt::DisplayRole
            item = index.internalPointer
            return item.data(index.column)
        end
        return Qt::Variant.new()
    end

    def rowCount(parent)
        if !parent.valid?
            parentItem = @rootItem
        else
            parentItem = parent.internalPointer
        end
        return parentItem.childCount()
    end

end

class Editor < Qt::Widget

    slots 'slotCurrentChanged(const QModelIndex&, const QModelIndex&)'

    def initialize(parent, view)
        super(parent)
        @view = view

        layout = Qt::VBoxLayout.new
        setLayout(layout)

        @label = Qt::Label.new(self)
        layout.addWidget(@label)

        @widget = Qt::Widget.new(self)
        layout.addWidget(@widget, 1)

        #layout.addSpacing(10)
        #layout.addStretch(1)
    end

    def slotCurrentChanged(current, prev)
        hide()
        @widget.hide()
        @widget.destroy()
        @widget = Qt::Widget.new(self)
        layout.addWidget(@widget, 1)
        if !current.valid?
            @label.text = ""
        else
            @label.text = @view.model().data(current, Qt::DisplayRole)
            current.internalPointer.createEditorWidget(@widget)
        end
        show()
    end

end

class Dialog < Qt::Dialog

    def initialize()
        super()
        self.windowTitle = 'Document Tree'

        layout = Qt::HBoxLayout.new
        setLayout(layout)
        splitter = Qt::Splitter.new(self)
        layout.addWidget(splitter)

        @view = Qt::TreeView.new(self)
        @view.header().setVisible(false)
        @view.model = TreeModel.new(self)
        @view.selectionModel = Qt::ItemSelectionModel.new(@view.model)
        @view.expandAll()
        @view.show()
        splitter.addWidget(@view)

        scrollArea = Qt::ScrollArea.new(self)
        scrollArea.widgetResizable = true
        #scrollArea.backgroundRole = Qt::Palette::Dark
        @editor = Editor.new(scrollArea, @view)
        scrollArea.widget = @editor
        splitter.addWidget(scrollArea)
        scrollArea.resize( Qt::Size.new(260, 200).expandedTo( scrollArea.minimumSizeHint() ) );

        connect(@view.selectionModel, SIGNAL('currentChanged(const QModelIndex&, const QModelIndex&)'), @editor, SLOT('slotCurrentChanged(const QModelIndex&, const QModelIndex&)'))
        resize( Qt::Size.new(580, 380).expandedTo( minimumSizeHint() ) );
    end

end

dialog = Dialog.new()
dialog.exec()
