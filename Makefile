targets:=interpreter

interpreter_sources=main.c
interpreter_sources+=helper/getch.c
interpreter_sources+=helper/putch.c
interpreter_sources+=src/lineedit.c
interpreter_sources+=src/interpreter.c
interpreter_sources+=commands/help.c
interpreter_sources+=commands/hidden.c
interpreter_sources+=commands/parameter.c
interpreter_sources+=commands/version.c
interpreter_sources+=src/is_whitespace.c
interpreter_sources+=src/commands.c

CFLAGS = -Wall -g
CC = gcc
INCLUDE =
LIBS = -lm

CFLAGS += -DLINUX_GETCH=1
CFLAGS += -DLINUX_PUTCH=1
CFLAGS += -DLINUX_PUTSTRING=1

INCLUDES+=-Iinclude

sourcepath:=src
temppath:=.temp
outpath:=$(temppath)/objects
dependpath:=$(temppath)/depend

objs:=$(foreach tgt,$(targets),\
	$(eval $(tgt)_objs:=$(patsubst %,$(outpath)/%.o,$($(tgt)_sources)))\
	$($(tgt)_objs)\
)

all: $(targets)

ifneq ("$(wildcard $(dependpath))","")
$(eval depends:=$(shell find $(dependpath) -name *.d))
endif
include $(depends)

$(targets):
	@echo "$@: $(filter %.o,$^)" > $(patsubst %,$(dependpath)/%.d,$@)
	$(CC) -o $@ $(filter %.o,$^)

%.c.o:
	$(CC) -o $@ -c -MT $@ -MMD -MP -MF $(patsubst $(outpath)/%.o,$(dependpath)/%.d,$@) $(CFLAGS) $(INCLUDES) $(filter %.c,$^)

%/:
	@mkdir -p $@

clean:
	@rm -f $(objs)
	@rm -f $(targets)

mrproper: clean
	@rm -rf $(temppath)

xx:
	@echo depends: $(depends)

.SECONDEXPANSION:
$(targets): | $$(dir $$@)
$(targets): | $$(dir $$(patsubst %,$(dependpath)/%,$$@))
$(targets): $$($$@_objs)

$(objs): | $$(dir $$@)
$(objs): | $$(dir $$(patsubst $(outpath)/%.o,$(dependpath)/%.d,$$@))
$(objs): $$(patsubst $(outpath)/%.o,%,$$@)
