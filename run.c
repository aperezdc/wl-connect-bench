/*
 * run.c
 * Copyright (C) 2021 Adrian Perez de Castro <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <wayland-client.h>
#include <stdio.h>
#include <string.h>
#include "deps/bench/bench.h"
#include "fullscreen-shell-unstable-v1-client.h"
#include "xdg-shell-client.h"

static void
connect_only(void)
{
    BENCHMARK(connect, 1);

    struct wl_display *display = wl_display_connect(NULL);
    if (!display)
        abort();

    wl_display_disconnect(display);

    END_BENCHMARK(connect);
    BENCHMARK_SUMMARY(connect);
}

struct registry_data {
    struct wl_shell *wlshell;
    struct zwp_fullscreen_shell_v1 *fsshell;
    struct xdg_wm_base *xdgwmshell;
};

static void
on_registry_global(void *data, struct wl_registry *registry, uint32_t name,
                   const char *interface, uint32_t version)
{
    struct registry_data *r = data;

    if (strcmp(interface, wl_shell_interface.name) == 0)
        r->wlshell = wl_registry_bind(registry, name, &wl_shell_interface, version);
    else if (strcmp(interface, zwp_fullscreen_shell_v1_interface.name) == 0)
        r->fsshell = wl_registry_bind(registry, name, &zwp_fullscreen_shell_v1_interface, version);
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
        r->xdgwmshell = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
}

static void
connect_and_roundtrip(void)
{
    BENCHMARK(connect_and_roundtrip, 1);

    struct wl_display *display = wl_display_connect(NULL);
    if (!display)
        abort();

    struct wl_registry *registry = wl_display_get_registry(display);
    struct registry_data registry_data;
    wl_registry_add_listener(registry,
                             &((struct wl_registry_listener) {
                               .global = on_registry_global,
                               }),
                             &registry_data);
    wl_display_roundtrip(display);

    wl_display_disconnect(display);

    END_BENCHMARK(connect_and_roundtrip);
    BENCHMARK_SUMMARY(connect_and_roundtrip);
}

int
main(int argc, char **argv)
{
    connect_only();
    connect_and_roundtrip();
    return EXIT_SUCCESS;
}
