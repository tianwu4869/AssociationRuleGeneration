all: hcrminer

hcrminer: hcrminer.o
	g++ hcrminer.o -o hcrminer

hcrminer.o: hcrminer.cpp
	g++ -c hcrminer.cpp

clean:
	rm -f *o hcrminer