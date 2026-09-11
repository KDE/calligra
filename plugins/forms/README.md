# Forms plugin

The plugin provides editable form controls, a property sidebar, ODF form
metadata, and interactive PDF widgets when QPDF is available.

Runtime interaction is deliberately out of scope for now.

Current priorities are:

- complete ODF event-listener serialization for forms and controls;
- improve PDF coverage and appearance streams for specialized controls;
- finish relationship handling where it has a meaningful editor or PDF effect;
- expand specialized editors for dates, numbers, images, files, ranges, and
  list/combo data;
- keep round-trip, rendering, and end-to-end PDF tests aligned with real
  documents.
