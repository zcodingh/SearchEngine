all: bin/test	bin/ProduceDcit

bin/Configuration.o: src/Configuration.cc
	g++ -g -c src/Configuration.cc -o bin/Configuration.o

bin/DictProducer.o: src/DictProducer.cc
	g++ -g -c src/DictProducer.cc -o bin/DictProducer.o

bin/SplitTool.o: src/SplitTool.cc
	g++ -g -c src/SplitTool.cc -o bin/SplitTool.o

bin/ProduceDcit.o: src/ProduceDcit.cc
	g++ -g -c src/ProduceDcit.cc -o bin/ProduceDcit.o

bin/test.o: src/test.cc
	g++ -g -c src/test.cc -o bin/test.o

bin/ProduceDcit: bin/Configuration.o bin/DictProducer.o bin/SplitTool.o bin/ProduceDcit.o
	g++ -g bin/Configuration.o bin/DictProducer.o bin/SplitTool.o bin/ProduceDcit.o -o bin/ProduceDcit

bin/test: bin/test.o
	g++ -g bin/test.o -o bin/test

clean:
	rm -f bin/*.o bin/test bin/ProduceDcit