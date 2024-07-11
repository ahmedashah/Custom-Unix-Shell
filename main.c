// homework 2 solution
#include <ctype.h>
#define  _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>


#define exit(N) {fflush(stdout); fflush(stderr); _exit(N); }

const int BSIZE = 1024;


// useful helper fn for wc

static void print_result(int bsize, int lsize, int wsize)
{
    printf("Num Bytes: %d\n", bsize);
    printf("Num Lines: %d\n", lsize);
    printf("Num Words: %d\n", wsize);
    fflush(stdout);
}


void wc()
{
    //TODO: See the PDF for the specification of wc
    int numBytes = 0 ; 
    int numLines = 0 ; 
    int numWords = 0 ; 
    int partOfWord = 0; 
    char c ; 
    char prevC ; 
    while(scanf("%c", &c) != EOF){
        numBytes++; 

        if (c == '\n'){
            numLines++; 
        } 

        if((c!= ' ') && (c != '\t') && (c != '\n')){
            if(!partOfWord){
                numWords++ ; 
                partOfWord = 1;  
            }
        } else{
            partOfWord = 0 ;
        }
    }
    print_result(numBytes, numLines, numWords) ; 
    
}
static void Trim(char **filename){

    char* startOfFile = *filename ;
    while(*startOfFile == ' '){
        *startOfFile++ = '\0' ;
    }
    *filename = startOfFile ; 

    char * endOfFile = *filename  ;
    while (*endOfFile != '\0') {
        endOfFile++;
    }
    while (endOfFile > *filename && (*(endOfFile - 1) == ' ' || *(endOfFile - 1) == '\n')) {
        *(--endOfFile) = '\0';
    }
 
}

void command(char *c)
{

    char *sep = NULL;
    // resubmit : did not know we had to trim the filenames
    if((sep=strstr(c, "2>"))) {
        *sep = 0;
        char *fileout = sep + 2;
        int fileOutDescript = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0644) ;
        dup2(fileOutDescript, STDERR_FILENO) ; 
        command(c) ; 
        //exit(1); 
        
    
        // TODO: PDF -> stderr redirect
        
    }

    if((sep=strstr(c,">"))) {
        *sep=0;
        char *fileout = sep + 1;
        Trim(&fileout) ;
        int fileOutDescript = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0644) ;
        // TODO: PDF -> stdout redirect
        dup2(fileOutDescript, STDOUT_FILENO); 
        command(c) ; // resubmit did not know we call command 
        //exit(1) ;
    }

    if((sep=strstr(c,"<"))) {
        *sep=0;
        char *filein = sep + 1;
        Trim(&filein); 
        int  fileInDescript = open(filein, O_RDONLY, 0); 
        dup2(fileInDescript, STDIN_FILENO); 
        //command(c) ; 
        //exit(1); 
        // TODO: PDF -> stdin redirect
    }

    // TODO: PDF -> Command Parse

    char *cmd = strtok(c," \n");

     if (cmd == NULL){
        //exit(1);
        return;
    }
    
    char *args[16];
    args[0] = cmd;
    args[1] = strtok(NULL, " \n");
    args[2] = strtok(NULL, " \n");
    args[3] = strtok(NULL, " \n");
    args[4] = strtok(NULL, " \n");
    args[5] = strtok(NULL, " \n");
    args[6] = strtok(NULL, " \n");
    args[7] = strtok(NULL, " \n");
    args[8] = strtok(NULL, " \n");
    args[9] = strtok(NULL, " \n");
    args[10] = strtok(NULL, " \n");
    args[11] = strtok(NULL, " \n");
    args[12] = strtok(NULL, " \n");
    args[13] = strtok(NULL, " \n");
    args[14] = strtok(NULL, " \n");
    args[15] = NULL; 
    //what I had before
    

    /*
    char *args[16] ;
    args[0] = cmd;
    int i = 1 ; 
    char* argcur = NULL; 
    while ( (i < 15) && (argcur = strtok(NULL," \n")) ){
        args[i++] = argcur ; 
    }
    args[i] = NULL; 
    */

   
    
    if (!strcmp("wc", cmd)) {
        wc();
        //exit(1);
    } else {
        
            //printf("%s\n", cmd);
            execv(cmd, args);

            char *paths = getenv("PATH");
            char* path =  strtok(paths, ":"); 
       
            while(path != NULL){
                char pathCmd[1024]; //resumbit: changed the size of buffer from 256 to 1024
                snprintf(pathCmd, sizeof(pathCmd), "%s/%s", path, cmd);
                execv(pathCmd, args);
               path = strtok( NULL, ":"); 
            }
              // TODO: PDF -> Executing Command
             fprintf(stderr,"cs361sh: command not found: %s\n",cmd);
             fflush(stderr);

            // If failed, will continue here
            //exit(1);           
        }

       
   // } else {
       // waitpid(cpid, NULL, 0);
    //}

    
}


void run(char*);


void pipeline(char* head, char* tail)
{
    // TODO: PDF -> pipeline
    int filedes[2];
    pipe(filedes);
    int pipe_in = filedes[0];
    int pipe_out = filedes[1];
    int ret = -1; // you cannot modify ret directly (only waitpid can modify it)
    pid_t cpid  = fork() ; 

    if(cpid == 0){
        dup2(pipe_out, STDOUT_FILENO) ;
        command(head) ; 
        exit(1) ; 
    }else{
        waitpid(cpid, NULL, 0); 
        close(pipe_out) ; 
    }
     cpid = fork(); 
     if (cpid == 0){
        dup2(pipe_in,STDIN_FILENO); 
        run(tail);
        exit(1); 
     }else{
        waitpid(cpid, NULL, 0) ;
        close(pipe_in); 

     }
     return ; 

}


void sequence(char *head, char *tail)
{
    // TODO: PDF -> sequence
    run(head) ; 
    run(tail) ; 
    fflush(stderr);
}


void run(char *line)
{
    char *sep = 0;

    if((sep=strstr(line,";"))) {
        *sep = 0;
        // resubmit: did not make it till here before
        sequence(line, sep+1) ; 

        // TODO: PDF -> sequence
    }
    else if((sep=strstr(line,"|"))) {
        *sep = 0;

        // TODO: PDF -> pipeline
        pipeline(line, sep+1);
    }
    else {
        // TODO: PDF -> Executing Command
        pid_t cpid = fork() ; 
        if (cpid == 0){
            command(line);
            exit(1) ;
        }  else {
             waitpid(cpid, NULL, 0);
        }   
    }
}

// reads a line in to buf, one char at a time.
// returns the number of bytes read

int read_line(int fd, char *buf, int max)
{
    if (max <= 0)
        return 0;

    int i = 0;
    while (i < max - 1) {
        char c;
        size_t amt = read(fd, &c, 1);
        if (amt != 1)
            break;
        buf[i++] = c;
        if (c == '\n')
            break;
    }
    buf[i] = '\0';

    return i;
}

int main(int argc, char **argv)
{
    printf("cs361sh> ");
    fflush(stdout);

    // handy copies of original file descriptors

    int orig_in = dup(0);
    int orig_out = dup(1);
    int orig_err = dup(2);

    const int BSIZE = 1024;
    char line[BSIZE];

    while(read_line(STDIN_FILENO, line, BSIZE) > 0)  {
        //printf("cs361sh> ");
        run(line);

        // TODO: PDF -> pipeline, any section about redirects

        printf("cs361sh> ");
        fflush(stdout);
   }

    printf("\n");
    fflush(stdout);
    fflush(stderr);
    return 0;
}
