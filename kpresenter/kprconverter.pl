# converts a KPresenter document from the old format to the new one (v2)
# due to the new text object
open(INPUT, "<$ARGV[0]") || die "Cannot open $ARGV[0]";
open(OUTPUT, ">$ARGV[1]") || die "Cannot create $ARGV[1]";
$objType="";
$insideParag=0;
$insideObj=0;
$currentText="";
$currentTextType=0;
while (<INPUT>)
{
  if (/<DOC/)
    {
      s/>$/ syntaxVersion=\"2\">/;
    }
  elsif (/<TEXTOBJ/)
    {
      # Save object type of the TEXTOBJ tag
      $objType=$1 if (m/objType=(\"[0-9]+\")/);
      s/gap=/margin=/;
    }
  elsif (/<PARAGRAPH/)
    {
      $insideParag=1;
      s/<PARAGRAPH /<P /;
      # In the old format we had horzAlign="[0|1|2]" 0=left, 1=center, 2=right
      # In the new one it's align="..." and uses the Qt::AlignmentFlags enums.
      # Qt::AlignLeft=1, AlignRight=2, AlignHCenter=4
      $alignment=$1 if(/horzAlign=(\"[0-2]+\")/);
      $alignment =~ tr/01/14/;
      s/horzAlign=\"[0-2]+\"/align=$alignment/;
      s/>$/ type=$objType>/;
    }
  elsif (/<\/PARAGRAPH>/)
    {
      $insideParag=0;
      s/<\/PARAGRAPH/<\/P/;
      # Flush last text tag
      $_ = $currentText . "</TEXT>\n" . $_ if ($currentText);
      $currentText="";
    }
  elsif (/<LINE/ || /<\/LINE/)
    {
      $_ = "" if ($insideParag); # ignore
    }
  elsif (/<OBJ>/)
    {
      $insideObj=1;
      $_ = ""; # ignore
    }
  elsif (/<\/OBJ>/)
    {
      $insideObj=0;
      $_ = ""; # ignore
    }
  elsif ($insideObj)
    {
      $toprint="";
      if (m/<TYPE value="([0-9]+)"/)
	{
	  $currentTextType=$1;
	  if ($currentTextType) # 1 -> this is a white space
	    {
	      # If we have a previous text element, we keep it (merging)
	      # Otherwise this white space is the first one in the object -> cheat
	      $currentTextType=0 if (!$currentText);
	    }
	  if (!$currentTextType) # 0 -> normal text
	    {
	      # If we have a previous text element, write it out
	      $toprint = $currentText . "</TEXT>\n" if ($currentText);
	      # Start a new text element
	      $currentText = "      <TEXT ";
	    }
	}
      elsif (/<FONT/ && !$currentTextType) # normal text
	{
	  s/\s*<FONT //;
	  s/\/>//;
	  chomp;
	  # Append all attributes
	  $currentText = $currentText . $_;
	}
      elsif (/<COLOR/ && !$currentTextType) # normal text
	{
	  $red=$1 if (m/red=\"([0-9]+)\"/);
	  $green=$1 if (m/green=\"([0-9]+)\"/);
	  $blue=$1 if (m/blue=\"([0-9]+)\"/);
	  # Convert color to HTML representation
	  $currentText = $currentText . sprintf(" color=\"#%02x%02x%02x\"", $red, $green, $blue );
	}
      elsif (m/<TEXT>(.*)<\/TEXT>/)
	{
	  if (!$currentTextType) # normal text
	    {
	      # Close opening tag and append the text - but don t do more yet
	      @entities=split( "(&[a-z]+;)", $1);
	      $text="";
	      foreach ( @entities )
		{
		  if (!/&lt;/ && !/&gt;/ )
		    {
		      # Replace & by &amp; but only if not in an entity
		      s/\&/\&amp;/g;
		    }
		  $text = $text . $_;
		}
	      $currentText = $currentText . ">" . $text;
	    }
	  else
	    {
	      # White space. Simply appending, closing the text tag.
	      $toprint = $currentText . $1 . "</TEXT>\n";
	      $currentText = ""; # reset
	    }
	}
      $_=$toprint;
    }

  print OUTPUT $_;
}

close(INPUT);
close(OUTPUT);
