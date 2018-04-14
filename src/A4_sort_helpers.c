#include "A4_sort_helpers.h"
#define BUF_SIZE 200
// Function: read_all() 
// Provided to read an entire file, line by line.
// No need to change this one.

/* NAME : FURAHA DAMIEN
    ID. : 260754407
*/
//----------------GLOBAL VARIABLES THAT I USE IN MY FUNCTIONS-----------------------------


int num_lines = 0;
sem_t *sema[27]; //declaring global array of semaphores
pid_t  pid;
// FILE *file;

//--------------END OF GLOBAL VARIABLES------------------------------------------------


char * sem = "sem_";

void read_all( char *filename ){
    
    FILE *fp = fopen( filename, "r" );
    int curr_line = 0;
    
    while( curr_line < MAX_NUMBER_LINES && 
           fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) )
    {
        curr_line++;
    }
    
    num_lines = curr_line;
    text_array[curr_line][0] = '\0';
    fclose(fp);
}

// Function: read_all() 
// Provided to read only the lines of a file staring with first_letter.
// No need to change this one.
void read_by_letter( char *filename, char first_letter ){

    FILE *fp = fopen( filename, "r" );
    int curr_line = 0;
    text_array[curr_line][0] = '\0';
    
    while( fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) ){
        if( text_array[curr_line][0] == first_letter ){
            curr_line++;
        }

        if( curr_line == MAX_NUMBER_LINES ){
            sprintf( buf, "ERROR: Attempted to read too many lines from file.\n" );
            write( 1, buf, strlen(buf) );
            break;
        }
    }
    num_lines = curr_line;
    text_array[curr_line][0] = '\0';
    fclose(fp);
}

// YOU COMPLETE THIS ENTIRE FUNCTION FOR Q1.

/* In this function, I use insertion sort to sort the content of the array alphabetically.
    I use the C function strcmp to compare which words occur firs alphatically.
    I also use a temporary array to hold the sorted word after which i copy the temp array
    to text_array using the C function strcpy.
*/

void sort_words( ){
    int i, j;

    char temp[MAX_LINE_LENGTH];

    for(i=1; i < num_lines; i++){
        strcpy(temp, text_array[i]);
        j = i - 1;

        while (j >= 0 && strcmp (text_array[j], temp) > 0) {
        strcpy(text_array[j+1], text_array[j]);
        j = j-1;

        }
        strcpy(text_array[j+1], temp);
    }
}

// YOU COMPLETE THIS ENTIRE FUNCTION FOR Q2.

/* Above i created an array of semaphores. In this function, i initialize the semaphores
    The semaphore for "a" is initialized with a value of 1 and the rest are given zero. I use 
    a loop to initialize all the semaphore. I make sure to unlink the previous semaphore before 
    open the next one to avoid confussion. All the semaphores have the name BASE_ (in small letters)
    followed by the letter of that semaphore  
*/

int initialize( ){
    
    sem_unlink("base_a");
    sema[0] = sem_open("base_a", O_CREAT, 0666, 1);

    for (int i = 1; i < 27; i++){

        char base[1000]  = "base_";
        char letter_to_add = ('a'+i);
        int len = strlen(base);
        base[len] = letter_to_add;
        base[len+1] = '\0';

         sem_unlink(base);  
        sema[i] = sem_open(base, O_CREAT, 0666, 0 );
           
    }
    FILE* file=fopen("temp.txt","w");
    fclose(file);
    
    return 0;
}

// YOU MUST COMPLETE THIS FUNCTION FOR Q2 and Q3.

/* This is the process where I control how the semaphore run and eventually sort the words.
    I also creeate a file that will hold my sorted words
    I call the sem_wait and sem_post functions depending on which semaphore has to run at a 
    specific instance. Everytime a specific semaphore is running, I call the read_by_letter and
    sort_word methods to sort the words that begin with a particular letter alphabetically.
    Then I write them to the file tha i created in the initialize method
*/

int process_by_letter( char* input_filename, char first_letter ){
   
    int i = first_letter - 'a';

    sem_wait(sema[i]);

    
    // For Q3, uncomment the following 2 lines and integrate them with your overall solution.
    read_by_letter( input_filename, first_letter );
    sort_words( );

    FILE *file=fopen("temp.txt","a");
        for(int i=0;i<MAX_NUMBER_LINES && text_array[i][0] != '\0';i++){
            fwrite(text_array[i], 1, strlen(text_array[i]), file);
        
            fflush(file);
        }

    fclose(file);

    sem_post(sema[i +1]);

    return 0;
}

// YOU COMPLETE THIS ENTIRE FUNCTION FOR Q2 and Q3.

/* Here allow the call wait on semaphore 27 which is my main semaphore and allow it to post. 
    I now read the content of the file that i created. 
    I use fgets to put the content of the file int the char array str.  
*/

int finalize( ){
    sem_wait(sema[26]);

    FILE *new_file = fopen("temp.txt","r");
    char str[MAX_LINE_LENGTH];

    
    while(!feof(new_file)){

        write(1, str, strlen(str));
        fflush(stdout);
        fgets(str,MAX_LINE_LENGTH,new_file);
    }
    fclose(new_file);

    sprintf( buf, "Sorting complete!\n" );
    write( 1, buf, strlen(buf) );


    return 0;
}

