#include <stdio.h>
#include <stdlib.h>

#define Command_Mod 0
#define Insert_Mod 1
#define ON 1
#define OFF 0
#define NEW_LINE 10

typedef struct stack {
    char character;
    struct stack* next;
}*STACK;

typedef struct list{
   char character;
   struct list* next;
}*LIST;

//verifica daca stiva e nula
int emptyStack(STACK stack) {
    return (stack == NULL);
}

//verifica daca lista e nula
int emptyList(LIST list) {
    return (list == NULL);
} 

//determina elementul din stiva
char top(STACK stack) {
	return stack->character;
}

//verifica daca caracterul din celula e cifra
int isNumber(STACK stack) {
    return (top(stack) >= '0' && top(stack) <= '9');
}

//initializeaza stiva
STACK initStack(char input_character) {
    STACK stack;
    stack = (STACK)malloc(sizeof(struct stack));
    stack->character = input_character;
    stack->next = NULL;
    return stack;
}

//adauga o celula in stiva
STACK push(STACK stack, char input_character) {
	STACK new_cell = initStack(input_character);
	new_cell->next = stack;
	stack = new_cell;
	return stack;
}

//scoate o celula din stiva
STACK pop(STACK stack) {
    if(!emptyStack(stack)) {
   	    STACK mem = stack;
	    stack = stack->next;
	    free(mem);
    }
    return stack;
}
//initializeaza lista
LIST initList(char input_character) {
    LIST list;
    list = (LIST)malloc(sizeof(struct list));
    list->character = input_character;
    list->next = NULL;
    return list;
}

//adauga o celula dupa o anumita pozitie din lista
LIST addCharacter(LIST position, char input_character) {
    LIST new_cell = initList(input_character);
    LIST mem = position->next;
    position->next = new_cell;
    new_cell->next = mem;
    return new_cell;
}

//executa comanda backspace la o anumita pozitie
LIST backspace(LIST list, LIST position) {
    LIST aux = list;
    while(aux->next != position) {
	    aux = aux->next;
    }
    LIST mem = aux->next;
    aux->next = aux->next->next;
    free(mem);		
    return aux;
}

//executa comanda delete la o anumita pozitie
LIST delete(LIST position) {
	LIST mem = position->next;
	position->next = position->next->next;
	free(mem); 
    return position;
}

//determina linia pe care se afla cursorul
int determineLine( LIST list, LIST position ) {
    LIST aux = list;
    int line = 0;
    while(aux != position->next) {
	    if(aux->character == NEW_LINE) {
	        line++;
	    }
	    aux = aux->next;
    }
    return line;
}

//executa comanda go to character
LIST gotoCharacter(LIST list, LIST position, int column, int line) {
    LIST aux = list;
    if(line == 0) {
	    line = determineLine(list, position);
    }
    int count_line = 1;
    aux = list;
    while(count_line != line) {
	    if(aux->next->character == NEW_LINE) {
	        count_line++;
	    }
	    aux = aux->next;
    }
    int count_column = 1;
    while(count_column != column) {
	    count_column++;
	    aux = aux->next;
    }
    return aux;
}

//executa comanda delete line
LIST deleteLine(LIST list, LIST position, int line) {
    LIST aux = list;
    int column = 1;
    if(line == 0) {
	    line = determineLine( list, position );
    }
    int count_line = 1;
    aux = list;
    while(count_line != line) {
	    if(aux->next->character == NEW_LINE) {
	        count_line++;
	    }
	    aux = aux->next;
    }
    while(aux->next->character != NEW_LINE) {
	    aux = delete( aux );  
    }
    if(!emptyList(aux->next->next)) {
	    aux = delete(aux);
    }
    return gotoCharacter(list, position, column, line);
}
//executa comanda replace
LIST replace(LIST position, LIST string) {
    LIST aux_1 = position;
    LIST aux_2 = string;
    LIST mem = position;
    while(!emptyList(aux_2)) {   
	    while(aux_1->next->character != aux_2->character) {
	        aux_1 = aux_1->next;
	    }
	    mem = aux_1;
	    while( !emptyList(aux_2) && aux_1->next->character == aux_2->character ) {
	        aux_1 = aux_1->next;
	        aux_2 = aux_2->next;
	    }
	    if(!emptyList(aux_2)) {
	        aux_2 = string;
	        aux_1 = mem->next;
	    } 
    }
    while(!emptyList(string)) {
	    mem = delete(mem);
	    LIST tmp = string;
	    string = string->next;
	    free(tmp);
    }
    return mem;    
}

