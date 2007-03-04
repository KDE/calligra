#!/usr/bin/env kross

# Load the Qt4-QtRuby module.
begin
    require 'Qt'
rescue LoadError
    raise "Failed to load the required QtRuby module. Please install Qt4-QtRuby."
end

# Load the KWord scripting module.
begin
    require 'KWord'
rescue LoadError
    # If loading failed (cause the script was not executed from within
    # a running KWord instance) we try to load the Kross module and
    # ask it to load the KWord module for us.
    require 'Kross'
    KWord = Kross.module('KWord')

    # Testcase to fill KWord with some content. You are also able to e.g.
    # load a document here with "KWord.document().openURL('/path/file.odt')"
    # (maybe openURL was renamed to openUrl meanwhile ;)
    mytextframeset1 = KWord.addTextFrameSet("myFirstTextFrame")
    mytextframeset1.addTextFrame()
    mytextframeset1.textDocument().setHtml("<h1>First Header</h1><p>Some text</p><p>Some more text with <b>bold</b> and <i>italic</i> and <u>underline</u> to test the stuff.</p>")
    mytextframeset2 = KWord.addTextFrameSet("mySecondTextFrame")
    mytextframeset2.addTextFrame()
    mytextframeset2.textDocument().setHtml("<h1>Second Header</h1><h2>First Sub Header</h2><p>Some text.</p><h2>Second Sub Header</h2><p>Some text.</p>")
end

#########################################################################
# Pages - Each item is able to provide different pages

# The Page interface class.
class Page
    def initialize(label)
        @label = label
    end
    def label
        return @label
    end
    def createWidget(parent)
        return nil
    end
end

# The PropertyPage class implements properties displayed within a table.
class PropertyPage < Page

    class Property
        def initialize(name, value)
            @name = name
            @value = value
        end
        def name
            return @name.to_s
        end
        def value
            return @value.to_s
        end
    end

    def initialize(label, properties = [])
        super(label)
        @properties = properties
    end

    def createWidget(parent)
        table = Qt::TableWidget.new(parent)
        table.setColumnCount(2)
        table.setHorizontalHeaderLabels( ["Name","Value"] )
        table.verticalHeader().hide
        table.alternatingRowColors = true
        #table.horizontalHeader().resizeSections( Qt::HeaderView::ResizeToContents )
        table.horizontalHeader().setStretchLastSection(true)
        #table.setEditTriggers( Qt::AbstractItemView::AllEditTriggers )
        table.setEditTriggers( Qt::AbstractItemView::NoEditTriggers )
        #@table.setSortingEnabled(true)
        for property in @properties
            rows = table.rowCount
            table.insertRow(rows)
            table.setItem(rows, 0, Qt::TableWidgetItem.new( property.name ))
            table.setItem(rows, 1, Qt::TableWidgetItem.new( property.value ))
        end
        return table
    end

    def addProperty(property)
        @properties.push(property)
    end

end

# The TextEditPage class implements a simple texteditor.
class TextEditPage < Page
    def initialize(label, text = "")
        super(label)
        @text = text
    end
    def createWidget(parent)
        edit = Qt::TextEdit.new(parent)
        if @text
            edit.plainText = @text.to_s
        end
        edit.readOnly = true
        return edit
    end
end

# The TextBrowserPage class implements a simple browser.
class TextBrowserPage < Page
    def initialize(label, text = "")
        super(label)
        @text = text
    end
    def createWidget(parent)
        edit = Qt::TextBrowser.new(parent)
        if @text
            edit.html = @text.to_s
        end
        edit.readOnly = true
        return edit
    end
end

#########################################################################
# Items displayed within the TreeModel.

# Item is the interface class for all items.
class Item

    def initialize(parentitem = nil, data = nil)
        @parentitem = parentitem
        @data = data
        @childitems = []
        @pages = []
    end

    def parent
        return @parentitem
    end

    def child(row)
        return @childitems[row]
    end

    def childCount
        return @childitems.length
    end

    def row
        #if !@parentitem.nil?
        #    return @parentitem.childitems.index(self)
        #end
        return 0
    end

    def page(index)
        return @pages[index]
    end

    def pageCount
        return @pages.length
    end

end

