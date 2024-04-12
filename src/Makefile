 
CFLAGS = -Wall -Wextra -Werror -std=c11
LIBS = -lcheck -lpthread -lm
SOURCES = $(wildcard s21_*.c)
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = a.out
TARGET = 3D_Viewer

ifeq ($(shell uname -s), Linux)
    LIBS = -lm -lpthread -lcheck -lsubunit
endif

all: install

$(EXECUTABLE): $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)
	clear
	./$(EXECUTABLE)
	@echo "\n\n"

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.out *.o gcov_report *.a *.gcda *.gcno test test.info ../dist ../build ./3D_Viewer/.qmake.stash ./build-3D_Viewer-Desktop_x86_darwin_generic_mach_o_64bit-Debug/ 

test: clean
	gcc $(CFLAGS) $(SOURCES) test.c -c
	gcc $(CFLAGS) *.o $(LIBS) -o test.out

# -------- Тест покрытия -------- #
gcov_report: clean add_coverage test
	./test.out
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info
	open report/index.html


checks:
	clang-format -style="{BasedOnStyle: Google}" -i *.c
	clang-format -style="{BasedOnStyle: Google}" -i *.h
	cppcheck enable=all missingIncludeSystem *.c

# -------- Создание документации -------- #
dvi:
	doxygen dvi_config
	open ./html/index.html

# -------------- Установка -------------- #
install:
	clear
	echo "Installation of S21_3D_Viewer_v1.0:"
	mkdir -p ../build
	cd ./3D_Viewer && qmake && make && make clean && rm -rf Makefile && mv 3D_Viewer.app ../../build/$(TARGET).app
	clear
	echo "Installation completed! You can find app in the ../build/ folder!"

uninstall:
	@rm -rf ../build
	@echo "Uninstall completed!"

# -------------- Архиваация -------------- #
dist: install
	mkdir -p ../dist
	mkdir -p ../dist/src
	cp -R ../build/$(TARGET).app ../dist
	cp -R . ../dist/src
	cp -R ./3D_Viewer ../dist/src
	cp -R ./GL ../dist/src
	cd ../dist && tar cvzf $(TARGET).tgz *
	cd ../dist && rm -rf $(TARGET).app
	rm -rf ../dist/src
	echo "Archive creation completed successfully!"

add_coverage:                                             
	$(eval CFLAGS += --coverage)
