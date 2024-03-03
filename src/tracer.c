#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include "../includes/pipes.h"

int criaFIFO(char *nome) {
    int res = mkfifo (nome,0640);
    if (res < 0)
        perror("erro no open\n");
    return 0;
}

void destroyFIFO (char *nome) {
    unlink(nome);
}

int status (int fd,char *fifo,informacao_processo *p, char *nome) {
    if (strcmp (nome, "status") != 0)
        return 1;

    strcat (p->nome, nome);
    p->pid = getpid();
    p->ms = -1;
    if (write (fd,p,sizeof (informacao_processo)) == -1) {
        perror ("writing struct");
        return 1;
    }

    int fifo_pid;
    if ((fifo_pid = open (fifo,O_RDONLY)) <0) {
        perror ("Status: open fifo");
        return 1;
    }
    char buffer[50];
    int res;
    while ((res = read (fifo_pid,&buffer,50))> 0) {
        if (write(1,buffer, res) == -1)
            return 1;
    }
    close (fifo_pid);
    return 0;
}

int stats_time (int fd, informacao_processo *p, char *fifoPid,char **argv, int argc) {
    int fres;
    if ((fres = fork()) == 0) {
        int fifo_pid;
        if ((fifo_pid = open (fifoPid, O_WRONLY)) < 0) {
            perror ("open FIFO\n");
            return 1;
        }
        char buffer [100];
        strcat (buffer, argv[2]);
        for (int i = 3; i < argc; i++) {
            strcat (buffer, " ");
            strcat (buffer, argv[i]);
        }

        if (write (fifo_pid, buffer, strlen (buffer)) < 0) {
            perror("writing pids status");
            return 1;
        }
        close (fifo_pid);
        _exit (0);
    }
    else {
        p->ms = -1;
        strcpy (p->nome, "stats-time");
        if (write (fd, p, sizeof (informacao_processo)) < 0) {
            perror ("writing struct\n");
            return 1;
        }
        wait(NULL);
        int res; 
        char buffer [50];
        int st;
        if ((st = open ("tmp/stats-time", O_RDONLY))<0) {
            perror ("stats time: open FIFO\n");
            return 1;
        }
        if ((res = read (st, &buffer, 50)) >0)
            if (write (1,buffer, res) == -1)  { 
                perror ("Writing result");
                return 1;
            }
        close (st);
    }

    return 0;
}

int stats_command (int fd, informacao_processo *p, char *fifoPid,char **argv, int argc) {
    int fres;
    if ((fres = fork()) == 0) {
        int fifo_pid;
        if ((fifo_pid = open (fifoPid, O_WRONLY)) < 0) {
            perror ("open FIFO\n");
            return 1;
        }
        char buffer [100];
        strcat (buffer, argv[2]);
        for (int i = 3; i < argc; i++) {
            strcat (buffer, " ");
            strcat (buffer, argv[i]);
        }

        if (write (fifo_pid, buffer, strlen (buffer)) < 0) {
            perror("writing pids status");
            return 1;
        }
        close (fifo_pid);
        _exit (0);
    }
    else {
        p->ms = -1;
        strcpy (p->nome, "stats-command");
        if (write (fd, p, sizeof (informacao_processo)) < 0) {
            perror ("writing struct\n");
            return 1;
        }
        wait(NULL);
        int res; 
        char buffer [50];
        int st;
        if ((st = open ("tmp/stats-command", O_RDONLY)) < 0) {
            perror ("open FIFO\n");
            return 1;
        }
        if ((res = read (st, &buffer, 50)) >0)
            if (write (1,buffer, res) == -1)  { 
                perror ("Writing result");
                return 1;
            }
        close (st);
    }
    return 0;        
}

int stats_uniq (int fd, informacao_processo *p, char *fifoPid,char **argv, int argc) {
    int fres;
    if ((fres = fork()) == 0) {
        int fifo_pid;
        if ((fifo_pid = open (fifoPid, O_WRONLY)) < 0) {
            perror ("open FIFO\n");
            return 1;
        }
        char buffer [100];
        strcat (buffer, argv[2]);
        for (int i = 3; i < argc; i++) {
            strcat (buffer, " ");
            strcat (buffer, argv[i]);
        }

        if (write (fifo_pid, buffer, strlen (buffer)) < 0) {
            perror("writing pids status");
            return 1;
        }
        close (fifo_pid);
        _exit (0);
    }
    else {
        p->ms = -1;
        strcpy (p->nome, "stats-uniq");
        if (write (fd, p, sizeof (informacao_processo)) < 0) {
            perror ("writing struct\n");
            return 1;
        }
        wait(NULL);
        int res; 
        char buffer [50];
        int st;
        if ((st = open ("tmp/stats-uniq", O_RDONLY)) < 0) {
            perror ("open FIFO\n");
            return 1;
        };
        while ((res = read (st, &buffer, 50)) >0)
            if (write (1,buffer, res) == -1)  { 
                perror ("Writing result");
                return 1;
            }
        close (st);
    }
    return 0;        
}


int comeca (int fd,informacao_processo *p, char **comando, int N) {
    int pid = getpid();
    p->pid = pid;
    p->ms = -1;
    char *token = NULL;
    for (int i = 0; i < N; i++) {
        char *copy = strdup (comando[i]);
        while (isspace(*copy)) {
            copy++;
        }

        token = strtok (copy, " ");
        if (i == 0)
            strcpy (p->nome,token);
        else {
            strcat (p->nome," | "); 
            strcat (p->nome,token);
        }
    }

    if (write (fd,p,sizeof (informacao_processo)) == -1) {
        perror ("writing struct");
        return 1;
    }

    char buffer[50];
    sprintf (buffer, "Pid: %d\n", pid);
    if (write(1,buffer, strlen(buffer)) == -1) {
        perror ("writing pid standart output");
        return 1;
    }

    return 0;
}

