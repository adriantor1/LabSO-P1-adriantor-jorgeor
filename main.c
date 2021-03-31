/*
 Simulador de planificacion de CPU para un solo procesador.
 Derechos Reservados de Erwin Meza Vega <emezav@gmail.com>.
 
 Presentado por: 
 
 Adrian Camilo Torres Gómez 104616011334 adriantor@unicauca.edu.co
 Jorge Ortis codigo jorgeor@unicauca.edu.co
 
 IMPORTANTE
 Este código se proporciona como una guía general para implementar
 el simulador de planificación. El estudiante deberá verificar su
 funcionamiento y adaptarlo a las necesidades del problema a solucionar.
 
 El profesor no se hace responsable por las omisiones, los errores o las 
 imprecisiones que se puedan encontrar en este código y los archivos relacionados.
 
 USO:
 
 ./nombre_ejecutable archivo_configuracion
 ./nombre_ejecutable < archivo_configuracion
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "split.h"
#include "list.h"


char * lcase(char *str);

#define equals(x, y) (strcmp(x, y) == 0)

/* Tipos de enumeracion*/
enum strategy {FIFO, SJF, RR};
enum status { UNDEFINED, LOADED, READY, RUNNING, BLOCKED, FINISHED};
enum type {CPU, LOCK};

/* Funciones para la cola de prioridad */
struct instruction_t{
     enum type type;
     int time;
     int remaining;
}; 

typedef struct instruction_t instruction;

typedef struct sequence_item {
        char * name;
        int time;
}sequence_item;

/* Estructura de datos del proceso */
struct process_t {
       char name[80];
       int priority;
       int arrival_time;
       int resume_time;       
       int waiting_time;
       int finished_time;
       enum status status;
       list * instructions;
       node_iterator thread;
};

typedef struct process_t process;

/* Estructuras de las listas de procesos */
struct priority_queue_t {
       int quantum;
       enum strategy strategy;
       list *ready;
       list *arrival;
       list *finished;
};

typedef struct priority_queue_t priority_queue;


priority_queue * create_queues(int);
void print_queue(priority_queue *);

/* Retorna el numero de procesos listos en una cola de prioridad */
int get_ready_count(priority_queue *, int);

/* Retorna el tiempo en el cual se presenta la nueva llegada a la cola 
de listos de una cola de prioridad */
int get_next_arrival(priority_queue *, int);

/* Procesa la llegada de procesos en el tiempo especificado */
int process_arrival(int, priority_queue *, int);

/* Cuenta el numero de procesos bloqueados de una cola de prioridad*/
int count_blocked(priority_queue *);

/* Imprime la informacion de un proceso */
void print_process(process *);

/* Crea un proceso a partir de la linea de configuracion */
process * create_process(char *);

/* Fusiona tajadas de tiempo similares */
void merge_slices(process *);

/* Re-inicia un proceso*/
void restart_process(process *);

/* Funcion que compara dos tiempos de llegada */
int compare_arrival(const void *, const void *);

/* Funcion que permite planificar los procesos*/
void schedule(list*, priority_queue *, int );

/**/
void prepare(list *, priority_queue *, int );

/* Actualizar el tiempo de espera de los procesos que se encuentran
en estado READY*/
void update_waiting_time(list *, int);