//salveaza in fisier textul curent
void saveFile(char* file_name, LIST list) {
    FILE* file;
    file = fopen(file_name, "w");
    LIST aux = list->next;	
    while(!emptyList(aux->next)) {
	    fprintf(file, "%c", aux->character);
	    aux = aux->next;
    }
    fclose(file);
}
//implementeaza comenzile de editare
void Implimentation(STACK stack, char* file) {
    LIST list, position;
    list = initList(NEW_LINE);
    position = addCharacter(list, NEW_LINE);
    position = list;	
    while(!emptyStack(stack)) {
	char s = ' ';
	while(top(stack)!= ':' || s != ':') {
	    s = top(stack);
	    stack = pop(stack);
	    if(top(stack) != ':' || s != ':') {
		    position = addCharacter(position, s);
	    }
	}
	stack = pop(stack); 
	stack = pop(stack); 
	stack = pop(stack); 
	while(top(stack) != ':' && top(stack) != 'q') {	
	    if(top(stack) == 's') {
		    saveFile(file, list); 
		    stack = pop(stack); 
		    stack = pop(stack);
	    }
	    else if(top(stack) == 'r') {
		    stack = pop(stack);
		    stack = pop(stack);
		    stack = pop(stack);
		    LIST tmp, position_tmp;
		    tmp = initList(top(stack));
		    stack = pop(stack);	    
		    position_tmp = tmp;
		    while(top(stack) != ' ') {
		        position_tmp = addCharacter(position_tmp, top(stack));
		        stack = pop(stack);
		    }
		    stack = pop(stack);
		    position = replace(position, tmp);
		    while(top(stack) != NEW_LINE) {
		        position = addCharacter(position, top(stack));
		        stack = pop(stack);
		    }
		    stack = pop(stack);
	    }
	    else if(top(stack) == 'b') {
		    position = backspace(list, position);
		    stack = pop(stack); 
		    stack = pop(stack);
	    }
	    else if(top(stack) == 'd') {
		    stack = pop( stack );
		    if(top(stack) == 'l') { 
		        stack = pop(stack); 
		        stack = pop(stack);
		        int line = 0;
		        while(isNumber(stack)) {
		            line = line*10 + top(stack) - '0';
		            stack = pop(stack);
		        }
		        if(line != 0) {
		            stack = pop(stack);
		        }
		        position = deleteLine(list, position, line);	    
	        }
		    else {
		        stack = pop(stack);
		        int number = 0;
		        while(isNumber(stack)) {
		            number = number*10 + top(stack) - '0';
		            stack = pop(stack);
		        }
		        if(number == 0) {
		            position = delete(position);
		        }
		        else{
		            int i;
		            for(i = 0; i < number; i++) {
			            position = delete(position);
		            }
		            stack = pop(stack);
		        }
		    }
	    }
	    else if(top(stack) == 'g') {
		    stack = pop(stack);
		    if(top(stack) == 'l') {
		        stack = pop(stack);
		        stack = pop(stack);
		        int column = 1;
		        int line = 0;
		        while(isNumber(stack)) {
			        line = line*10 + top(stack) - '0';
			        stack = pop( stack );
		        }
		        stack = pop(stack);
		        position = gotoCharacter(list, position, column, line);
	        }
	        else  {
		        stack = pop(stack);
		        stack = pop(stack);
		        int column = 0;
		        while(isNumber(stack)) {
			        column = column*10 + top(stack) - '0';
			        stack = pop(stack);
		        }
		        stack = pop(stack);
		        int line = 0;
		        while(isNumber(stack)) {
			        line = line*10 + top(stack) - '0';
			        stack = pop(stack);
		        }
		        if(line != 0) {
			        stack = pop( stack );
		        }
		        position = gotoCharacter(list, position, column, line);
	        }
	    }
	}
	if(top(stack) == 'q') {
	    break;
	}
	stack = pop(stack);
	stack = pop(stack);
	stack = pop(stack);
	stack = pop(stack);
    }
    while(!emptyList(list)) {
	    LIST tmp = list;
	    list = list->next;
	    free(tmp);
    }
    while(!emptyStack(stack)) {
	    STACK tmp = stack;
	    stack = stack->next;
	    free(tmp);
    }
}

