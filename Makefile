APPNAME := blocks-and-bombs
FILES := main except options util bbengine menustate playstate helpstate highscorestate \
	textwriter resources effects
SOURCES = $(foreach f,$(FILES),$(SRCDIR)/$(f).cc)
OBJECTS = $(foreach f,$(FILES),$(SRCDIR)/$(f).o)
SRCDIR := .

CC := g++
SDLCXXFLAGS := $(shell sdl-config --cflags)
CXXFLAGS := -I. -O0 -ggdb -Wall -Wextra -Wabi -Weffc++ -Wctor-dtor-privacy
CXXFLAGS += $(SDLCXXFLAGS)

SDLLDFLAGS := $(shell sdl-config --libs) -lSDL_ttf -lSDL_image
LDFLAGS := -L.
LDFLAGS += $(SDLLDFLAGS)

MAKEDEPEND = g++ -M $(CXXFLAGS) -o $*.d $<


all: blocks-and-bombs


blocks-and-bombs: $(OBJECTS) Makefile
	$(CC) $(LDFLAGS) -o $(APPNAME) $(OBJECTS)

clean:
	rm -f $(APPNAME)
	rm -f $(OBJECTS)
	rm -f $(SOURCES:.cc=.P)
	rm -f $(SOURCES:.cc=.d)
	find . -type f -name '*~' -print0 | xargs -0 -i rm -f {}
	rm -f TAGS

TAGS:
	rm -f TAGS
	touch TAGS
	find . -type f -print0 -o -iname '*.cc' -o -iname '*.hh' | \
	xargs -0 -i etags.emacs --declarations --ignore-indentation -a -o TAGS {}

.PHONY: all blocks-and-bombs clean TAGS

%.o: %.cc Makefile
	@$(MAKEDEPEND); \
	cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
	$(CC) $(CXXFLAGS) -o $@ -c $<

-include $(SRCDIR)/$(SOURCES:.cc=.P)
