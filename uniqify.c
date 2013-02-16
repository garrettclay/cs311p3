#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct word_cnt{
	char word[50];
	int cnt;
};

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

	int k = 0;
	FILE *output[num_sort];
	char word_in[50];
	int scan_result = 1;

	//int j = num_sort;
	for(int j = 0; j < num_sort; j++){
		output[j] = fdopen(par_sort_pipe[j][1], "w");
	}

	while(scan_result != EOF){
		// To do: scanf w/o including chars but 
		// so you can stil read EOF
		scan_result = scanf("%[a-zA-Z]", word_in);
		fputs(word_in, output[k]);
		fputc('\n', output[k]);
		scan_result = scanf("%*[^a-zA-Z]");
		k++;
		if(k == num_sort){
			k = 0;
		}
	}
	
	/* closing all streams */
	for(int l = 0; l<num_sort; l++){
		fclose(output[l]);	
	}

}

void suppressor(int **input_pipe, int num_sort){

	FILE *input[num_sort];
	char **buf;
	int empty_pipe_cnt = 0;
	int k = 0;

	buf = malloc(sizeof(char *) * num_sort);
	for(int j = 0; j < num_sort; j++){
		buf[j] = malloc(50 * sizeof(char));
	}

	for(int i = 0; i < num_sort; i++){
		input[i] = fdopen(input_pipe[i][0], "r");
	}

	while(empty_pipe_cnt < num_sort){

		if(fgets(buf[k], 50, input[k]) == EOF){
			empty_pipe_cnt++;
		}
		//use strcmp to see which word is alphabetically
		//smaller. need to get rid of capitals still



	}
	


	free(buf);
}

void solder_pipes_shut(int **open_pipe, int num_pipes){
	int i;
	int j;
	for(i=0; i < num_pipes; i++){
		for(j=0; j < 2; j++){
			if(close(open_pipe[i][j]) == -1){
				perror("recursive close");
			}
		}	
	}

}

int main(int argc, char **argv){

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

	parser(prs_fds, num_sort);

	while(j < num_sort){
		switch((result = fork())){
			case -1:
				//in parent
				perror("fork");
				exit(-1);
				break;

			case 0:
				//child case 
				close(STDIN_FILENO);
		//		close(STDOUT_FILENO);
	
				/* close write end of parser pipe */
	//			if(close(prs_fds[j][1]) == -1){	
	//				perror("close 1");
	//			}
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
	
				solder_pipes_shut(prs_fds, num_sort);
				solder_pipes_shut(spr_fds, num_sort);
				execlp("sort", "sort", (char *)NULL);
				break;		
	
			default:
				//parent case -- result holds pid of child
				/* close read end of pipe */		
				if(close(prs_fds[j][0]) == -1){
					perror("close 3");
				}
				if(close(spr_fds[j][1]) == -1){
					perror("close 8");
				}		
	
				//child = wait(&status);
				break; 
		}
		j++;
	}

	suppressor(spr_fds, num_sort);

	//if(close(prs_fds[1]) == -1){
	//	perror("close 4");
	//}
	for(int h = 0; h < num_sort; h++){
		child = wait(&status);
	}

	free(prs_fds);
	free(spr_fds);
	return 0;
}
