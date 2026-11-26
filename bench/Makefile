CC      ?= cc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra
BINS     = bitsort rotate layout prefix bits
# The sentinel trick is measured at four optimization levels, because the
# answer depends on the build. That is the finding, so it ships.
SENT     = sentinel_O0 sentinel_O1 sentinel_O2 sentinel_O3

.PHONY: all bench clean

all: $(BINS) $(SENT)

$(BINS): %: %.c bench.h
	$(CC) $(CFLAGS) -o $@ $<

sentinel_O0: sentinel.c bench.h
	$(CC) -O0 -std=c11 -Wall -Wextra -DOPT_LABEL='"-O0"' -o $@ $<
sentinel_O1: sentinel.c bench.h
	$(CC) -O1 -std=c11 -Wall -Wextra -DOPT_LABEL='"-O1"' -o $@ $<
sentinel_O2: sentinel.c bench.h
	$(CC) -O2 -std=c11 -Wall -Wextra -DOPT_LABEL='"-O2"' -o $@ $<
sentinel_O3: sentinel.c bench.h
	$(CC) -O3 -std=c11 -Wall -Wextra -DOPT_LABEL='"-O3"' -o $@ $<

# Every program prints its own before/after line and verifies both agree.
bench: all
	@echo "greybeard benchmarks"
	@echo "  $(CC) $(CFLAGS)  |  $(shell uname -srm)"
	@echo "  naive = the obvious version, tuned = same answer, cheaper"
	@echo
	@./bitsort
	@echo
	@./rotate
	@echo
	@./sentinel_O0
	@./sentinel_O1
	@./sentinel_O2
	@./sentinel_O3
	@echo
	@./layout
	@echo
	@./prefix
	@echo
	@./bits
	@echo
	@echo "  A ratio above 1x means the receipt is real. Run it yourself."

clean:
	rm -f $(BINS) $(SENT)
