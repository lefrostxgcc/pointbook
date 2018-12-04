bin_PROGRAMS = pointbook
pointbook_SOURCES = main.c sql.h sql.c
pointbook_CPPFLAGS = @PKGGTK_CFLAGS@ @SQLITE3_CFLAGS@ $(AM_CPPFLAGS) \
-DDATA_PATH=\"$(pkgdatadir)\"
pointbook_CFLAGS = $(AM_CFLAGS)
pointbook_LDFLAGS = $(PKGGTK_LDFLAGS) $(SQLITE3_LDFLAGS) $(AM_LDFLAGS)	
pointbook_LDADD = $(PKGGTK_LIBS) $(SQLITE3_LIBS)
