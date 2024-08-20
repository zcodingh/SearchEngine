all: bin/ProduceDcit bin/server bin/client bin/PageLibPreprocessor

bin/Configuration.o: src/Configuration.cc
	g++ -g -c src/Configuration.cc -o bin/Configuration.o

bin/DictProducer.o: src/DictProducer.cc
	g++ -g -c src/DictProducer.cc -o bin/DictProducer.o

bin/SplitTool.o: src/SplitTool.cc
	g++ -g -c src/SplitTool.cc -o bin/SplitTool.o

bin/ProduceDcit.o: src/ProduceDcit.cc
	g++ -g -c src/ProduceDcit.cc -o bin/ProduceDcit.o

bin/ProduceDcit: bin/Configuration.o bin/DictProducer.o bin/SplitTool.o bin/ProduceDcit.o
	g++ -g bin/Configuration.o bin/DictProducer.o bin/SplitTool.o bin/ProduceDcit.o -o bin/ProduceDcit

bin/server.o: src/ServerStarter.cc
	g++ -g -c src/ServerStarter.cc -o bin/server.o

bin/Acceptor.o: src/Acceptor.cc
	g++ -g -c src/Acceptor.cc -o bin/Acceptor.o

bin/EventLoop.o: src/EventLoop.cc
	g++ -g -c src/EventLoop.cc -o bin/EventLoop.o

bin/InetAddress.o: src/InetAddress.cc
	g++ -g -c src/InetAddress.cc -o bin/InetAddress.o

bin/Socket.o: src/Socket.cc
	g++ -g -c src/Socket.cc -o bin/Socket.o

bin/SocketIO.o: src/SocketIO.cc
	g++ -g -c src/SocketIO.cc -o bin/SocketIO.o

bin/TaskQueue.o: src/TaskQueue.cc
	g++ -g -c src/TaskQueue.cc -o bin/TaskQueue.o

bin/TcpConnection.o: src/TcpConnection.cc
	g++ -g -c src/TcpConnection.cc -o bin/TcpConnection.o

bin/TcpServer.o: src/TcpServer.cc
	g++ -g -c src/TcpServer.cc -o bin/TcpServer.o

bin/ThreadPool.o: src/ThreadPool.cc
	g++ -g -c src/ThreadPool.cc -o bin/ThreadPool.o

bin/Dictionary.o: src/Dictionary.cc
	g++ -g -c src/Dictionary.cc -o bin/Dictionary.o

bin/KeyRecommander.o: src/KeyRecommander.cc
	g++ -g -c src/KeyRecommander.cc -o bin/KeyRecommander.o

bin/SearchEngineServer.o: src/SearchEngineServer.cc
	g++ -g -c src/SearchEngineServer.cc -o bin/SearchEngineServer.o

bin/server: bin/server.o bin/Acceptor.o bin/EventLoop.o bin/InetAddress.o bin/Socket.o bin/SocketIO.o bin/TaskQueue.o bin/TcpConnection.o bin/TcpServer.o bin/ThreadPool.o bin/Dictionary.o bin/KeyRecommander.o bin/SearchEngineServer.o bin/Configuration.o bin/SplitTool.o
	g++ -g bin/server.o bin/Acceptor.o bin/EventLoop.o bin/InetAddress.o bin/Socket.o bin/SocketIO.o bin/TaskQueue.o bin/TcpConnection.o bin/TcpServer.o bin/ThreadPool.o bin/Dictionary.o bin/KeyRecommander.o bin/SearchEngineServer.o bin/Configuration.o bin/SplitTool.o -o bin/server

bin/client.o: test/client.cc
	g++ -g -c test/client.cc -o bin/client.o

bin/client: bin/client.o bin/Configuration.o
	g++ -g bin/client.o bin/Configuration.o -o bin/client

bin/PageLibPreprocessor.o: src/PageLibPreprocessor.cc
	g++ -g -c src/PageLibPreprocessor.cc -o bin/PageLibPreprocessor.o

bin/WebPage.o: src/WebPage.cc
	g++ -g -c src/WebPage.cc -o bin/WebPage.o

bin/FileProcessor.o: src/FileProcessor.cc
	g++ -g -c src/FileProcessor.cc -o bin/FileProcessor.o

bin/PageLib.o: src/PageLib.cc
	g++ -g -c src/PageLib.cc -o bin/PageLib.o

bin/PageLibPreprocessor: bin/PageLibPreprocessor.o bin/WebPage.o bin/FileProcessor.o bin/PageLib.o bin/Configuration.o
	g++ -g bin/PageLibPreprocessor.o bin/WebPage.o bin/FileProcessor.o bin/PageLib.o bin/Configuration.o -ltinyxml2 -o bin/PageLibPreprocessor

clean:
	rm -f bin/*.o bin/ProduceDcit bin/server bin/PageLibPreprocessor