//executa comenzile undo-redo
void undoRedo(STACK stack, char* file) {
    STACK new_stack;
    int command_state, save_state, undo = 0, undo_redo = 0;
    new_stack = NULL;
    command_state = Command_Mod;
    save_state = OFF;
    while(!emptyStack(stack)) {
	    if(top(stack) == 's' && command_state == Command_Mod && save_state == OFF) {
	        new_stack = push(new_stack, 'q');
	        new_stack = push(new_stack, NEW_LINE);
	        new_stack = push(new_stack, 's');
	        new_stack = push(new_stack, NEW_LINE);
	        stack = pop(stack);
	        stack = pop(stack);
	        save_state = ON;
	        undo = 0;
	        undo_redo = 0;
	    }
	    else if(top(stack) == 'k' && command_state == Command_Mod  && undo == 0) {
    	    stack = pop(stack); 
    	    while(top(stack) != NEW_LINE) {
    		    new_stack = push(new_stack, top(stack));    
    		    stack = pop(stack);
    		}
    		new_stack = push(new_stack, top(stack));  
    		stack = pop(stack);		
    	}
    	else if(top(stack) == 'r' && command_state == Command_Mod) {
    	    stack = pop(stack);
    	    stack = pop(stack);
    	    undo_redo++;
    	} 
    	else if(top(stack) == 'u' && command_state == Command_Mod) {
    		stack = pop(stack);
    		stack = pop(stack);
    	    if(undo_redo > 0) {
    		    undo_redo--;
    	    }
    	    else {
    		    undo++;
    	    }
    	}
    	else if(undo > 0) {
    	    if(top(stack) == 's') {
    		    new_stack = push(new_stack, top(stack));
    		    stack = pop(stack);
    		    new_stack = push(new_stack, top(stack));
    		    stack = pop(stack);
    	    }
    	    else if(top(stack) == 'k') {
    		    while(top(stack) != NEW_LINE) {
    		        stack = pop(stack);
    		    }
    		    stack = pop(stack);
    		    undo--;
    	    }
    	    else if(top(stack) == 'i') {
    		    stack = pop(stack);
    		    stack = pop(stack);
    		    stack = pop(stack);
    		    char s = 'a';
    		    while(top(stack) != ':' || s != ':') {
    		        s = top(stack); 
    		        stack = pop(stack);
    		        if(emptyStack(stack)) {
    			        new_stack = push(new_stack, 'i');
    			        new_stack = push(new_stack, ':');
    			        new_stack = push(new_stack, ':');
    			        break;
    		        }
    		    }
    		    stack = pop(stack);
    		    stack = pop(stack);
    		    undo--;
    	    }
    	    else if(top(stack) == 'b') {
    		    stack = pop(stack); 
    		    stack = pop(stack);
    		    undo--;
    	    }
    	    else { 
    		    while(isNumber(stack) || top(stack) == ' ') {
    		        stack = pop(stack);
    		    }
    		    if(top(stack) == 'l' || top(stack) == 'c') {
    		        stack = pop(stack);
    		        stack = pop(stack);
    		        stack = pop(stack);
    		        undo --;
    		    } 
    	 	    else if(top(stack) == 'd'){
    		        stack = pop(stack);
    		        stack = pop(stack);
    		        undo --;
    		    }
    	    }
    	}
    	else {
    	    if(top(stack) == 'i') {
    		    stack = pop(stack);
    		    if(top(stack) == ':') {
    		        stack = pop( stack );
    		        if(top(stack) == ':') {
    			        if(command_state == Command_Mod) {
    			            command_state = Insert_Mod;
    			        }
    			        else {
    			            command_state = Command_Mod;
    			        }
    		        }
    		        stack = push(stack,':');
    		    }
    		    stack = push(stack,'i');
    	    }
    	    new_stack = push( new_stack, top(stack) );
    	    stack = pop( stack );
    	}	
    }
    Implimentation(new_stack, file);
}

//citeste caracterele si le include intr-o stiva
void scanText(char* file) {
    STACK stack;
    char new_text;
    int command_state, num_command=-1;
    stack = NULL;
    command_state = Insert_Mod;
    stack = push(stack, NEW_LINE);
    stack->next = NULL;
    while(!(top(stack) == 'q' && command_state == Command_Mod)) {
	    scanf("%c", &new_text);
	    if(new_text == NEW_LINE && command_state == Command_Mod) {
	        num_command++;
	    }
	    else if(new_text == 'e' && top(stack) == 'r' && command_state == Command_Mod) { 
	        while(new_text != NEW_LINE) {
		        stack = push(stack, new_text);
		        scanf("%c", &new_text);
	        }
	        stack = push(stack, 'k');
	    }
	    else if(new_text == 's' && command_state == Command_Mod) {
	        num_command = -1;
	    }
	    if(new_text == ':' && top(stack) == ':') {
	        if(command_state == Command_Mod ) {
		        command_state = Insert_Mod;
		        num_command--;
	        }
	        else {
		    command_state = Command_Mod;
	        }
	    }
	    if(num_command == 5) {
	        stack = push(stack, NEW_LINE);
	        stack = push(stack, 's'); 
	        num_command = 0;
	    }
        if (top(stack) == NEW_LINE && emptyStack(stack->next)) {
	        stack = pop(stack);
	    }
	    stack = push(stack, new_text);
    }
    undoRedo(stack, file);
}
 
int main(int argc, char **argv) {
    scanText(argv[1]);
    return 0;
}
