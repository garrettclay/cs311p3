#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <pipe.h>

int **pipe_generator(int pipe_cnt){
	int **pipe_array = (int **) malloc(sizeof(int *) * (pipe_cnt));
	int i;
	for(i=0; i<pipe_cnt; i++){
		pipe_array[i] = (int *) malloc(sizeof(int) * 2);
	}
	return pipe_array;
}

void suppressor(int *array_pipe){



}

int main(int argc, char **argv){

	char buf[50] = "why does FPUT work?\n";
	char buf2[50] = "this prints fine.\n";
	char buf3[50];
	int result;
	//int prs_fds[2];
	int status;
	int j = 0;
	pid_t child;


	// pass argc in here eventually
	int **prs_fds = pipe_generator(1);


//	pipe_array[1][0] = 5;
	while(j < 1){
		if(pipe(prs_fds[j]) != 0){
			perror("pipe");
			exit(-1);
		}
	
		switch((result = fork())){
			case -1:
				//in parent
				perror("fork");
				exit(-1);
				break;

			case 0:
				//child case
				//read from prs_fds
				//write to sup_fds 
				if(close(prs_fds[j][1] == -1)){	//write pipe end
					perror("close 1");
				}
			
				// duplicate stdout
				if(prs_fds[j][0] != STDIN_FILENO){
					if(dup2(prs_fds[j][0], STDIN_FILENO) == -1){
						perror("dup2 1");
					}
					if(close(prs_fds[j][0]) == -1){
						perror("close 2");
					}	
				}	
				FILE *input = fdopen(STDIN_FILENO, "r");

				int o = 0;
				while(o < 8){
					fgets(buf, 12, input);	
					printf("%s\n",buf);
					o++;
				}		
				//execlp("sort", "sort", (char *)NULL);
				//int num_read;
				//num_read = read(STDIN_FILENO, buf2, 100);
			//	printf("Start of child PID %d\n", getpid());
			//	fgets(buf3, 50, input);
			//	printf("\n%s\n", buf3);

				printf("End of child %d\n",getpid());
				//fgets(buf2, 5, input);
				fclose(input);	
				close(STDIN_FILENO);
				break;		
	
			default:
				//parent case -- result holds pid of child
				//close output end of pipe		
				if(close(prs_fds[j][0]) == -1){
					perror("close 3");
				}
				//bind stdout to file input
				if(prs_fds[j][1] != STDOUT_FILENO){
					if(dup2(prs_fds[j][1], STDOUT_FILENO) == -1){
						perror("dup2 2");
					}
					if(close(prs_fds[j][1]) == -1){
						perror("close 5");
					}
				}
				FILE *output = fdopen(STDOUT_FILENO, "w");

				char word_in[50];
				int scan_result = 1;
				while(scan_result != EOF){
					// To do: scanf without including chars but 
					// so you can stil read EOF
					// scanf("%[^!-@]", word_in);
					scan_result = scanf("%s", word_in);
					fputs(word_in, output);
					fputc('\n', output);
				}
			
				fclose(output);	
				close(STDOUT_FILENO);
				//child = wait(&status);
				//fputs(buf2, output);
				break; 
		}
		j++;
	}

	//if(close(prs_fds[1]) == -1){
	//	perror("close 4");
	//}
	
	child = wait(&status);
	printf("Current PID: %d\n",getpid());
	return 0;
}
