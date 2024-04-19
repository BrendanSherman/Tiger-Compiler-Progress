CC = gcc
FLAGS = -Wall -Werror -std=c99 -D_XOPEN_SOURCE=700 -g

parse: parse.o print_ir.o prabsyn.o semant.o translate.o frame.o env.o types.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o
	$(CC) $(FLAGS) $^ -o $@

TARGET = parse
${TARGET}.o: ${TARGET}.c y.tab.h
	$(CC) $(FLAGS) -c $<

TARGET = print_ir
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = prabsyn
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = semant
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = translate
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = frame
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = env
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = types
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = absyn
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = lex.yy
${TARGET}.o: ${TARGET}.c
	$(CC) $(FLAGS) -c $<

${TARGET}.c: tiger.lex
	lex $<

TARGET = y.tab
${TARGET}.o: ${TARGET}.c
	$(CC) $(FLAGS) -c $<

${TARGET}.h: ${TARGET}.c

${TARGET}.c: tiger.grm
	bison -dv $< -o $@

TARGET = symbol
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

TARGET = table
${TARGET}.o: ${TARGET}.c ${TARGET}.h $(COMMON_HEADERS)
	$(CC) $(FLAGS) -c $<

TARGET = error
${TARGET}msg.o: ${TARGET}msg.c ${TARGET}msg.h
	$(CC) $(FLAGS) -c $<

TARGET = util
${TARGET}.o: ${TARGET}.c ${TARGET}.h
	$(CC) $(FLAGS) -c $<

clean: 
	rm -f parse *.o lex.yy.c y.tab.* y.output

