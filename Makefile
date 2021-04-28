# disable builtin rules
.SUFFIXES:

NAME=Great Burst 2
ROM=gburst2
EXT=gb

# for not installed gbdk and sdcc
GBDKBIN=
SDCCBIN=

# globally installed
LCC=$(GBDKBIN)lcc -v
# use --sdccbindir= for a custom sdcc
ifneq ($(SDCCBIN),)
LCC+= --sdccbindir=$(SDCCBIN)
endif

CPP=$(LCC) -Wf-E
CPPFLAGS=
CC=$(LCC)
CFLAGS=-Wf--opt-code-size -Wf--max-allocs-per-node50000
AR=$(SDCCBIN)sdar
ARFLAGS=
AS=$(LCC)
ASFLAGS=-c
LD=$(LCC)
LDFLAGS=-Wm-yn"$$(echo "$(NAME)" | tr -d ' ' | tr '[:lower:]' '[:upper:]')" -Wm-yt0x03 -Wm-ya1 -Wl-j -Wm-yS -Wl-w

BUILDDIR=build/
BINDIR=bin/
VPATH=src:$(BUILDDIR)

EMU?=sameboy
SFC?=./res/SuperFamiconv/bin/superfamiconv
SFCFLAGS?=-Mgb --no-remap
XXD?=xxd


ROMDEBUG?=0

# 0: no debug
# 1: all labels in .sym
# 2: .cdb file
# 3: disable optimizations
# 9: show complexity of compiled functions (and do 1)
ifeq ($(ROMDEBUG), 0)
BANK=
else
CFLAGS+= -Wf--fverbose-asm
ASFLAGS+= -Wa-a
ifeq ($(ROMDEBUG), 9)
CFLAGS+= -Wf--cyclomatic
else
ifneq ($(ROMDEBUG), 1)
CFLAGS+= -debug
LDFLAGS+= -debug
ifneq ($(ROMDEBUG), 2)
CFLAGS+= -Wf--no-peep -Wf--nolospre -Wf--noloopreverse -Wf--noinduction -Wf--noinvariant
endif
endif
endif
endif

OBJ=$(addprefix $(BUILDDIR),$(addsuffix .rel, main gfx game level interrupt_hack plonger))
GFX=$(addprefix $(BUILDDIR),$(addsuffix .cdata, squont8ng.1bpp great_burst_fg.2bpp great_burst_blocks.2bpp))

########################################################

.PHONY: build run spaceleft statistics clean

build: $(BUILDDIR) $(BINDIR) $(BINDIR)$(ROM).$(EXT)

%/:
	mkdir -p $@

$(BINDIR)$(ROM).$(EXT): $(OBJ)
ifeq ($(ROMDEBUG), 9)
	rm -f $(BINDIR)$(ROM).cdb
endif
ifeq ($(ROMDEBUG), 0)
	rm -f $(BINDIR)$(ROM).cdb
endif
ifeq ($(ROMDEBUG), 1)
	rm -f $(BINDIR)$(ROM).cdb
endif
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILDDIR)%.asm: %.c %.h
	$(CC) $(CFLAGS) -S -o $@ $<

$(BUILDDIR)main.asm: main.c gfx.c gfx.h
	$(CC) $(CFLAGS) -S -o $@ $<

$(BUILDDIR)gfx.asm: gfx.c gfx.h $(GFX)
	$(CC) $(CFLAGS) -S -o $@ $<

# generated
$(BUILDDIR)%.rel: $(BUILDDIR)%.asm
	$(AS) $(ASFLAGS) -o $@ $^

# handwritten
$(BUILDDIR)%.rel: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# .1bpp and .2bpp
$(BUILDDIR)%bpp.cdata: $(BUILDDIR)%bpp
	cat $^ | xxd -i > $@

$(BUILDDIR)%.2bpp: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -i $^ -d $@

$(BUILDDIR)%.2bpp.png: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -i $^ -o $@

$(BUILDDIR)%.1bpp: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -B 1 -i $^ -d $@

$(BUILDDIR)%.1bpp.png: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -B 1 -i $^ -o $@

FILE_ID.DIZ: README.md
	echo "$(NAME)" | tr '[:lower:]' '[:upper:]' > $@
	echo "v0.0.0" >> $@
	echo "-" >> $@
	sed -n '/## $(NAME)/,/##/p' $^ | sed 's/##.*//g' | sed 's/\[\(.*\)\]\(\[.*\]\|(.*)\)/\1/g' | sed '/^$$/d' >> $@

run: build
	$(EMU) $(BINDIR)$(ROM).$(EXT)

clean:
	find bin/ -type f -regex '.*.\(ihx\|map\|noi\|cdb\)' -delete
	find $(BUILDDIR) -type f -regex '.*.\(rel\|c\|h\|cdata\|asm\|2bpp\|1bpp\|tilemap\|pal\|adb\|png\)' -delete

spaceleft: build
	res/romusage/bin/romusage $(BINDIR)$(ROM).noi -g -E

statistics: build
	test -e $(BINDIR)$(ROM).cdb && res/romusage/bin/romusage $(BINDIR)$(ROM).cdb -g
	res/romusage/bin/romusage $(BINDIR)$(ROM).noi -G -E -sH -a

FORCE: