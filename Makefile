W := xdg-shell fullscreen-shell-unstable-v1
O := $(patsubst %.c,%.o,$(wildcard deps/bench/*.c)) \
	 $(patsubst %,%-client.c,$W)
P := $(shell pkg-config wayland-protocols --variable=pkgdatadir)
F := $(shell pkg-config wayland-client --cflags)
L := $(shell pkg-config wayland-client --libs)

run: $O
run: override CFLAGS += $F
run: override LDLIBS += $L

run.c: $(patsubst %,%-client.h,$W)

define wl-protocol
$2-client.h: $P/$1/$2.xml
	wayland-scanner client-header $$< $$@

$2-client.c: $P/$1/$2.xml
	wayland-scanner private-code $$< $$@
endef

$(eval $(call wl-protocol,stable/xdg-shell,xdg-shell))
$(eval $(call wl-protocol,unstable/fullscreen-shell,fullscreen-shell-unstable-v1))

clean:
	$(RM) run $O

check: run
	@./run

.PHONY: check