/* Programa principal*/
int main(int argc, char * argv[]) {
    char linea[80];
    FILE *fd;
    split_list * t;
    char ** aux;
    int i;
    int quantum;
    int simulated = 0;

    int nqueues;
    
  
    /* Referencia al arreglo de colas de prioridad */  
    priority_queue * queues;
    
    /* Lista de procesos */
    list * processes;

    /* Referencia a un proceso */
    process * p;

    printf("Scheduler start\n");
    
    /* Leer el archivo de configuracion, pasado como parametro al main
    o por redireccion de la entrada estandar */
    if (argc < 2) {
       fd = stdin;
    }else {
          fd = fopen(argv[1], "r");
          if (!fd) {fd = stdin;}
    }
    
    nqueues = 0;
    /* Lectura del archivo de configuracion */
    while (!feof(fd)) {
          
         strcpy(linea, "" );
         fgets(linea, 80, fd);

         //printf("Linea leida: %s", linea);

         if (strlen(linea) <= 1) {continue;}
         if (linea[0] == '#') {continue;}          
         
         //Convertir en minusculas
         lcase(linea);
    
          // Partir la linea en tokens
          t = split(linea, 0);
          //Ignora las lineas que no contienen tokens
          if (t->count == 0) { continue;}
          
          //Procesar cada linea, aux apunta al arreglo de tokens
          aux = t->parts;

         
          if (equals(aux[0], "define") && t->count >= 3) {
                //Comando define queues n   
               if (equals(aux[1], "queues")) {                     
                  nqueues = atoi(aux[2]);
                  //Crear el arreglo de colas de prioridad y la lista de procesos
                  if (nqueues > 0) {
                     queues = create_queues(nqueues);
                     processes = create_list();
                  }
                  simulated = 0;
               }else if (equals(aux[1], "scheduling") && t->count >= 4){
                    //Comando scheduling n ESTATEGIA
                    //n = 1 ... # de colas de prioridad
                    
                    //i = n - 1, los arreglos comienzan en cero
                    i = atoi(aux[2]) - 1;
                    //printf("Defining scheduling to queue %d\n", i);
                    if (i < 0 || i >= nqueues) {continue;}
                    if (equals(aux[3], "rr")) {
                       queues[i].strategy = RR;
                    }else if (equals(aux[3], "sjf")) {
                       queues[i].strategy = SJF;
                    }
                    else if (equals(aux[3], "fifo")) {
                       queues[i].strategy = FIFO;
                    }
               }else if (equals(aux[1], "quantum") && t->count >= 4){
                    //Comando scheduling n QUANTUM
                    //n = 1 ... # de colas de prioridad
                    i = atoi(aux[2]) - 1;
                    //printf("Defining quantum to queue %d\n", i);
                    quantum = atoi(aux[3]);
                    if (i < 0 || i >= nqueues) {continue;}
                    queues[i].quantum = quantum;
               }
          }
          else if (equals(aux[0], "process")&& t->count > 1){
                 //Comando process FILENAME
                //printf("process %s\n", aux[1]);
                p = create_process(aux[1]);
                if (p != 0 && p->priority >= 0) {
                   //printf("Inserting process %s\n", p->name);
                   //Insertar el proceso en la lista general de procesos
                   insert_ordered(processes, p, compare_arrival);
                }
                //print_process(p);                
          }
          else if (equals(aux[0], "start")) {
                //Comando start
                //Comenzar la simulacion!!! 

                schedule(processes, queues, nqueues);
                
                simulated = 1;

                //system("pause");
          }                  
    }
}


/* Funcion para crear las colas de prioridad */
priority_queue * create_queues(int n) {
      priority_queue * ret;
      int i;
      
      ret = (priority_queue *)malloc(sizeof(priority_queue) * n);      
      
      for (i=0; i<n; i++) {
          ret[i].strategy = RR;   //Por defecto RR
          ret[i].quantum =  0;
          ret[i].arrival = create_list();
          ret[i].ready = create_list();
          ret[i].finished = create_list();
      }      

      return ret;
      
}

/*Rutina para imprimir una cola de prioridad */
void print_queue(priority_queue *queue) {
     int i;
     node_iterator  ptr;

     printf("Priority queue\n");
     printf("\tstrategy: %s\n", (queue->strategy == RR)?"RR":((queue->strategy == FIFO)?"FIFO":((queue->strategy == SJF)?"SJF":"UNKNOWN")));
     printf("\tquantum: %d\n", queue->quantum);
     
     printf("Ready list: %d\n", queue->ready->count);

     for (ptr = head(queue->ready); ptr != 0; ptr= next(ptr)) {
           print_process((process*)ptr->data);
     }

     printf("Arrival list: %d\n", queue->arrival->count);

     for (ptr = head(queue->arrival); ptr != 0; ptr= next(ptr)) {
           print_process((process*)ptr->data);
     }
     
     printf("Finished list: %d\n", queue->finished->count);

     for (ptr = head(queue->finished); ptr != 0; ptr= next(ptr)) {
           print_process((process*)ptr->data);
     }

}

/* Comparar dos procesos*/
int compare_process(void * a, void * b) {
    process * p1;
    process * p2;

    p1 = (process * )a;
    p2 = (process * )b;    

    return p2->resume_time - p1->resume_time;
}

/* Comparar dos procesos por tiempo de llegada */
int compare_arrival(const void *a, const void *b) {
    process *p1;
    process *p2;

    p1 = (process *) a;
    p2 = (process *) b;

    //printf("Comparing %s to %s : %d %d\n", p1->name, p2->name, p1->arrival_time, p2->arrival_time);

    return p2->arrival_time - p1->arrival_time;
}