# FrameItem for KWord Frame objects.
class FrameItem < Item

    class TextFrameItem < Item
        def initialize(parentitem, textframe)
            super(parentitem, textframe)
            #cursor = textframe.firstCursorPosition()
            for i in 0..(textframe.childFrameCount() - 1)
                f = textframe.childFrame(i)
                if f
                    puts "TextFrameItem %s" % f.to_s
                    @childitems.push( TextFrameItem.new(self, f) )
                end
            end
        end
        def data(column)
            return Qt::Variant.new( "TextFrame %i-%i" % [@data.firstPosition(),@data.lastPosition()] )
        end
    end

    class TextDocumentItem < Item
        def initialize(frameitem, textdoc)
            super(frameitem, textdoc)

            @pages.push(
                PropertyPage.new("Properties", [
                    PropertyPage::Property.new("width", @data.width),
                    PropertyPage::Property.new("height", @data.height),
                    PropertyPage::Property.new("pageCount", @data.pageCount),
                    PropertyPage::Property.new("isModified", @data.isModified),
                ] )
            )

            varpage = PropertyPage.new("Variables")
            for n in @data.variableNames
                varpage.addProperty( PropertyPage::Property.new(n, @data.variableValue(n)) )
            end
            @pages.push(varpage)

            @pages.push( TextEditPage.new("Text", @data.toText) )
            @pages.push( TextEditPage.new("HTML", @data.toHtml) )
            @pages.push( TextEditPage.new("Style", @data.defaultStyleSheet) )
            @pages.push( TextBrowserPage.new("Preview", @data.toHtml) )

            roottextframe = textdoc.rootFrame
            @childitems.push( TextFrameItem.new(self, roottextframe) )

            cursor = roottextframe.firstCursorPosition()
            #@childitems.push( TextCursorItem.new(self, ) )
        end
        def data(column)
            return Qt::Variant.new("Document")
        end
    end

    def initialize(framesetitem, frame)
        super(framesetitem, frame)
        frameset = frame.frameSet
        textdoc = frameset.textDocument
        if textdoc != 0
            @childitems.push( TextDocumentItem.new(self, textdoc) )
        end
        @pages.push(
            PropertyPage.new("Properties", [
                PropertyPage::Property.new("width", @data.width),
                PropertyPage::Property.new("shapeId", @data.shapeId),
                PropertyPage::Property.new("visible", @data.isVisible),
                PropertyPage::Property.new("scaleX", @data.scaleX),
                PropertyPage::Property.new("scaleY", @data.scaleY),
                PropertyPage::Property.new("rotation", @data.rotation),
                PropertyPage::Property.new("shearX", @data.shearX),
                PropertyPage::Property.new("shearY", @data.shearY),
                PropertyPage::Property.new("width", @data.width),
                PropertyPage::Property.new("height", @data.height),
                PropertyPage::Property.new("positionX", @data.positionX),
                PropertyPage::Property.new("positionY", @data.positionY),
                PropertyPage::Property.new("zIndex", @data.zIndex),
            ] )
        )
    end

    def data(column)
        return Qt::Variant.new( @data.shapeId().to_s )
    end

end

# FramesetItem for KWord FrameSet objects.
class FramesetItem < Item

    def initialize(rootitem, frameset)
        super(rootitem, frameset)
        for i in 0..(frameset.frameCount() - 1)
            f = frameset.frame(i)
            if f
                puts "FramesetItem frame=%s" % f.to_s
                @childitems.push( FrameItem.new(self, f) )
            end
        end
        @pages.push(
            PropertyPage.new("Properties", [
                PropertyPage::Property.new("name", @data.name),
                PropertyPage::Property.new("frameCount", @data.frameCount),
                PropertyPage::Property.new("isText", @data.textDocument != 0),
            ] )
        )

    end

    def data(column)
        return Qt::Variant.new( @data.name().to_s )
    end

end

# Rootitem within the TreeModel.
class RootItem < Item

    def initialize
        super(nil, nil)
        for i in 0..(KWord.frameSetCount() - 1)
            f = KWord.frameSet(i)
            if f
                puts "RootItem %s" % f.to_s
                @childitems.push( FramesetItem.new(self, f) )
            end
        end
    end

    def data(column)
        return Qt::Variant.new
    end

end

#########################################################################
# TreeModel - The model that handles the items.

class TreeModel < Qt::AbstractItemModel

    def initialize(parent)
        super(parent)
        @rootItem = RootItem.new
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

    def index(row, column, parent)
        if !parent.valid?
            parentItem = @rootItem
        else
            parentItem = parent.internalPointer
        end
        childItem = parentItem.child(row)
        if !childItem.nil?
            return createIndex(row, column, childItem)
        end
        return Qt::ModelIndex.new
    end

    def parent(index)
        if index.valid?
            parentItem = index.internalPointer.parent
            if parentItem != @rootItem
                return createIndex(parentItem.row, 0, parentItem)
            end
        end
        return Qt::ModelIndex.new
    end

    def data(index, role)
        if !index.valid?
            return Qt::Variant.new
        end
        if role == Qt::DisplayRole
            item = index.internalPointer
            return item.data(index.column)
        end
        return Qt::Variant.new
    end

    def rowCount(parent)
        if !parent.valid?
            parentItem = @rootItem
        else
            parentItem = parent.internalPointer
        end
        return parentItem.childCount
    end

end

#########################################################################
# Dialog - The main dialog window.

class Dialog < Qt::Dialog

    slots 'slotCurrentChanged(const QModelIndex&, const QModelIndex&)'

    def initialize
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
        @view.expandAll
        @view.show
        splitter.addWidget(@view)

        editorwidget = Qt::Widget.new(self)
        editorlayout = Qt::VBoxLayout.new
        editorlayout.setMargin(0)
        editorwidget.setLayout(editorlayout)
        #@label = Qt::Label.new(editorwidget)
        #editorlayout.addWidget(@label)
        @tab = Qt::TabWidget.new(self)
        editorlayout.addWidget(@tab)
        #layout.addSpacing(10)
        #layout.addStretch(1)
        splitter.addWidget(editorwidget)
        editorwidget.resize( Qt::Size.new(360, 400).expandedTo( editorwidget.minimumSizeHint ) )

        connect(@view.selectionModel, SIGNAL('currentChanged(const QModelIndex&, const QModelIndex&)'), self, SLOT('slotCurrentChanged(const QModelIndex&, const QModelIndex&)'))
        resize( Qt::Size.new(700, 520).expandedTo( minimumSizeHint ) );
    end

    def slotCurrentChanged(current, prev)
        while @tab.count > 0; @tab.removeTab(0); end
        if current.valid?
            #@label.text = @view.model().data(current, Qt::DisplayRole)
            for i in 0..(current.internalPointer.pageCount() - 1)
                page = current.internalPointer.page(i)
                widget = page.createWidget(@tab)
                if widget != nil
                    @tab.addTab(widget, page.label)
                end
            end
        end
    end

end

dialog = Dialog.new
dialog.exec
