#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
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

int readStatusServer (char *fifo_pid, informacao_processo *status, informacao_processo *array, int pos) {
    int fres;
    if ((fres =fork()) == 0) {  
        int fd;
        if ((fd = open (fifo_pid,O_WRONLY,0640)) < 0) {
            perror ("readStatusServer: open FIFO\n");
            return 1;
        }
        for (int i = 0; i < pos; i++) {
            char buffer[100];
            sprintf (buffer, "%d %s %ld ms\n", array[i].pid, array[i].nome,  (status->time.tv_sec - array[i].time.tv_sec) * 1000 + (status->time.tv_usec - array[i].time.tv_usec) / 1000);
            if (write (fd,buffer, strlen(buffer)) < 0) {
                perror ("error writing status\n");
                return 1;
            }
        }
        close (fd);
        _exit(0);
    }
    return 0;
}

int Stats_time (char *fifo_pid, char *path) {
    int fres;
    if ((fres =fork()) == 0) {  
        int res, res2;
        char buffer [100];
        long total = 0;
        int fd,st;
        if ((fd = open (fifo_pid, O_RDONLY)) < 0) {
            perror ("Open Fifo\n");
            return 1;
        }
        if ((st = open ("tmp/stats-time", O_WRONLY)) < 0) {
            perror ("Open Fifo stats\n");
            return 1;
        }
        informacao_processo p;
        while ((res = read (fd,buffer,100)) > 0) {
            char *token = strtok (buffer, " ");
            char *path_file = strdup (path);
            while (token) {
                strcat (path_file, token);
                int file;
                if ((file = open (path_file, O_RDONLY, 0640)) <0) {
                    perror ("Status_time: open file");
                    return 1;
                }
                if ((res2 = read(file, &p, sizeof (informacao_processo))) > 0) {
                    total += p.ms;
                }
                close (file);
                path_file[strlen(path)] = '\0';
                token = strtok (NULL, " ");
            }
            free (path_file);
            free (token);
        }
        close (fd);

        char buffer2[50];
        sprintf (buffer2, "Total execution time is %ld ms\n",total);
        if (write (st,buffer2,strlen (buffer2)) == -1) {
            perror ("writing result fifo");
            return 1;
        }
        close (st);
        _exit(0);
    }
    return 0;
}

int Stats_command (char *fifo_pid, char *path) {
    int fres;
    if ((fres =fork()) == 0) {  
        int res, res2, total = 0,fd,st;
        char buffer [100];
        char *program = NULL;
        informacao_processo p;

        if ((fd = open (fifo_pid, O_RDONLY)) < 0) {
            perror ("Open Fifo\n");
            return 1;
        }
        if ((st = open ("tmp/stats-command", O_WRONLY)) < 0) {
            perror ("Open Fifo stats\n");
            return 1;
        }
        //ler do fifo os nomes dos pids
        while ((res = read (fd,buffer,100)) > 0) {
            // evitar lixo no final do buffer
            buffer[res] = '\0';

            char *inner_pointer, *outer_pointer;

            //Separar nome do programa da string
            char *token = strtok_r (buffer, " ", &outer_pointer);
            program = strdup (token);

            token = strtok_r (NULL, " ", &outer_pointer);
            char *path_file = strdup (path);
            // passar pelos ficheiros todos
            while (token) {
                strcat (path_file, token);
                int file;
                if ((file = open (path_file, O_RDONLY, 0640)) <0) {
                    perror ("Status_command: open file");
                    return 1;
                }
                if ((res2 = read(file, &p, sizeof (informacao_processo))) > 0) {
                    char *tmp = strtok_r (p.nome, " | ", &inner_pointer);
                    // procurar pelo programa no nome
                    while (tmp) {
                        if (strcmp(tmp, program) == 0)
                            total ++;
                        tmp = strtok_r (NULL, " | ", &inner_pointer);
                    }
                }

                close (file);
                path_file[strlen(path)] = '\0';
                token = strtok_r (NULL, " ",&outer_pointer);
            }

            free (path_file);
            free (token);
        }
        close (fd);
        char buffer2[50];
        sprintf (buffer2, "%s was executed %d times\n",program, total);
        if (write (st,buffer2,strlen (buffer2)) == -1) {
            perror ("writing result fifo");
            return 1;
        }
        close (st);
        free (program);
        _exit(0);
    }
    return 0;
}

