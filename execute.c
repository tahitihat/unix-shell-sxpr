#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <fcntl.h>
#include <assert.h> 
#include <stdlib.h>

#include "execute.h"
#include "ivec.h"

/*
 * Confirm that return value is not an error.
 */
void
check_rv(int rv)
{
	if (rv == -1) {
		perror("fail");
		exit(1);
	}
}

/*
 * Return 1 if given strings are the same.
 */
int
streq(const char* aa, const char* bb)
{
	return strcmp(aa, bb) == 0;
}

/*
 * Return index of the closing paren (accounts for nested paren).
 */
int
find_paren(ivec* xs, int start)
{
	int nested = 0;
        int ii = start;
        while(1) {
                if (streq(xs->data[ii], ")")) {
                        if (nested) {
                                nested--;
                        }
                        if (!nested) {
                                break;
                        }
                }
                else if (streq(xs->data[ii], "(")) {
                        nested++;
                }
                ii++;
        }
	return ii;
}

/*
 * Process a simple command -- no operators.
 */
int 
cmnd_simple(ivec* xs) 
{
	char* cmnd = xs->data[1];
	int rv = 0;

	if (streq(cmnd, "exit")) {
		exit(0);
	} else if (streq(cmnd, "cd")) {
		rv = chdir(xs->data[2]);
		check_rv(rv);
	} else {
		int numArgs = length(xs) - 1;
		char* args[numArgs];
		int ii;
		for (ii = 0; ii < length(xs) - 2; ii++) {
			args[ii] = xs->data[ii + 1];
		}
		args[numArgs - 1] = NULL;
		int cpid;
		if ((cpid = fork())) {
			int status = 0;
			wait(&status);
			rv = status;
		}
		else {
			execvp(*args, args);
		}
	}
	return rv;
}

/*
 * Process redirection command: < or > 
 */
int
cmnd_redirect(ivec* xs)
{
	char* cmnd = xs->data[1];
	int rv = 0;
	int ii = find_paren(xs, 2);
	ivec *arg1 = partition(xs, 2, ii);
	char* FILE = xs->data[length(xs) - 2];
	int cpid;
	if ((cpid = fork())) {
		int status = 0;
		wait(&status);
		rv = status;
	}
	else {
		int x;
		if (streq(cmnd, "<")) {	
			x = open(FILE, O_RDONLY, 0777);
			close(0);
		}
		else {
			x = open(FILE, O_WRONLY | O_CREAT | O_TRUNC, 0777);
			close(1);
		}
		rv = dup(x);
		check_rv(rv);
		close(x); 
		int result = execute(arg1);
		exit(result);
	}
	free_ivec(arg1);
	return rv;
}

/*
 * Execute 2 commands in sequence (process ; operator).
 */
int 
cmnd_seq(ivec* xs) 
{
	int rv = 0;
	int nested = 0;
	int ii = find_paren(xs, 2);
	ivec *arg1 = partition(xs, 2, ii);
	int jj = find_paren(xs, ii + 1);
	ivec *arg2 = partition(xs, ii + 1, jj);
	int cpid;
	if ((cpid = fork())) {
		int status = 0;
		wait(&status);
		rv = status;
	}
	else {
		execute(arg1);
		int res = execute(arg2);
		exit(res);
	}
	free_ivec(arg1);
	free_ivec(arg2);
	return rv;
}

/*
 * Process logical operator: || or &&.
 */
int
cmnd_logic(ivec* xs)
{
	int rv;
	char* cmnd = xs->data[1];	
	int nested = 0;
	int ii = find_paren(xs, 2);
	ivec *arg1 = partition(xs, 2, ii);
	int jj = find_paren(xs, ii + 1);
	ivec *arg2 = partition(xs, ii + 1, jj);

	int cpid;
	if ((cpid = fork())) {
		int status = 0;
		wait(&status);
		rv = status;
		int proceed = 0;
		if (streq(cmnd, "&&") && !WEXITSTATUS(status)) {
			proceed = 1;
		}
		else if (streq(cmnd, "||") && WEXITSTATUS(status)) {
			proceed = 1;
		}

		if (proceed) {
			int cpid2;
			if ((cpid2 = fork())) {
				int status2 = 0;
				wait(&status2);
				rv = status2;
			}
			else {
				int result2 = execute(arg2);
				exit(result2);
			}
		}
	}
	else {
		int result = execute(arg1);
		if (result) {
			exit(1);
		}
		else {
			exit(0);
		}
	}
	free_ivec(arg1);
	free_ivec(arg2);
	return rv;
}

/*
 * Run a command in the background without waiting for it to finish (& op).
 */
int
cmnd_sleep(ivec* xs)
{
	int rv = 0;
	int ii = find_paren(xs, 2);
	ivec *arg1 = partition(xs, 2, ii);
	int secondarg = 0;
	ivec *arg2;
	if (streq(xs->data[ii + 1], "(")) {
		secondarg = 1;
		int jj = find_paren(xs, ii + 1);
		arg2 = partition(xs, ii + 1, jj);	
	}
	int cpid;
	if ((cpid = fork())) {
		if (secondarg) {
			execute(arg2);
		}
	}
	else {
		setpgid(0, 0);
		execute(arg1);
	}
	free_ivec(arg1);
	if (secondarg) {
		free_ivec(arg2);
	}
	return rv;	
}


/*
 * Execute commands in given vector. 
 * Note that tokens are ordered as s-expressions.
 */
int
execute(ivec* xs) 
{
	char* cmnd = xs->data[1];

	int rv = 0;
	if (streq(cmnd, "<") || (streq(cmnd, ">"))) {
		rv = cmnd_redirect(xs);
	}
	else if (streq(cmnd, "&&") || (streq(cmnd, "||"))) {
		rv = cmnd_logic(xs);
	}
	else if (streq(cmnd, "&")) {
		rv = cmnd_sleep(xs);
	} 
	else if (streq(cmnd, ";")) {
		rv = cmnd_seq(xs);
	}
	else {
		rv = cmnd_simple(xs);
	}
	return rv;
}