char **parsecomando (char *comando) {
    char **array = malloc (sizeof (char *));
    int tamanho = 1;
    char *token;
    token = strtok (comando, " ");
    array[0] = token;
    while ((token = strtok (NULL," ")) != NULL) {
        tamanho++;
        if (realloc (array, sizeof (char *) * tamanho) ==NULL) {
            perror ("Realloc\n");
            return NULL;
        }
        array[tamanho -1] = token;
    }

    tamanho++;
    if (realloc (array, sizeof (char *) * tamanho) == NULL) {
        perror ("Realloc\n");
        return NULL;
    }
    array[tamanho -1] = NULL;
    return array;
}


void executa_pipeline (char **comandos, int N) {
    int fres;
    int p[N-1][2];
    for (int i = 0; i < N; i++) {
        if (i != N-1) pipe(p[i]);
        if (i == 0) {
            if ((fres = fork()) == 0) {
                close (p[i][0]);
                dup2 (p[i][1], 1);
                close (p[i][1]);
                char **comando = parsecomando(comandos[i]);
                execvp (comando[0], comando);
                _exit(i);
            }
            else
                close (p[i][1]);
        }
        else if (i == N-1) {
                if ((fres = fork()) == 0) {
                    dup2 (p[i-1][0], 0);
                    close (p[i-1][0]);
                    char **comando = parsecomando(comandos[i]);
                    execvp (comando[0], comando);
                    _exit(i);
                }
                else {
                    wait(NULL);
                    close (p[i-1][0]);
                }
        }
        else {
            if ((fres = fork()) == 0) {
                    dup2 (p[i-1][0], 0);
                    close (p[i-1][0]);

                    close (p[i][0]);
                    dup2 (p[i][1], 1);
                    close (p[i][1]);
                    
                    char **comando = parsecomando(comandos[i]);
                    execvp (comando[0], comando);
                    _exit(i);
                }
                else {
                    close (p[i][1]);
                    close (p[i-1][0]);
                }
        }
    }
}


int executar (char *opcao, char *comandos[], int N) {
    
    if (strcmp (opcao, "-u") == 0) {
        // calcular array com comando e argumentos
        char **comando = parsecomando (comandos[0]);
        // executar comando
        int fres;
        if ((fres = fork ()) == 0) {
            execvp (comando[0], comando);
            _exit(1);
        }
        else
            wait (NULL);
        free (comando);
    }
    else if (strcmp (opcao, "-p") == 0) {
        executa_pipeline (comandos, N);
    }
    return 0;
}

int acaba (int fd, informacao_processo *p) {
    struct timeval now;
    gettimeofday(&now,NULL);
    p->ms= (now.tv_sec - p->time.tv_sec) * 1000 + (now.tv_usec - p->time.tv_usec) / 1000;
    p->time = now;
    if (write(fd,p,sizeof(informacao_processo)) == -1) {
        perror ("writing struct");
        return 1;
    }

    char buffer [50];
    sprintf(buffer,"Elapsed time: %ld ms\n", p->ms);

    if (write(1,buffer, strlen(buffer)) == -1) {
        perror ("writing time ellapsed");
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    struct timeval now;
    gettimeofday (&now,NULL);
    informacao_processo *p = malloc (sizeof(informacao_processo));
    p->time = now;

    if (argc <= 1) {
        perror ("Erro no número de argumentos\n");
        return 1;
    }

    int fd = open ("tmp/FIFO",O_WRONLY);
    if (fd < 0)
        perror("erro no open FIFO\n");
        
    char buffer[50];
    sprintf (buffer, "tmp/FIFO%d", getpid());
    criaFIFO (buffer);

    if (argc == 2) {
        if (status (fd,buffer,p,argv[1]))
            return 1;
    }
    else if (strcmp (argv[1], "stats-time") == 0) {
        criaFIFO ("tmp/stats-time");
        p->pid = getpid();
        if (stats_time(fd,p,buffer,argv, argc)) {
            perror ("stats time!");
            return 1;
        }
        destroyFIFO ("tmp/stats-time");
    }
    else if (strcmp (argv[1], "stats-command") == 0) {
        criaFIFO ("tmp/stats-command");
        p->pid = getpid();
        if (stats_command (fd, p, buffer, argv, argc)) {
            perror ("stats command!");
            return 1;
        }
        destroyFIFO ("tmp/stats-command");
    }
    else if (strcmp (argv[1], "stats-uniq") == 0) {
        criaFIFO ("tmp/stats-uniq");
        p->pid = getpid();
        if (stats_uniq (fd, p, buffer, argv, argc)) {
            perror ("stats uniq!");
            return 1;
        }
        destroyFIFO ("tmp/stats-uniq");
    }
    else {
        char **comandos = malloc (sizeof (char *));
        int tamanho = 1;
        char *token;
        token = strtok (argv[3], "|");
        comandos[0] = token;
        while ((token = strtok (NULL,"|")) != NULL) {
            tamanho++;
            if (realloc (comandos, sizeof (char *) * tamanho) == NULL)
                return 1;
            comandos[tamanho -1] = token;
        }
        if (comeca(fd,p, comandos, tamanho))
            return 1;

        int fres;
        if ((fres = fork())  == 0) {
            if (executar(argv[2],comandos, tamanho))
                return 1; 
            _exit (0);
        }
        else 
            wait (NULL);
        if (acaba(fd,p))
            return 1;
        free (comandos);    
    }

    close(fd);
    free (p);
    destroyFIFO (buffer);
    return 0;
}