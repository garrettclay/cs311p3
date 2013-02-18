#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct word_cnt{
	char word[50];
	int cnt;
	int index;
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

	scan_result = scanf("%*[^a-zA-Z]");
	while(scan_result != EOF){
		scan_result = scanf("%[a-zA-Z]", word_in);
	
		/* decapitalizes words by character */	
		for(int m = 0; m < sizeof(word_in); m++){
			word_in[m] = tolower(word_in[m]);
		}

		fputs(word_in, output[k]);
		fputc('\n', output[k]);
		scan_result = scanf("%*[^a-zA-Z]");
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
	struct word_cnt *cur_word = malloc(sizeof(struct word_cnt));
	cur_word->cnt = 1;
	char *z_buf = "zzzzzzzzzz";

	buf = malloc(sizeof(char *) * num_sort);
	for(int j = 0; j < num_sort; j++){
		buf[j] = malloc(50 * sizeof(char));
	}

	// opens all pipes and initializes bufs
	for(int i = 0; i < num_sort; i++){
		input[i] = fdopen(input_pipe[i][0], "r");
		if(fgets(buf[i], 50, input[i]) == NULL){
			buf[i] = NULL;
			empty_pipe_cnt++;
		}
	}

	strncpy(cur_word->word, buf[0], 50);
	cur_word->index = 0;
	cur_word->cnt = 1;

	while(empty_pipe_cnt < num_sort){

		for(int i = 0; i < num_sort; i++){
		

	
			// if cur_word is equal to buf but not same index
			// replace copy and keep count
			if((strcmp(cur_word->word, buf[i]) == 0) && (cur_word->index != i)){
				cur_word->cnt++;
				if(fgets(buf[i], 50, input[i]) == NULL){
					buf[i] = NULL;
					empty_pipe_cnt++;
				}
				i--;
				if(buf[i] == NULL){
					printf("\nI am here\n");	
					strncpy(buf[i], z_buf, 10);
				}
			}
			// if buf is alpha highter (a,b,c..)
			// replace cur_word
			else if((strcmp(cur_word->word, buf[i]) > 0)){
				strncpy(cur_word->word, buf[i], 50);
				cur_word->index = i;
				cur_word->cnt = 1;		
			}
		}
	
		// last check, see if cur_word pipe has same
		// word behind it
		while(cur_word->index != -1){
			int j = cur_word->index;
			if(fgets(buf[j], 50, input[j]) == NULL){
				buf[j] = NULL;
				empty_pipe_cnt++;
			}

			if(buf[j] == NULL){
				//strncpy(buf[j], z_buf, 10);
				buf[j] = z_buf;
			}

			if((strcmp(cur_word->word, buf[j])) == 0){
				cur_word->cnt++;
				if(fgets(buf[j], 50, input[j]) == NULL){
					buf[j] = NULL;		
					empty_pipe_cnt++;
				}
				if(buf[j] == NULL){
					//strncpy(buf[j], z_buf, 10);
					buf[j] = z_buf;
				}
			}
			else{
				cur_word->index = -1;
			}
		}

		printf(" %d %s\n", cur_word->cnt, cur_word->word);
		strncpy(cur_word->word, buf[0], 50);
		cur_word->index = 0;
		cur_word->cnt = 1;
	}
	free(buf);
}

void solder_pipes_shut(int **open_pipe, int num_pipes){
	int i;
	int j;
	for(i=0; i < num_pipes; i++){
		for(j=0; j < 2; j++){
			if(close(open_pipe[i][j]) == -1){
			//	perror("recursive close");
			}
		}	
	}

}

int main(int argc, char **argv){

	int result;
	int status;
	int j = 0;
	pid_t child;
	int num_sort = argc;

	//delete following line for more than one process
	num_sort = 3;

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
				close(STDOUT_FILENO); // comment out to work
	
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
				if(spr_fds[j][1] != STDOUT_FILENO){
					if(dup2(spr_fds[j][1], STDOUT_FILENO) ==-1){
						perror("dup3 1");
					}
					if(close(spr_fds[j][1]) == -1){
						perror("close 6");
					}
				}
	
	
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
