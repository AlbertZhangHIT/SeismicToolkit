
TARGET1 := gatherSplitor

TARGET2 := traceExtractor

TARGET3 := shotExtractor

TARGET4 := inlineExtractor

CUR_DIR := .

INCLUDE_DIR = $(CUR_DIR)/../include

CXX = g++

CXXFLAGS = -g -Wall -I$(INCLUDE_DIR)

SRCS_DIR = .

SHOTSPLIT_SRCS = gatherSplit.cpp $(SRCS_DIR)/vhead.cpp $(SRCS_DIR)/preReader.cpp \
				$(SRCS_DIR)/io.cpp
SHOTSPLIT_OBJS = $(SHOTSPLIT_SRCS:.cpp=.o)

TRACEEXT_SRCS = traceExtract.cpp $(SRCS_DIR)/vhead.cpp $(SRCS_DIR)/preReader.cpp \
				$(SRCS_DIR)/io.cpp
TRACEEXT_OBJS = $(TRACEEXT_SRCS:.cpp=.o)

SHOTEXT_SRCS = shotExtract.cpp $(SRCS_DIR)/vhead.cpp $(SRCS_DIR)/preReader.cpp \
				$(SRCS_DIR)/io.cpp
SHOTEXT_OBJS = $(SHOTEXT_SRCS:.cpp=.o) 


INLINEEXT_SRCS = inlineExtract.cpp $(SRCS_DIR)/vhead.cpp $(SRCS_DIR)/preReader.cpp \
				$(SRCS_DIR)/io.cpp 

INLINEEXT_OBJS = $(INLINEEXT_SRCS:.cpp=.o)  

BIN_DIR = ../bin

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET1): $(SHOTSPLIT_OBJS)
	$(CXX) $(CXXFLAGS) $(SHOTSPLIT_SRCS) -o $@ 
	@mv $@ $(BIN_DIR)

$(TARGET2): $(TRACEEXT_OBJS)
	$(CXX) $(CXXFLAGS) $(TRACEEXT_SRCS) -o $@ 
	@mv $@ $(BIN_DIR)

$(TARGET3): $(SHOTEXT_OBJS)
	$(CXX) $(CXXFLAGS) $(SHOTEXT_SRCS) -o $@ 
	@mv $@ $(BIN_DIR)

$(TARGET4): $(INLINEEXT_OBJS)
	$(CXX) $(CXXFLAGS) $(INLINEEXT_SRCS) -o $@ 
	@mv $@ $(BIN_DIR)

clean:
	rm -rf *~ *.d *.o 
	cd $(BIN_DIR); rm -rf $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) *.d 