#
# OSD target makefile
#

-include ../Makedefs

ifeq ($(PANASAS_OSD),1)
SRC := pan_coll.c pan_mtq.c pan_attr.c pan_obj.c osd.c pan_io.c cdb.c osd-sense.c list-entry.c
INC := attr.h obj.h osd-types.h osd.h cdb.h list-entry.h target-sense.h io.h
else
SRC := attr.c db.c obj.c osd-schema.c osd.c io.c cdb.c osd-sense.c list-entry.c
SRC += osd-schema.c coll.c mtq.c
INC := attr.h db.h obj.h osd-types.h osd.h cdb.h list-entry.h target-sense.h io.h
INC += coll.h mtq.h
endif
DEP := .depend
OBJ := $(SRC:.c=.o)
TESTDIR := ./tests/
UTILDIR := ../osd-util/ ../dbus/
STGTDIR := ../tgt/
INITDIR := ../osd-initiator/
OSDTARGETLIB := libosdtgt.a

STGTLIB := ../stgt/libstgt.a
UTILLIB := ../osd-util/libosdutil.a

IB_HW_OF_LIBS= -L/usr/local/openib-iser/lib

ifeq ($(FreeBSD_Make),1)
LIBS += -L/usr/local/lib/
ifeq ($(PANASAS_OSD),1)
CFLAGS += -I/usr/local/include -I/usr/include -I$(STGTDIR)/usr/bsd -I$(STGTDIR)/usr -I../../../export/include/ 
else
CFLAGS += -I/usr/local/include -I/usr/include -I$(STGTDIR)/usr/bsd -I$(STGTDIR)/usr 
endif
CFLAGS += -D__MAKE_BSD_BUILD__
endif

ifeq ($(PANASAS_OSD),1)
CFLAGS += -D__PANASAS_OSD__
ifeq ($(PANASAS_OSDSIM),1)
CFLAGS += -D__PANASAS_OSDSIM__
endif
else
# PANASAS_OSD does not use DB for anything
TGT_EXTRA_LIBS = -lsqlite3
endif

LIBS += $(TGT_EXTRA_LIBS) -lm -lcrypto -laio -lavahi-core -lavahi-common \
	$(IB_HW_OF_LIBS) -libverbs -lrdmacm

ifeq ($(DBUS_STATS),1)
UTILLIB += ../dbus/libosddbus.a
CFLAGS += -D__DBUS_STATS__ -I../dbus -I/usr/local/include/dbus-1.0 -I/usr/local/include/dbus-1.0/include
LIBS += -ldbus-glib-1 -ldbus-1 -lglib-2.0 -lgobject-2.0 -lgio-2.0
endif

CC := gcc
CPP_M := -MM
LD := $(CC)
COPTS := $(OPT)
COPTS += -fPIC -foptimize-sibling-calls -D_GNU_SOURCE
CWARN := -Wall -W -Wpointer-arith -Wwrite-strings -Wcast-align -Wcast-qual \
	-Wbad-function-cast -Wundef -Wmissing-prototypes \
	-Wmissing-declarations -Wnested-externs
	#-Wmissing-declarations -Wnested-externs -Werror
CFLAGS += -g $(COPTS) $(CWARN) -I.. -Wno-unused -I../osd-util 

ifeq ($(PVFS_OSD_INTEGRATED),1)
CFLAGS += -DPVFS_OSD_INTEGRATED
endif

# To enable the use of tick tracing inside OSD target library.  It is
# driven by tgt, and we must pick up the header files from it.  When changing
# TICK_TRACE symbols, be sure to compile in stgt to update ttrace_def.h
# first, then build osd-target, then go back to stgt to link the binary.
#CFLAGS += -DTICK_TRACING -include ../stgt-git/usr/ttrace.h

.PHONY: UTIL TESTS STGT

.SUFFIXES: .c .o .i

all :: $(OSDTARGETLIB)

# Need -whole-archive this to get the constructors for the various device
# types.  Another way would be to force a ref to an object in the file
# somehow, or just use shared libs.
tgtd: $(STGTLIB) $(OSDTARGETLIB) $(UTILLIB)
	$(LD) -o $@ -Wl,-whole-archive $(STGTLIB) -Wl,-no-whole-archive \
		$(OSDTARGETLIB) $(UTILLIB) $(LIBS)

TESTS: $(UTILLIB) $(OSDTARGETLIB)
	make -C $(TESTDIR)

$(OSDTARGETLIB): $(OBJ)
	ar cr $@ $^

$(STGTLIB): STGT
	make -C $(STGTDIR)

$(UTILLIB): UTIL
	make -C $(UTILDIR)

%.o: %.c Makefile ../Makedefs
	$(CC) $(CFLAGS) -c $< -o $@

osd-schema.c: osd.schema
	@( echo "const char osd_schema[] =";\
	   sed 's/^/"/; s/$$/\\n"/' < $< ;\
	   echo ";" ) > $@

#depend
ifneq (clean, $(MAKECMDGOALS))
-include $(DEP)
endif

all :: $(DEP)
$(DEP) : osd-schema.c
	@$(CC) $(CPP_M) $(CFLAGS) $(SRC) > $(DEP)

clean:
	rm -f tgtd $(OSDTARGETLIB) $(OBJ) $(DEP) osd-schema.c

tags: FORCE osd-schema.c
	ctags -R $(SRC) $(INC) $(TESTDIR) $(UTILDIR) $(INITDIR) osd-schema.c

FORCE:;

# distribution tarball
.PHONY: dist
MV := osd-target
MVD := $(MV)-$(shell date +%Y%m%d)
dist:
	rm -rf $(MV) $(MVD)
	mkdir $(MVD)
	svn list -R | grep -v '/$$' | rsync --files-from=/dev/stdin ./ $(MVD)/
	ln -s $(MVD) $(MV)
	tar cf - $(MV) $(MVD) | bzip2 -9c > $(MVD).tar.bz2
	rm -rf $(MV) $(MVD)

