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

class Dialog < Qt::Dialog

    slots 'startClicked()','endClicked()','startOfLineClicked()','endOfLineClicked()'
    slots 'startOfBlockClicked()','endOfBlockClicked()','previousBlockClicked()','nextBlockClicked()'
    slots 'startOfWordClicked()','endOfWordClicked()','previousWordClicked()','nextWordClicked()'
    slots 'upClicked()','downClicked()','leftClicked()','rightClicked()'

    def initialize
        super()
        self.windowTitle = 'Cursor'

        layout = Qt::VBoxLayout.new
        setLayout(layout)

#         fs = KWord.frameSet(0)
#         doc = fs.textDocument()
#         puts "doc.c() ............................"
#         c = doc.c()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "doc.firstc() ............................"
#         c = doc.firstc()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "doc.lastc() ............................"
#         c = doc.lastc()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "doc.rootFrame().c() ............................"
#         c = doc.rootFrame().c()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "doc.rootFrame().firstcPosition() ............................"
#         c = doc.rootFrame().firstcPosition()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "doc.rootFrame().lastcPosition() ............................"
#         c = doc.rootFrame().lastcPosition()
#         puts "position=%s" % c.position()
#         puts "anchor=%s" % c.anchor()
#         puts "hasSelection=%s" % c.hasSelection()
#         puts "selectedText=%s" % c.selectedText()
#         puts "fs.frame(0).c() ............................"
#         puts "fs.frame(0).position=%s" % fs.frame(0).position()
#         puts "fs.frame(0).endPosition=%s" % fs.frame(0).endPosition()
#         c = fs.frame(0).c()
#         puts "c.position=%s" % c.position()
#         puts "c.anchor=%s" % c.anchor()
#         puts "c.hasSelection=%s" % c.hasSelection()
#         puts "c.selectedText=%s" % c.selectedText()
#        puts "KWord.activec() ............................"

        @cursor = KWord.activeCursor()
        if not @cursor
            fs = KWord.frameSet(0)
            doc = fs.textDocument
            @cursor = doc.cursor
            if not @cursor
                raise "Failed to get the KWord cursor."
            end
        end

        w = Qt::Widget.new(self)
        l = Qt::VBoxLayout.new
        #l.setMargin(0)
        w.setLayout(l)
        layout.addWidget(w)
        l.addWidget( Qt::Label.new("position:%s anchor:%s" % [@cursor.position,@cursor.anchor], w) )
        if @cursor.hasSelection
            l.addWidget( Qt::Label.new("selectionStart:%s selectionEnd:%s" % [@cursor.selectionStart,@cursor.selectionEnd], w) )
            #l.addWidget( Qt::Label.new("selectedText:%s" % @cursor.selectedText, w) )
            #l.addWidget( Qt::Label.new("selectedHtml:%s" % @cursor.selectedHtml, w) )
        end

        w = Qt::Widget.new(self)
        l = Qt::HBoxLayout.new
        l.setMargin(0)
        w.setLayout(l)
        layout.addWidget(w)
        btn = Qt::PushButton.new("Start",w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('startClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("End", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('endClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("StartOfLine", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('startOfLineClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("EndOfLine", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('endOfLineClicked()'))
        l.addWidget(btn)

        w = Qt::Widget.new(self)
        l = Qt::HBoxLayout.new
        l.setMargin(0)
        w.setLayout(l)
        layout.addWidget(w)
        btn = Qt::PushButton.new("StartOfBlock", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('startOfBlockClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("EndOfBlock", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('endOfBlockClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("PreviousBlock", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('previousBlockClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("NextBlock", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('nextBlockClicked()'))
        l.addWidget(btn)

        w = Qt::Widget.new(self)
        l = Qt::HBoxLayout.new
        l.setMargin(0)
        w.setLayout(l)
        layout.addWidget(w)
        btn = Qt::PushButton.new("StartOfWord", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('startOfWordClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("EndOfWord", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('endOfWordClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("PreviousWord", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('previousWordClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("NextWord", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('nextWordClicked()'))
        l.addWidget(btn)

        w = Qt::Widget.new(self)
        l = Qt::HBoxLayout.new
        l.setMargin(0)
        w.setLayout(l)
        layout.addWidget(w)
        btn = Qt::PushButton.new("Up", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('upClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("Down", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('downClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("Left", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('leftClicked()'))
        l.addWidget(btn)
        btn = Qt::PushButton.new("Right", w)
        connect(btn, SIGNAL('clicked(bool)'), self, SLOT('rightClicked()'))
        l.addWidget(btn)

        resize( Qt::Size.new(200, 100).expandedTo( minimumSizeHint ) );
    end

    def cursor
        return @cursor
    end

    #WordLeft
    #PreviousCharacter
    #NextCharacter

    def startClicked()
        @cursor.movePosition(@cursor.Start)
    end
    def endClicked()
        @cursor.movePosition(@cursor.End)
    end
    def startOfLineClicked()
        @cursor.movePosition(@cursor.StartOfLine)
    end
    def endOfLineClicked()
        @cursor.movePosition(@cursor.EndOfLine)
    end

    def startOfBlockClicked()
        @cursor.movePosition(@cursor.StartOfBlock)
    end
    def endOfBlockClicked()
        @cursor.movePosition(@cursor.EndOfBlock)
    end
    def previousBlockClicked()
        @cursor.movePosition(@cursor.PreviousBlock)
    end
    def nextBlockClicked()
        @cursor.movePosition(@cursor.NextBlock)
    end

    def startOfWordClicked()
        @cursor.movePosition(@cursor.StartOfWord)
    end
    def endOfWordClicked()
        @cursor.movePosition(@cursor.EndOfWord)
    end
    def previousWordClicked()
        @cursor.movePosition(@cursor.PreviousWord)
    end
    def nextWordClicked()
        @cursor.movePosition(@cursor.NextWord)
    end

    def upClicked()
        @cursor.movePosition(@cursor.Up)
    end
    def downClicked()
        @cursor.movePosition(@cursor.Down)
    end
    def leftClicked()
        @cursor.movePosition(@cursor.Left)
    end
    def rightClicked()
        @cursor.movePosition(@cursor.Right)
    end

end

dialog = Dialog.new
dialog.exec
#puts "setActiveCursor ...............1"
#KWord.setActiveCursor( dialog.cursor )
#puts "setActiveCursor ...............2"