int compare_resume(const void *a, const void *b) {
    process *p1;
    process *p2;

    p1 = (process *) a;
    p2 = (process *) b;

    if (p2->status == LOADED) { //El proceso en la cola es LOADED?
          return p2->arrival_time - p1->resume_time; 
    }else { //El proceso en la cola es BLOCKED?
          return p2->resume_time - p1->resume_time;
    }
    return -1;
}

/* Rutina para leer la informacion de un proceso */
process * create_process(char *filename) {
        FILE *fd;
        char linea[80];
        split_list *t;
        char ** aux;

        instruction *ins;
        
        process * ret;

        ret = 0;

        fd = fopen(filename, "r");
        if (!fd) {
           printf("file %s not found\n", filename);
           return ret;
        }

           ret = (process *) malloc(sizeof(process));
           strcpy(ret->name, filename);
           ret->arrival_time = -1;
           ret->priority = -1;
           ret->resume_time = -1;
           ret->waiting_time = -1;
           ret->finished_time = -1;
           ret->status = UNDEFINED;
           ret->instructions = create_list();
           ret->thread = 0;

           while (!feof(fd)) {
                 fgets(linea, 80, fd);
                 lcase(linea);
                 if (strlen(linea) <= 1) {continue;}
                 if (linea[0] == '#') {continue;}
                 t = split(linea, 0);
                 if (t->count <= 0) {continue;} 
                 aux = t->parts;

                 if (equals(aux[0], "begin")&& t->count >= 3) {
                    ret->priority  = atoi(aux[1]) - 1;
                    ret->arrival_time  = atoi(aux[2]);
                 }else if(equals(aux[0], "cpu")) {
                       ins = (instruction *)malloc(sizeof(instruction));
                       ins->type = CPU;                       
                       ins->time = atoi(aux[1]);
                       push_back(ret->instructions, ins);                       
                       //printf("cpu %s\n", aux[1]);
                 }else if(equals(aux[0], "lock")) {
                       //printf("lock %s\n", aux[1]);
                       ins = (instruction *)malloc(sizeof(instruction));
                       ins->type = LOCK;                       
                       ins->time = atoi(aux[1]);
                       push_back(ret->instructions, ins);
                 }else if(equals(aux[0], "end")) {
                       //printf("End!\n");
                       break;
                 }
           }
           merge_slices(ret);
           fclose(fd);
        //print_process(ret);
        return ret;       

}

/* Rutina para fundir quantus de tiempo */
void merge_slices(process *p) {
     node_iterator ant;
     node_iterator aux;

     instruction *ins;
     instruction *ins_next;

     ant = 0;
     aux = head(p->instructions);
     while (aux != 0 && next(aux) != 0) {
           ins = (instruction*)aux->data;
           ins_next = (instruction*)next(aux)->data;
           //printf("\tComparando %d %d con %d %d\n", ins->type, ins->time, ins_next->type, ins_next->time);
           if (ins->type == ins_next->type) { //Nodos iguales, fusionar y restar 1 al conteo de nodos
              //printf("\t\t%d %d y %d %d son iguales\n", ins->type, ins->time, ins_next->type, ins_next->time);
              ins->time = ins->time + ins_next->time;
              aux->next = next(aux)->next;
              if (aux->next != 0) {
                 aux->next->previous = aux;
              }              
              p->instructions->count = p->instructions->count - 1;
              if (aux->next == p->instructions->tail) {
                 p->instructions->tail = aux;
              }
           }else {
               aux = next(aux);
           }
     }
}

/* Rutina para re-iniciar un proceso */
void restart_process(process *p) {
     
     node_iterator n;
     instruction *it;
     
     p->resume_time = -1;
     p->waiting_time = -1;
     p->finished_time = -1;
     p->status = LOADED;
     p->thread = head(p->instructions);

     for (n = head(p->instructions); n!= 0; n = next(n)) {
         it = (instruction*)n->data;
         it->remaining = it->time;
     }     
}

/* Rutina para imprimir un proceso */
void print_process(process *p) {
     if (p == 0) {return;}
     node_iterator ptr;
     instruction *ins;
     printf("\t%s\n\tPriority: %d\n\tArrival: %d\n\tFinished: %d\n\tWaiting:%d\n", 
     p->name, p->priority, p->arrival_time, p->finished_time, p->waiting_time);
     //UNDEFINED, LOADED, READY, RUNNING, BLOCKED, FINISHED
     printf("\tStatus: %s\n", (p->status == READY)?"Ready":(p->status==LOADED)?"loaded":(p->status == BLOCKED)?"blocked":(p->status == FINISHED)?"Finished":"unknown");
     printf("Instructions: %d\n", p->instructions->count);        
     for ( ptr = head(p->instructions); ptr !=0; ptr = next(ptr)) {
           ins = (instruction *)ptr->data;
           printf("\t%s %d\n",(ins->type == CPU)?"CPU":"LOCK", ins->time);
     }
}

