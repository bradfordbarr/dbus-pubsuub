#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

#define error(x) do { \
	fprintf(stderr, x ": (%d) %s\n", -errno, strerror(errno)); \
	goto finish; \
 } while (0);

int sighandler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
	sd_event *event = (sd_event *)userdata;
	char *str = NULL;

	sd_bus_message_read(m, "s", &str);
	printf("got signal %s\n", str);
	sd_event_exit(event, EXIT_SUCCESS);

	return -1;
}

int main(int argc, char *argv[]) {
	sd_event *event = NULL;
	sd_bus *bus = NULL;
	int r = 0;

	r = sd_event_default(&event);
	if (r < 0) error("Failed to create event loop");

	r = sd_bus_default(&bus);
	if (r < 0) error("Failed to aquire bus");

	r = sd_bus_attach_event(bus, event, 0);
	if (r < 0) error("Failed to attach event loop");

	r = sd_bus_add_match(bus, NULL, "type='signal',member='Hello'", sighandler, event);
	if (r < 0) error("Failed to add match");

	r = sd_event_loop(event);

finish:
	sd_bus_unref(bus);
	sd_event_unref(event);
	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
