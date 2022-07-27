proc:proc.o
	g++ $^ -o $@ 

%.o:%.c
	g++ -c $< 

.PHONY:clean
clean:
	rm -rf *.o proc
