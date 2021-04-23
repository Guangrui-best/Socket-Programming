all: 
	g++ -o scheduler scheduler.cpp
	g++ -o hospitalA hospitalA.cpp
	g++ -o hospitalB hospitalB.cpp
	g++ -o hospitalC hospitalC.cpp
	g++ -o client client.cpp

clean:
	$(RM) scheduler
	$(RM) hospitalA
	$(RM) hospitalB
	$(RM) hospitalC
	$(RM) client