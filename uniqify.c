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

void suppressor(int **input_pipe){



}

void solder_pipes_shut(int **open_pipe, int num_pipes){
	int i;
	int j;
	for(i=0; i < num_pipes; i++){
		for(j=0; j < 2; j++){
			if(close(open_pipe[i][j]) == -1){
				//perror("recursive close");
			}
		}	
	}

}

int main(int argc, char **argv){

	char buf[50] = "why does FPUT work?\n";
	char buf2[50] = "this prints fine.\n";
	char buf3[50];
	int result;
	int status;
	int j = 0;
	int k = 0;
	pid_t child;
	int num_sort = argc;
	//delete following line for more than one process
	num_sort = 1;

	// pass argc in here eventually
	int **prs_fds = pipe_generator(1);
	int **spr_fds = pipe_generator(1);


//	pipe_array[1][0] = 5;
	while(j < 1){
		if(pipe(prs_fds[j]) != 0){
			perror("pipe");
			exit(-1);
		}
	//	if(pipe(spr_fds[k]) != 0){
	//		perror("pipe");
	//		exit(-1);
	//	}
	
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
			//	close(STDIN_FILENO);
			//	close(STDOUT_FILENO);
				if(close(prs_fds[j][1]) == -1){	//write pipe end
					perror("close 1");
				}
	//			if(close(spr_fds[k][0]) == -1){
	//				perror("close 7");
	//			}				


				// duplicate stdout
				if(prs_fds[j][0] != STDIN_FILENO){
					if(dup2(prs_fds[j][0], STDIN_FILENO) == -1){
						perror("dup2 1");
					}
					if(close(prs_fds[j][0]) == -1){
						perror("close 2");
					}	
				}	
	/*			if(spr_fds[k][1] != STDOUT_FILENO){
					if(dup2(spr_fds[k][1], STDOUT_FILENO) ==-1){
						perror("dup3 1");
					}
					if(close(spr_fds[k][1]) == -1){
						perror("close 6");
					}
				}
	*/

			//	FILE *input = fdopen(STDIN_FILENO, "r");

	/*			int o = 0;
				while(o < 8){
					fgets(buf, 12, input);	
					printf("%s\n",buf);
					o++;
				}		
	*/			
	//			close(spr_fds[k][1]);
	//
				solder_pipes_shut(prs_fds, 1);
				//close(prs_fds[j][0]);
				execlp("sort", "sort", (char *)NULL);
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
				//child = wait(&status);
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
