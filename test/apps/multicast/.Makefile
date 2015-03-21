SRC=../../../src

all: clean
	gcc -DDEBUG -o multicast \
		-I${SRC}/alg -I${SRC}/bug -I${SRC}/dat \
		-I${SRC}/io -I${SRC}/ipc -I${SRC}/net \
		-I${SRC}/sig -I${SRC}/sys -I${SRC}/thr \
		-lpthread \
		${SRC}/**/*.c *.c

debug: clean
	gcc -DDEBUG -g -o multicast-debug \
		-I${SRC}/alg -I${SRC}/bug -I${SRC}/dat \
		-I${SRC}/io -I${SRC}/ipc -I${SRC}/net \
		-I${SRC}/sig -I${SRC}/sys -I${SRC}/thr \
		-lpthread \
		${SRC}/**/*.c *.c

clean:
	-rm multicast
	-rm multicast-debug
