CC = gcc
CFLAGS = -Wall -O3

LIBS = -lm -lgsl -lgslcblas
EXEC = reconstruction
TEST = test
OBJS = utils.o recon.o
INCS = utils.h recon.h

all: $(EXEC) $(TEST)

debug: CFLAGS += --DDEBUG -g
debug: all

profile: PROFILE += -pg
profile: $(EXEC)

$(EXEC): main.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(PROFILE)

$(TEST): unittests.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(INCS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET) *.~ *.mtx *.dSYM
