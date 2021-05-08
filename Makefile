# disable builtin rules
.SUFFIXES:

NAME=Great Burst 2
ROM=gburst2
EXT=gb

# for not installed gbdk and sdcc
GBDKBIN=
SDCCBIN=
HDBIN=res/hUGETracker/hUGEDriver/
DINTLV=res/deinterleaver

# globally installed
LCC=$(GBDKBIN)lcc -v
# use --sdccbindir= for a custom sdcc
ifneq ($(SDCCBIN),)
LCC+= --sdccbindir=$(SDCCBIN)
endif

CPP=$(LCC) -Wf-E
CPPFLAGS=
CC=$(LCC)
CFLAGS=-Wf--opt-code-size -Wf--max-allocs-per-node50000 -I$(HDBIN)include/
AR=$(SDCCBIN)sdar
ARFLAGS=
AS=$(LCC)
ASFLAGS=-c
LD=$(LCC)
LDFLAGS=-Wm-yn"$$(echo "$(NAME)" | tr -d ' ' | tr '[:lower:]' '[:upper:]')" -Wm-yt0x03 -Wm-ya1 -Wl-j -Wm-yS -Wl-w -Wm-yc -Wm-ys

RGBAS=rgbasm
#HUGET=wine res/hUGETracker/hugetracker.exe
RGB2SDAS=$(HDBIN)tools/rgb2sdas

BUILDDIR=build/
BINDIR=bin/
VPATH=src:$(BUILDDIR)

EMU?=sameboy
SFC?=./res/SuperFamiconv/bin/superfamiconv
SFCFLAGS?=-Mgb --no-remap
SFCCFLAGS?=-Mgbc --no-remap
SFCSFLAGS?=-Msnes --no-remap
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

OBJ=$(addprefix $(BUILDDIR),$(addsuffix .rel, main sara game plonger  interrupt_hack set_t_fix hUGEDriver gfx level msx))
GFX=$(addprefix $(BUILDDIR),$(addsuffix .cdata, squont8ng.1bpp great_burst_fg.2bpp great_burst_blocks.2bpp great_burst_blocks_cgb.2bpp great_burst_bg.2bpp great_burst_special.2bpp oga_spring_2021_gb.2bpp oga_spring_2021_cgb.2bpp oga_spring_2021_overlay_cgb.2bpp great_burst_border_sgb.4bpp great_burst_blocks_cgb.pal oga_spring_2021_cgb.pal oga_spring_2021_overlay_cgb.pal great_burst_border_sgb.pal oga_spring_2021_gb.map oga_spring_2021_cgb.map oga_spring_2021_cgb.attr great_burst_border_sgb.map))

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

$(BUILDDIR)%cgb.2bpp: gfx/%cgb.png
	$(SFC) tiles $(SFCCFLAGS) -i $^ -d $@

$(BUILDDIR)great_burst_blocks_cgb.2bpp: gfx/great_burst_blocks_cgb.png
	$(SFC) tiles $(SFCCFLAGS) -D -F -i $^ -d $@

$(BUILDDIR)great_burst_special.2bpp: gfx/great_burst_special.png
	$(SFC) tiles $(SFCFLAGS) -H 16 -i $^ -d $@

$(BUILDDIR)oga_spring_2021_overlay_cgb.2bpp: gfx/oga_spring_2021_overlay_cgb.png
	$(SFC) tiles $(SFCCFLAGS) -D -i $^ -d $@

$(BUILDDIR)%.2bpp.png: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -i $^ -o $@

$(BUILDDIR)%.1bpp: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -B 1 -i $^ -d $@

$(BUILDDIR)%.1bpp.png: gfx/%.png
	$(SFC) tiles $(SFCFLAGS) -B 1 -i $^ -o $@

$(BUILDDIR)%.4bpp: gfx/%.png
	$(SFC) tiles $(SFCSFLAGS) -i $^ -d $@

# palette
$(BUILDDIR)%pal.cdata: $(BUILDDIR)%pal
	cat $^ | xxd -i > $@

$(BUILDDIR)%.pal: gfx/%.png
	$(SFC) palette $(SFCFLAGS) -i $^ -d $@

$(BUILDDIR)%cgb.pal: gfx/%cgb.png
	$(SFC) palette $(SFCCFLAGS) -i $^ -d $@

$(BUILDDIR)%sgb.pal: gfx/%sgb.png
	$(SFC) palette $(SFCSFLAGS) -i $^ -d $@

# tilemap
$(BUILDDIR)%map.cdata: $(BUILDDIR)%map
	cat $^ | xxd -i > $@

$(BUILDDIR)%attr.cdata: $(BUILDDIR)%attr
	cat $^ | xxd -i > $@

$(BUILDDIR)%.map: gfx/%.png $(BUILDDIR)%.2bpp $(BUILDDIR)%.pal
	$(SFC) map -Mgb -F -i $< -t $(BUILDDIR)$*.2bpp -p $(BUILDDIR)$*.pal -d $@

$(BUILDDIR)%cgb.map $(BUILDDIR)%cgb.attr: gfx/%cgb.png $(BUILDDIR)%cgb.2bpp $(BUILDDIR)%cgb.pal
	$(SFC) map -Mgbc -i $< -t $(BUILDDIR)$*cgb.2bpp -p $(BUILDDIR)$*cgb.pal -d $(BUILDDIR)$*cgb.mapattr
	$(DINTLV) $(BUILDDIR)$*cgb.mapattr $(BUILDDIR)$*cgb.map $(BUILDDIR)$*cgb.attr

$(BUILDDIR)%sgb.map $(BUILDDIR)%sgb.attr: gfx/%sgb.png $(BUILDDIR)%sgb.4bpp $(BUILDDIR)%sgb.pal
	$(SFC) map -Msnes -i $< -t $(BUILDDIR)$*sgb.4bpp -p $(BUILDDIR)$*sgb.pal -d $@

# for hugedriver conversion
# rgb2sdas does not allow to specify an output file
$(BUILDDIR)%.obj.o: $(BUILDDIR)%.obj
	$(RGB2SDAS) $^

$(BUILDDIR)%.rel: $(BUILDDIR)%.obj.o
	cp $^ $@

$(BUILDDIR)hUGEDriver.obj: $(HDBIN)hUGEDriver.asm
	$(RGBAS) -o$@ -i$(HDBIN) $^

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