int Stats_uniq (char *fifo_pid, char *path) {
    int fres;
    if ((fres =fork()) == 0) {
        int capacidade = 2, pos = 0; 
        char **unicos = malloc (sizeof (char *) * 2);
        int res, res2, fd,st;
        char buffer [100];
        if ((fd = open (fifo_pid, O_RDONLY)) < 0) {
            perror ("Open Fifo\n");
            return 1;
        }
        if ((st = open ("tmp/stats-uniq", O_WRONLY)) < 0) {
            perror ("Open Fifo stats\n");
            return 1;
        }
        informacao_processo p;
        while ((res = read (fd,buffer,100)) > 0) {
            buffer[res] = '\0';
            char *inner_pointer, *outer_pointer;

            char *token = strtok_r (buffer, " ", &outer_pointer);
            char *path_file = strdup (path);
            while (token) {
                strcat (path_file, token);
                int file;
                if ((file = open (path_file, O_RDONLY, 0640)) <0) {
                    perror ("Status_command: open file");
                    return 1;
                }
                if ((res2 = read(file, &p, sizeof (informacao_processo))) > 0) {
                    char *tmp = strtok_r (p.nome, " | ", &inner_pointer);
                    while (tmp) {
                        int exist = 0;
                        for (int i = 0; i < pos; i++) {
                            if (strcmp (unicos [i], tmp) == 0) {
                                exist = 1;
                                break;
                            } 
                        }
                        if (!exist) {
                            if (pos == capacidade) {
                                if ((unicos = realloc(unicos, 2 * capacidade * sizeof (char *))) == NULL) {
                                    perror ("cant allocate memory for uniq processes\n");
                                    return 1;
                                }
                                capacidade *= 2;
                            }
                            unicos[pos++] = strdup (tmp);
                        }
                        tmp = strtok_r (NULL, " | ", &inner_pointer);
                    }
                }
                close (file);
                path_file[strlen(path)] = '\0';
                token = strtok_r (NULL, " ", &outer_pointer);
            }
            free (path_file);
            free (token);
        }
        close (fd);

        for (int i = 0; i < pos; i++) {
            strcat (unicos[i], "\n");
            if (write (st,unicos[i],strlen (unicos[i])) == -1) {
                perror ("writing result fifo");
                return 1;
            }
            free (unicos[i]);
        }
        free (unicos);
        close (st);
        _exit(0);
    }
    return 0;
}


int acabaProcesso (char *path, informacao_processo *p) {
    int fres;
    if ((fres = fork()) == 0) {    
        if (path) {
            char buffer[100];
            sprintf (buffer, "%s%d", path,p->pid);
            int processo;
            if ((processo = open (buffer, O_WRONLY | O_CREAT | O_TRUNC, 0640)) < 0) {
                perror ("Nao conseguiu criar o ficheiro do processo\n");
                return 1;
            }
            if (write (processo, p, sizeof (informacao_processo)) == -1) {
                perror ("Escrita nos processos terminados\n");
                return 1;
            }
            close (processo);
        }
        _exit(0);
    }
    return 0;
}

void removeProcesso (informacao_processo *array, int pid, int *pos) {
    int el = -1;
    for (int i = 0; i < (*pos); i++) {
        if (array[i].pid == pid) {
            el = i;
            break;
        }
    }
    if (el > -1) {
        for (int i= el; i < (*pos) - 1; i++) {
            array [i] = array[i+1];
        }
        (*pos)--;
    }
}

int readlinepipe (char *path, informacao_processo *array, int *capacidade, int *pos) {
    int fd = open ("tmp/FIFO",O_RDONLY);
    if (fd < 0)
        perror("erro no open FIFO\n");
    informacao_processo p;
    int res;
    if ((res = read (fd,&p,sizeof(informacao_processo)))> 0) {
        char buffer[50];
        sprintf (buffer, "tmp/FIFO%d", p.pid);
        if (p.ms != -1) {
            removeProcesso (array,p.pid, pos);
            acabaProcesso (path,&p);
        }
        else {
            if (strcmp (p.nome, "status") == 0) {
                if(readStatusServer (buffer,&p, array, *pos))
                    return 1;
            }
            else if (strcmp (p.nome, "stats-time") == 0) {
                if (Stats_time (buffer, path))
                    return 1;
            }
            else if (strcmp (p.nome, "stats-command") == 0) {
                if (Stats_command (buffer, path))
                    return 1;
            }
            else if (strcmp (p.nome, "stats-uniq") == 0) {
                if (Stats_uniq (buffer, path))
                    return 1;
            }
            else {
                if ((*pos) == (*capacidade)) {
                    if ((array = realloc (array, 2 * sizeof (informacao_processo) * (*capacidade))) == NULL) {
                        perror ("cant alloc more memory\n");
                        return 1;
                    }
                    *capacidade *= 2;
                }
                array [(*pos)++] = p;
            }
        }
    }

    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    int capacidade = 2, pos = 0;
    informacao_processo *array = malloc (sizeof (informacao_processo) * 2);
    criaFIFO("tmp/FIFO");
    while (1)
        if (argc == 2) {
            if (readlinepipe(argv[1], array, &capacidade, &pos))
                break;    
        }
        else if (readlinepipe(NULL, array, &capacidade, &pos))
            break;
    destroyFIFO("tmp/FIFO");
    return 0;
}