/* Rutina para convertir una cadena en minusculas */
char * lcase(char * s) {
     char * aux;

     aux = s;
     while (*aux != '\0' ) {
           if (isalpha(*aux) && isupper(*aux)) {
              *aux = tolower(*aux);
           }
           aux++;
     }
     return s;
}

void prepare(list * processes, priority_queue *queues, int nqueues) {
   int i;
   process *p;
   node_iterator it;

   /* Limpiar las colas de prioridad */

   for (i=0; i<nqueues; i++) {
       //printf("Clearing queue %d\n", i);
       if (queues[i].ready != 0) {
          clear_list(queues[i].ready);
          queues[i].ready = create_list();
       }
       if (queues[i].arrival != 0) {
          clear_list(queues[i].arrival);
          queues[i].arrival = create_list();
       }
       if (queues[i].finished != 0) {
          clear_list(queues[i].finished);
          queues[i].finished = create_list();
       }
   }

   /* Inicializar la informacion de los procesos en la lista de procesos */  
   
   for (it = head(processes); it != 0; it = next(it)) {
       p = (process *)it->data;
       restart_process(p);

       insert_ordered(queues[p->priority].arrival, p, compare_arrival);                    
   }

   //printf("Prepared queues:\n");
   //for (i=0; i<nqueues; i++) {
      //print_queue(&queues[i]);
   //}

   //system("pause");
      
}

/* Procesa la llegada de procesos  a una cola de prioridad */
int process_arrival(int now, priority_queue *queues, int nqueues) {
     int i;
     process *p;
     int finished;
     int total;

    //printf("Process arrival at %d\n", now);
     
     total = 0;
     for (i=0; i<nqueues; i++) {
         //printf("Queue %d\n", i);
         finished = 0;
         
         while (finished == 0) {
             p = front(queues[i].arrival); 
             if (p == 0) {
                finished = 1;
                continue;
             } //Cola vacia, pasar a la siguiente cola             
             if (p->status == LOADED) { //Es un proceso nuevo?
                if (p->arrival_time <= now) { //Es hora de llevarlo a ready?
                   p->status = READY;
                   p->waiting_time = now - p->arrival_time;
                   push_back(queues[i].ready, p);
                   pop_front(queues[i].arrival);
                }else { //Terminar de procesar esta cola
                      finished = 1;
                }                                
             }else if(p->status == BLOCKED){ //Es un proceso bloqueado?
                if (p->resume_time <= now) {
                   p->waiting_time = p->waiting_time + (now - p->resume_time);
                   p->status = READY;
                   if (queues[i].strategy == FIFO) {
                      push_front(queues[i].ready, p);
                   }else if (queues[i].strategy == RR) {
                      push_back(queues[i].ready, p);   
                   }else if (queues[i].strategy == SJF) {
                     //TODO: Logica para insertar en la cola de listos con SJF
                   }                                  
                   //printf("[%d]Resume process %s\n", now, p->name);
                   //print_queue(&queues[i]); 
                   //system("pause");
                   pop_front(queues[i].arrival);
                }else { //Terminar en esta cola
                      finished = 1;
                }    
             }else { //??
                   finished = 1;
             }
         }                 
     }       

     //Retorna el numero de procesos que se encuetran en estado de
     //listo en todas las colas de prioridad
     return get_ready_count(queues, nqueues);
}

/* Retorna el tiempo en el cual se presenta la nueva llegada a la cola 
de listos de una cola de prioridad */
int get_next_arrival(priority_queue * queues, int nqueues) {
    int ret;
    process *p;
    int i;

    ret = INT_MAX;

    for (i=0; i<nqueues; i++) {         
         //Revisar el primer proceso en la lista arrival
         //este tiene el menor tiempo de llegada.
             p = front(queues[i].arrival); 
             if (p != 0) {
                if (p->status == LOADED && p->arrival_time < ret) {
                   ret = p->arrival_time;
                }else if(p->status == BLOCKED && p->resume_time < ret) {
                   ret = p->resume_time;
                }
             }
     }

     //printf("Next arrival : %d\n", ret);

     if (ret == INT_MAX) {
        ret = -1;
     }
    
    return ret;
}

