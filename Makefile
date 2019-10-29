CC ?= cc
# on linux invoke as:
# make CFLAGS="-std=c99 -D_GNU_SOURCE"
# or uncomment the following line:
CFLAGS=-std=c99 -D_GNU_SOURCE

BIN=bin
TEST_SRC=test_src

TEST_LIB_C=$(TEST_SRC)/test_lib.c
TEST_LIB_SRC=$(TEST_SRC)/test_lib.c $(TEST_SRC)/test_lib.h
TEST_LIB=$(BIN)/test_lib.o

STRTEST_LIB_C=$(TEST_SRC)/strtest_lib.c
STRTEST_LIB_SRC=$(TEST_SRC)/strtest_lib.c $(TEST_SRC)/strtest_lib.h
STRTEST_LIB=$(BIN)/strtest_lib.o

INTEGER_C=integer.c
INTEGER_SRC=$(INTEGER_C) integer.h
INTEGER_LIB=$(BIN)/integer.o
INTEGER_APP=$(BIN)/integer_app
TEST_INTEGER=$(BIN)/test_integer

OBJ_MAP_C=obj_map.c
OBJ_MAP_SRC=$(OBJ_MAP_C) obj_map.h
OBJ_MAP_LIB=$(BIN)/obj_map.o
OBJ_MAP_APP=$(BIN)/obj_map_app
TEST_OBJ_MAP=$(BIN)/test_obj_map

OBJ_STORE_C=obj_store.c
OBJ_STORE_SRC=$(OBJ_STORE_C) obj_store.h
OBJ_STORE_LIB=$(BIN)/obj_store.o
TEST_OBJ_STORE=$(BIN)/test_obj_store

STRING_C=string_o.c
STRING_SRC=$(STRING_C) string_o.h
STRING_LIB=$(BIN)/string_o.o
STRING_APP=$(BIN)/string_app
TEST_STRING=$(BIN)/test_string

INT_LIBS=$(INTEGER_LIB) $(OBJ_MAP_LIB) $(OBJ_STORE_LIB) $(TEST_LIB)
OBM_LIBS=$(OBJ_MAP_LIB)
OBS_LIBS=$(INT_LIBS) $(STRING_LIB) $(STRTEST_LIB)
STR_LIBS=$(STRING_LIB) $(OBJ_MAP_LIB) $(OBJ_STORE_LIB) $(TEST_LIB) $(STRTEST_LIB)

all: obj_map integer string obj_store
.PHONY: all

integer: $(TEST_INTEGER) $(INTEGER_APP)
.PHONY: integer

obj_map: $(TEST_OBJ_MAP) $(OBJ_MAP_APP)
.PHONY: obj_map

string: $(TEST_STRING) $(STRING_APP)
.PHONY: string

obj_store: $(TEST_OBJ_STORE)
.PHONY: obj_store

clean:
	-rm -rf $(BIN)
.PHONY: clean

clean_all: clean_core clean_ostore clean 
.PHONY: clean_all

$(BIN)/integer_app: $(TEST_SRC)/integer_app.c $(INT_LIBS)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(INT_LIBS) -o $@
   
$(BIN)/test_integer: $(TEST_SRC)/test_integer.c $(INT_LIBS)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(INT_LIBS) -o $@
    
$(BIN)/obj_map_app: $(TEST_SRC)/obj_map_app.c $(OBM_LIBS)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(OBM_LIBS) -o $@
   
$(BIN)/test_obj_map: $(TEST_SRC)/test_obj_map.c $(OBM_LIBS) $(TEST_LIB) $(OBJ_STORE_LIB)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(OBM_LIBS) $(TEST_LIB) $(OBJ_STORE_LIB) -o $@
    
$(BIN)/string_app: $(TEST_SRC)/string_app.c $(STR_LIBS) 
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(STR_LIBS) -o $@
   
$(BIN)/test_string: $(TEST_SRC)/test_string.c $(STR_LIBS)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(STR_LIBS) -o $@

$(BIN)/test_obj_store: $(TEST_SRC)/test_obj_store.c $(OBS_LIBS)
	$(CC) -Wall $(CFLAGS) $(TEST_SRC)/$(@F).c $(OBS_LIBS) -o $@

$(INTEGER_LIB): $(INTEGER_SRC) $(BIN) 
	$(CC) -Wall $(CFLAGS) -c $(INTEGER_C) -o $@
	
$(OBJ_MAP_LIB): $(OBJ_MAP_SRC) $(BIN) 
	$(CC) -Wall $(CFLAGS) -c $(OBJ_MAP_C) -o $@

$(STRING_LIB): $(STRING_SRC) $(BIN)
	$(CC) -Wall $(CFLAGS) -c $(STRING_C) -o $@

$(OBJ_STORE_LIB): $(OBJ_STORE_SRC) $(BIN)
	$(CC) -Wall $(CFLAGS) -c $(OBJ_STORE_C) -o $@
	
$(TEST_LIB): $(TEST_LIB_SRC) $(BIN)
	$(CC) -Wall $(CFLAGS) -c $(TEST_LIB_C) -o $@

$(STRTEST_LIB): $(STRTEST_LIB_SRC) $(BIN)
	$(CC) -Wall $(CFLAGS) -c $(STRTEST_LIB_C) -o $@

$(BIN):
	test -d $(BIN) || mkdir $(BIN)


clean_integer:
	-rm -f $(INTEGER_LIB)
	-rm -f $(TEST_INTEGER)
	-rm -f $(INTEGER_APP)
.PHONY: clean_integer

clean_obj_map:
	-rm -f $(OBJ_MAP_LIB)
	-rm -f $(TEST_OBJ_MAP)
	-rm -f $(OBJ_MAP_APP)
.PHONY: clean_obj_map

clean_string:
	-rm -f $(STRING_LIB)
	-rm -f $(TEST_STRING)
	-rm -f $(STRING_APP)
.PHONY: clean_string

clean_obj_store:
	-rm -f $(OBJ_STORE_LIB)
	-rm -f $(TEST_OBJ_STORE)
.PHONY: clean_obj_store

clean_libs:
	-rm -f $(OBJ_MAP_LIB)
	-rm -f $(OBJ_STORE_LIB)
	-rm -f $(INTEGER_LIB)
	-rm -f $(STRING_LIB)
	-rm -f $(TEST_LIB)
	-rm -f $(STRTEST_LIB)
.PHONY: clean_libs

clean_ostore:
	-rm -rf ostore/*
	-rm -rf ostore
.PHONY: clean_ostore
	
clean_core:
	-rm -f ./core.*
	-rm -f $(BIN)/core.*
.PHONY: clean_core
    
