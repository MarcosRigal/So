EXEC     = demo1_pid_ppid_uid demo2_fork demo3_wait demo4_wait demo5_environ demo6_exit
CC       = LC_MESSAGES=C gcc

CFLAGS   = -Wall# -Wextra -Wpedantic -Wstrict-aliasing

all: $(EXEC)

$(EXEC): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean

clean:
	@rm -rf $(EXEC)