/* Retorna el numero de procesos listos en una cola de prioridad */
int get_ready_count(priority_queue *queues, int nqueues){
    int ret;
    int i;

    ret = 0;

    for (i=0; i<nqueues; i++) {         
         if (queues[i].strategy == RR) {
            ret = ret + queues[i].ready->count;
         }else if (queues[i].strategy == SJF && count_blocked(&queues[i]) == 0) {
            ret = ret + queues[i].ready->count;            
         }else if (queues[i].strategy == FIFO && count_blocked(&queues[i]) == 0) {
            ret = ret + queues[i].ready->count;            
         }
             
     }

    return ret;
}

/* Cuenta el numero de procesos bloqueados de una cola de prioridad*/
int count_blocked(priority_queue *queue) {
    node_iterator n;
    process *p;

    int ret;
    
    ret = 0;

    for (n=head(queue->arrival); n!=0; n = next(n)) {
        p = (process*)n->data;
        if (p->status == BLOCKED) {
           ret = ret + 1;
        }
    }
    return ret;
}

/* Rutina para la planificacion.*/
void schedule(list * processes, priority_queue *queues, int nqueues) {
    
   int finished;
   int nprocesses;
    
   //Preparar para una nueva simulacion
   //Inicializar las colas de prioridad con la informacion de la lista
   //de procesos leidos
   prepare(processes, queues, nqueues);

   //tiempo_actual = MINIMO TIEMPO DE LLEGADA DE TODOS LOS PROCESOS
   int tiempo_actual = 0;
   node_iterator it = head(processes);
   process * p_init = (process *)it->data;
   for (it; it !=0; it = next(it)){
      process * it_p = (process *)it->data;
      if (it_p->arrival_time == p_init->arrival_time && p_init->priority > it_p->priority)
      {
         p_init = it_p;
      }
   }
   tiempo_actual = p_init->arrival_time;
   int varEsColaEncontrada=0;

   //cola_actual = COLA EN LA CUAL SE ENCUENTRA EL PROCESO CON TIEMPO = tiempo_actual
   priority_queue * current_queue;
   for (int i = 0; i < nqueues; i++)
   {
      priority_queue * cola_it = &queues[i];
      node_iterator ptr;
      for ( ptr = head(cola_it->arrival); ptr !=0; ptr = next(ptr)) {

         if (strcmp(ptr->data,p_init->name)==0)
         {
            current_queue = cola_it;
            varEsColaEncontrada=1;
            break;
         }
      }
      if (varEsColaEncontrada==1)
      {
         break;
      }      
   }

   //proceso_actual = NULO
   process * proceso_actual=NULL;

   //Numero de procesos que falta por ejecutar     
   nprocesses = processes->count;
   
   while (nprocesses > 0) {

      //TODO: Implementar la planificación      
      for (node_iterator it = head(processes); it !=0; it = next(it)){
         process * p = (process *)it->data;
         if (p->status == LOADED && p->arrival_time <= tiempo_actual)
         {
            p->status = READY;
            //print_process(p);
         }
         
      }
      if (proceso_actual!=NULL)
      {
         proceso_actual->status=READY;
         //poner al proceso_actual en su cola correspondiente,
         //de acuerdo con el algoritmo de planificacion de la cola
      }
      
      //Cuando un proceso termina, decrementar nprocesses.
      nprocesses = nprocesses - 1;
      //El ciclo termina cuando todos los procesos han terminado,
      //es decir nprocesses = 0 
   }
   
    
    //Imprimir la salida del programa
    printf(" -----------------------------------------------------------------------------------\n");
    printf("||                      *****Resultados de la simulación*****                      ||\n");
    printf("|| Colas de prioridad           : %d                                                ||\n",nqueues);
    printf("|| Tiempo total de la simulación:                                                  ||\n");
    printf("|| Tiempo promedio de espera    :                                                  ||\n");
    printf("|| #Proceso       T.Llegada      Tamaño        T.Espera       T.Finalizacion       ||\n");
    printf("|| ------------------------------------------------------------------------------- ||\n");
    printf("|| Secuencia de ejecucion:                                                         ||\n");
    printf(" -----------------------------------------------------------------------------------\n");
}

void update_waiting_time(list * processes, int t) {
  node_iterator it; 
  for (it = head(processes); it != 0; it = next(it)) {
    process * p = (process *)it->data;
    if (p->status == READY) {
      p->waiting_time += t;    
    }
  }
}
