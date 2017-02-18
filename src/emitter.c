#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

#define error(x) do { \
	fprintf(stderr, x ": (%d) %s\n", -errno, strerror(errno)); \
	goto finish; \
 } while (0);

static const sd_bus_vtable emitter_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_SIGNAL("Hello", "s", 0),
	SD_BUS_VTABLE_END
};

int usrhandler(sd_event_source *s, const struct signalfd_siginfo *si, void *userdata) {
	static int count = 0;
	sd_bus *bus = (sd_bus *)userdata;

	printf("emitting signal\n");
	sd_bus_emit_signal(bus, "/io/density/Emitter", "io.density.Emitter", "Hello", "s", "world");

	if (++count >= 3) {
		sd_event *event = sd_bus_get_event(bus);
		sd_event_exit(event, EXIT_SUCCESS);
	}

	return 0;
}

int main(int argc, char *argv[]) {
	sd_event *event = NULL;
	sd_event_source *sigsource = NULL;
	sd_bus *bus = NULL;
	sd_bus_slot *slot = NULL;
	sigset_t x;
	int r = 0;

	r = sd_event_default(&event);
	if (r < 0) error("Failed to create event loop");

	r = sd_bus_default(&bus);
	if (r < 0) error("Failed to aquire bus");

	r = sd_bus_add_object_vtable(
		bus,
		&slot,
		"/io/density/Emitter",
		"io.density.Emitter",
		emitter_vtable,
		NULL);
	if (r < 0) error("Failed to create object");

	r = sd_bus_request_name(bus, "io.density.Emitter", 0);
	if (r < 0) error("Failed to aquire name");

	r = sd_bus_attach_event(bus, event, 0);
	if (r < 0) error("Failed to attach event loop");

	sigemptyset(&x);
	sigaddset(&x, SIGUSR1);
	r = sigprocmask(SIG_BLOCK, &x, NULL);
	if (r < 0) error("Failed to mask signals");

	r = sd_event_add_signal(event, &sigsource, SIGUSR1, usrhandler, bus);
	if (r < 0) error("Failed to add signal handler");

	r = sd_event_loop(event);

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(bus);
	sd_bus_unref(bus);
	sd_event_source_unref(sigsource);
	sd_event_unref(event);
	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
