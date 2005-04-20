Kross uses PyCXX 5.3.0 (http://cxx.sourceforge.net/)
to access the Python C API.

Following patches where applied and send back
to the PyCXX team.

- cxx530-unsigned.patch
  http://sourceforge.net/tracker/index.php?func=detail&aid=1085205&group_id=3180&atid=303180
- cxx530-isinstance.patch
  http://sourceforge.net/tracker/index.php?func=detail&aid=1178048&group_id=3180&atid=303180
- cxx530-dir.patch
  http://sourceforge.net/tracker/index.php?func=detail&aid=1186676&group_id=3180&atid=303180

I also changed some namespace- and include stuff to
get PyCXX compiled the way we use it. I'll try to
minimize such changes and add patches for such
changes within next merge.
