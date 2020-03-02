build: editor.c
	        gcc -Wall editor.c -o editor

run: build
	        ./editor 

clean: 
	        rm editor 

