# Below here are the test make recipes
# Helpful site http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
CXX=gcc
CXXFLAGS:=-Wall -g

all: aws clnt sva svb

##############################################################
# User facing recipes
##############################################################
aws: AWS.o AWS-utils.o
	$(CXX) -o aws.o AWS.o AWS-utils.o $(CXXFLAGS);

clnt: client.o client-utils.o
	$(CXX) -o clnt.o client.o client-utils.o $(CXXFLAGS);

sva: ServerA.o ServerA-utils.o shortest-path-tree.o
	$(CXX) -o sva.o ServerA.o ServerA-utils.o \
	shortest-path-tree.o $(CXXFLAGS);

svb: ServerB.o ServerB-utils.o
	$(CXX) -o svb.o ServerB.o ServerB-utils.o $(CXXFLAGS);

##############################################################
# Recipes for files with main()
##############################################################
AWS.o: AWS.c
	$(CXX) -c AWS.c $(CXXFLAGS);

client.o: client.c
	$(CXX) -c client.c $(CXXFLAGS);

ServerA.o: ServerA.c
	$(CXX) -c ServerA.c $(CXXFLAGS);

ServerB.o: ServerB.c
	$(CXX) -c ServerB.c $(CXXFLAGS);

##############################################################
# Utility function recipes
##############################################################
AWS-utils.o: AWS-utils.c AWS-utils.h
	$(CXX) -c AWS-utils.c $(CXXFLAGS);

client-utils.o: client-utils.c client-utils.h
	$(CXX) -c client-utils.c $(CXXFLAGS);

ServerA-utils.o: ServerA-utils.c ServerA-utils.h
	$(CXX) -c ServerA-utils.c $(CXXFLAGS);

ServerB-utils.o: ServerB-utils.c ServerB-utils.h
	$(CXX) -c ServerB-utils.c $(CXXFLAGS);

shortest-path-tree.o: shortest-path-tree.c shortest-path-tree.h
	$(CXX) -c shortest-path-tree.c $(CXXFLAGS);

##############################################################
# Auxiliary Recipes
##############################################################
.PHONY: clean

clean:
	rm -r *.o* *~ *.exe*
