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

	for(i = 0; i < num_sort; i++){
		// if cur_word is equal to buf but not same index
		// replace copy and keep count
		if((strcmp(cur_word->word, buf[i]) == 0) && (cur_word->index != i)){
			cur_word->cnt++;
			if(fgets(buf[i], 50, input[i]) == NULL){
				buf[i] = NULL;
				empty_pipe_cnt++;
			}
			i--;
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
		if((strcmp(cur_word->word, buf[j])) == 0){
			cur_word->cnt++;
			if(fgets(buf[j], 50, input[j]) == NULL){
				buf[j] = NULL;		
				empty_pipe_cnt++;
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
