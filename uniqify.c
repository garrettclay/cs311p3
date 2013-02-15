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

void make_pipes(int **pipe_array, int num_sort){
 	/* pipe from parser to sort(s) */
	for(int j = 0; j < num_sort; j++){
		if(pipe(pipe_array[j]) != 0){
			perror("pipe");
			exit(-1);
		}
	}
}

void parser(int **par_sort_pipe, int num_sort){

	int j = num_sort;

	/* bind STDOUT to pipe write end */
	if(par_sort_pipe[j][1] != STDOUT_FILENO){
		if(dup2(par_sort_pipe[j][1], STDOUT_FILENO) == -1){
			perror("dup2 2");
		}
		if(close(par_sort_pipe[j][1]) == -1){
			perror("close 5");
		}
	}
	FILE *output[num_sort];
	output[j] = fdopen(STDOUT_FILENO, "w");

	char word_in[50];
	int scan_result = 1;
	while(scan_result != EOF){
		// To do: scanf w/o including chars but 
		// so you can stil read EOF
		// scanf("%[^!-@]", word_in);
		scan_result = scanf("%s", word_in);
		fputs(word_in, output[j]);
		fputc('\n', output[j]);
	}
	/* closing all streams */
	for(int l = 0; l<num_sort; l++){
		fclose(output[l]);	
	}


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

	//char buf[50] = "why does FPUT work?\n";
	//char buf2[50] = "this prints fine.\n";
	//char buf3[50];
	int result;
	int status;
	int j = 0;
	int k = 0;
	pid_t child;
	int num_sort = argc;

	//delete following line for more than one process
	num_sort = 1;

	// pass argc in here eventually
	int **prs_fds = pipe_generator(num_sort);
	int **spr_fds = pipe_generator(num_sort);

	make_pipes(prs_fds, num_sort);
	make_pipes(spr_fds, num_sort);

	while(j < num_sort){
	//	/* pipe from parser to sort(s) */
	//	if(pipe(prs_fds[j]) != 0){
	//		perror("pipe");
	//		exit(-1);
	//	}
		/* pipe from sort(s) to suppressor */
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
			//	close(STDIN_FILENO);
			//	close(STDOUT_FILENO);
				/* close write end of parser pipe */
				if(close(prs_fds[j][1]) == -1){	//write pipe end
					perror("close 1");
				}
				/* close read end of suppressor pipe */
	//			if(close(spr_fds[k][0]) == -1){
	//				perror("close 7");
	//			}				


				/* bind sorts STDIN to output of parser pipe */
				if(prs_fds[j][0] != STDIN_FILENO){
					if(dup2(prs_fds[j][0], STDIN_FILENO) == -1){
						perror("dup2 1");
					}
					if(close(prs_fds[j][0]) == -1){
						perror("close 2");
					}	
				}	
				/* bind sorts STDOUT to input of sup pipe */
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

	//			close(spr_fds[k][1]);
	
				solder_pipes_shut(prs_fds, num_sort);
			//	solder_pipes_shut(spr_fds, num_sort);
				//close(prs_fds[j][0]);
				execlp("sort", "sort", (char *)NULL);
				break;		
	
			default:
				//parent case -- result holds pid of child
				/* close read end of pipe */		
				if(close(prs_fds[j][0]) == -1){
					perror("close 3");
				}
				
				/* bind STDOUT to pipe write end */
				if(prs_fds[j][1] != STDOUT_FILENO){
					if(dup2(prs_fds[j][1], STDOUT_FILENO) == -1){
						perror("dup2 2");
					}
					if(close(prs_fds[j][1]) == -1){
						perror("close 5");
					}
				}
				FILE *output[num_sort];
				output[j] = fdopen(STDOUT_FILENO, "w");

				char word_in[50];
				int scan_result = 1;
				while(scan_result != EOF){
					// To do: scanf w/o including chars but 
					// so you can stil read EOF
					// scanf("%[^!-@]", word_in);
					scan_result = scanf("%s", word_in);
					fputs(word_in, output[j]);
					fputc('\n', output[j]);
				}
				// closing all streams 
				for(int l = 0; l<num_sort; l++){
					fclose(output[l]);	
				}

				//child = wait(&status);
				break; 
		}
		j++;
	}

//	parser(prs_fds, num_sort);

	//if(close(prs_fds[1]) == -1){
	//	perror("close 4");
	//}
	
	child = wait(&status);
	free(prs_fds);
	free(spr_fds);
	return 0;
}
