# KSpread docker script written in Ruby and using QtRuby.
#
# (C)2007 Sebastian Sauer <mail@dipe.org>
# Licensed under LGPLv2+

require 'Kross'
require 'KSpread'
require 'KoDocker'

require 'Qt'
#if(require 'Qt')

class Snippets

    class Base < Qt::Widget
        slots 'slotExecute()'
        def initialize(parent, caption, description)
            super(parent)
            @caption = caption
            @description = description
        end
        def slotExecute()
            raise NotImplementedError("#{self.class.name}#slotExecute() is not implemented.")
        end
    end

    class Snippet_Execute < Base
        slots 'slotExecuteCode()'
        def initialize(parent)
            super(parent, "Execute", "Execute Scripting Code")
        end
        def slotExecute()
            dialog = findChild(Qt::Dialog, "dialog")
            if dialog == nil
                dialog = Qt::Dialog.new(self, Qt::Window)
                dialog.setObjectName("dialog")
                dialog.setModal(false)
                layout = Qt::VBoxLayout.new(dialog)
                layout.setMargin(0)
                layout.setSpacing(0)
                dialog.setLayout(layout)
                interpreterlayout = Qt::HBoxLayout.new(dialog)
                layout.addLayout(interpreterlayout)
                interpreterlayout.addWidget(Qt::Label.new("Interpreter:", dialog))
                @interpreterCombo = Qt::ComboBox.new(dialog)
                interpreterlayout.addWidget(@interpreterCombo, 1)
                interpr = Kross.interpreters()
                interpr.each do |n|
                    @interpreterCombo.addItem(n)
                    if n == "ruby"
                        @interpreterCombo.setCurrentIndex(@interpreterCombo.count()-1)
                    end
                end
                runbtn = Qt::PushButton.new('Execute', dialog)
                connect(runbtn, SIGNAL('clicked()'), self, SLOT('slotExecuteCode()'))
                interpreterlayout.addWidget(runbtn)
                #closebtn = Qt::PushButton.new('Close', dialog)
                #connect(closebtn, SIGNAL('clicked()'), dialog, SLOT('reject()'))
                #interpreterlayout.addWidget(closebtn)
                @edit = Qt::TextEdit.new(dialog)
                @edit.setLineWrapMode(Qt::TextEdit::NoWrap)
                layout.addWidget(@edit)
                @edit.plainText = ""
                @edit.setFocus()
                dialog.resize(560,400)
            end
            dialog.show()
        end
        def slotExecuteCode()
            puts "EXECUTE START"
            action = Kross.action("SnippetAction")
            action.setInterpreter(@interpreterCombo.currentText)
            action.setCode(@edit.plainText)
            action.trigger()
            #action.destroyLater()
            puts "EXECUTE DONE"
        end
    end

    class Snippet_Informations < Base
        def initialize(parent)
            super(parent, "Information", "Environment Details")

            ObjectSpace.define_finalizer(self,
                                     self.class.method(:finalize).to_proc)

        end
        def slotExecute()
            text = "<h3>Ruby</h3><table>"
                text += "<tr><td>RUBY_VERSION</td><td>" + RUBY_VERSION + "</td></tr>"
                begin
                    text += "<tr><td>RUBY_PATCHLEVEL</td><td>" + RUBY_PATCHLEVEL + "</td></tr>"
                rescue
                    text += "<tr><td>RUBY_PATCHLEVEL</td><td></td></tr>"
                end
                text += "<tr><td>RUBY_RELEASE_DATE</td><td>" + RUBY_RELEASE_DATE + "</td></tr>"
                text += "<tr><td>RUBY_PLATFORM</td><td>" + RUBY_PLATFORM + "</td></tr>"
                text += "</table>"
            text += "<h3>QtRuby</h3><table>"
                text += "<tr><td>Qt.version</td><td>" + Qt.version + "</td></tr>"
                text += "<tr><td>Qt.qtruby_version</td><td>" + Qt.qtruby_version + "</td></tr>"
            text += "</table>"
            text += "<h3>Environment Variables</h3><table>"
                ENV.keys.each do |n|
                    text += "<tr><td>" + n + "</td><td>" + ENV[n] + "</td></tr>"
                end
            text += "</table>"

            dialog = Qt::Dialog.new(self)
            layout = Qt::VBoxLayout.new(dialog)
            layout.setMargin(0)
            layout.setSpacing(0)
            dialog.setLayout(layout)
            browser = Qt::TextBrowser.new(dialog)
            layout.addWidget(browser)
            browser.html = text
            dialog.resize(560,400)
            dialog.exec()
        end
def Snippet_Informations.finalize(id)
        puts "Object #{id} dying at #{Time.new}"
end

    end

end

class SnippetsWidget < Qt::Widget

    class WidgetListBox < Qt::TableWidget
        def initialize(parent, scriptaction)
            super(parent)
            setRowCount(0)
            setColumnCount(1)
            horizontalHeader().hide()
            verticalHeader().hide()
            setSelectionMode(Qt::AbstractItemView::SingleSelection)
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff)
            horizontalHeader().setStretchLastSection(true)
            setAlternatingRowColors(true)
            @scriptaction = scriptaction
            @snippets = {}
        end
        def scriptAction
            return @scriptaction
        end
        def addItem(snippetClazzName)
            snippet = Snippets.const_get(snippetClazzName).new(self)
            @snippets[snippetClazzName] = snippet #this is a hack which seems to be needed to keep the qwidget-instance alive.

            row = rowCount()
            setRowCount(row + 1)
            widget = Qt::Widget.new(self)
            layout = Qt::GridLayout.new(widget)
            layout.setMargin(2)
            layout.setSpacing(0)
            layout.setColumnStretch(0, 1)
            widget.setLayout(layout)

            caption = snippet.instance_variable_get("@caption")
            layout.addWidget(Qt::Label.new(caption, widget), 0, 0)

            runbtn = Qt::PushButton.new('Run', widget)
            runbtn.setProperty("snippetClazzName", Qt::Variant.new(snippetClazzName))
            connect(runbtn, SIGNAL('clicked()'), snippet, SLOT('slotExecute()'))
            layout.addWidget(runbtn, 0, 1, 2, 1)

            description = snippet.instance_variable_get("@description")
            layout.addWidget(Qt::Label.new("<i>" + description + "</i>", widget), 1, 0)

            setCellWidget(row, 0, widget)
            setRowHeight(row, widget.height() + 8)
        end
    end

    def initialize(parent, scriptaction)
        super(parent)
        layout = Qt::VBoxLayout.new(self)
        layout.setMargin(0)
        layout.setSpacing(0)
        setLayout(layout)
        @listbox = WidgetListBox.new(self, scriptaction)
        layout.addWidget(@listbox)
        Snippets.constants.each do |n|
            if n.index('Snippet_') == 0
                @listbox.addItem(n)
            end
        end
        layout.addWidget(@label)
    end
end

$voidptr = KoDocker.toVoidPtr()
$wdg = Qt::Internal.kross2smoke($voidptr, Qt::DockWidget)
$label = SnippetsWidget.new($wdg, self)
$wdg.setWidget($label)

puts "Ruby Docker Script 'snippetsdocker.rb' Loaded!"
