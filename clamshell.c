// vim: et sw=4 ts=4

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LID_OPENED 0
#define LID_CLOSED 1

void usage(void);
uint8_t clamshell_state(void);
void debug(const char * restrict format, ...);

static int debug_flag = 0;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        usage();
        exit(0);
    }

    // Enable debug if DEBUG environment variable is set
    if (getenv("DEBUG") != NULL) {
        debug_flag = 1;
    }

    // Main loop check clamshell lid every 10 seconds
    pid_t child_pid = 0;
    uint8_t lid_state0 = LID_CLOSED;
    uint8_t lid_state1 = LID_CLOSED;
    while (true) {
        debug("Querying lid status\n");
        lid_state1 = lid_state0;
        lid_state0 = clamshell_state();
        debug("lid_state0 %s lid_state1 %s\n",
                lid_state0 == LID_OPENED ? "LID_OPENED" : "LID_CLOSED",
                lid_state1 == LID_OPENED ? "LID_OPENED" : "LID_CLOSED");

        if (lid_state0 == LID_OPENED && lid_state1 == LID_CLOSED) {
            debug("Lid transitioned to open\n");
            child_pid = fork();
            if (child_pid == -1) {
                fprintf(stderr, "Error: failed to fork().\n");
                exit(1);
            }
            else if (child_pid == 0) {
                debug("Executing command\n");
                execvp(argv[1], &argv[1]);
            }
        }
        else if (lid_state0 == LID_CLOSED && lid_state1 == LID_OPENED) {
            debug("Lid transitioned to closed\n");
            if (child_pid != 0) {
                debug("Killing command %i\n", child_pid);
                kill(child_pid, SIGTERM);
            }
        }

        // Reap child processes that exit before we have a chance to kill them
        int child_status;
        if (child_pid != 0 && waitpid(child_pid, &child_status, WNOHANG) != 0) {
            debug("Child exited %i\n", WEXITSTATUS(child_status));
            child_pid = 0;
        }

        // Wait before repeating the loop
        debug("Sleeping 10 seconds\n");
        sleep(10);
    }

    return 0;
}

uint8_t clamshell_state(void) {
    uint8_t clamshell_state = 0;
    io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,  IOServiceMatching("IOPMrootDomain"));
    CFTypeRef clamshell = IORegistryEntryCreateCFProperty(service,  CFSTR("AppleClamshellState"),  kCFAllocatorDefault,  0);

    if (clamshell != nil && CFBooleanGetTypeID() == CFGetTypeID(clamshell)) {
        clamshell_state = CFBooleanGetValue(clamshell);
    }

    CFRelease(clamshell);
    IOObjectRelease(service);

    return clamshell_state;
}

void usage(void) {
    printf("Usage:\n");
    printf("  clamshell <command>\n");
    printf("\n");
    printf("Executes the command the clamshell lid is opened. When the lid is closed the process is killed.\n");
    printf("Checking for changes between opened and closed is done every 10 seconds.\n");
    printf("Set the DEBUG variable to any value to enable debug messages\n");
    printf("\n");
}

void debug(const char * restrict format, ...) {
    va_list ap;
    if (debug_flag) {
        va_start(ap, format);
        printf("%i ", getpid());
        vprintf(format, ap);
        va_end(ap);
    }
}
