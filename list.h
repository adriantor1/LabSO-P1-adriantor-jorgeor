#ifndef _TYPELIST_H
#define _TYPELIST_H
/*
 Libreria que implementa funciones de listas enlazadas en forma 
 de funciones inline
 Derechos Reservados de Erwin Meza Vega <emezav@gmail.com>.
*/

typedef struct node {                                               
       void * data;                                                      
       struct node *next;                                   
       struct node *previous;                               
}node; 


typedef node * node_iterator;                                                                                                                                                                              
                                                                                               
typedef struct list {                                               
       node *head;  
       node *tail;                                              
       int count;                                                              
} list;                                                                            
                  
static __inline__ list * create_list() {
       list *ret;

       ret = (list *) malloc(sizeof(list));

       ret->head = 0;
       ret->tail = 0;
       ret->count = 0;
       return ret;
}
                                                                             
static __inline__ node *                                        
       create_node(void * data) {                         
         node * ret;                                            
                                                                               
         ret = (node *)malloc(sizeof(node));     
         ret->data = data;                                                     
         ret->next = 0;                                                     
         ret->previous = 0;                                                 
                                                                               
         return ret;                                                           
}                                                                              
                                                                               
static __inline__ void clear_list(list *l) { 
     if (l ==0) {return;}      
     node *aux, *ptr;

     ptr = l->head;

     //printf("Clearing list...\n");
     while (ptr != 0) {
           aux = ptr;
           ptr = ptr->next;
           free(aux);
           aux = NULL;
     }

     //printf("Finished clearing list\n");
     
     l->head = 0;                                                           
     l->tail = 0;                                                           
     l->count = 0;                                                             
} 

static __inline__ void * insert_ordered(list *l, void * data,
                                int (*compare)  (const void *, const void *)) {

    node * n;
    node * aux, *ant;
    if (l ==0) { return 0;}
    n = create_node(data);

    if (l->head == 0) { /*Primer elemento en la lista  */                
          l->head = n;                                                         
          l->tail = n;                                                         
          l->count = 0;                                                        
    }else {                                                                 
          ant = 0;
          aux = l->head;
          while (aux != 0 && compare(data, aux->data) <= 0) {
              ant = aux;
              aux = aux->next;
          }
          
          n->previous = ant;

          if (ant == 0) { //Primer nodo de la lista
             n->next = l->head;
             l->head->previous = n;
             l->head = n;
          }else {
              ant->next = n;
              n->next = aux;
              if (aux == 0) { //Final de la lista
                 l->tail = n;                
              }else {
                    aux->previous = n;
              }
          }          
                                                                                                                         
    }                                                                       
                                                                               
    l->count++;                                                             
    //printf("%s",data);                                                           
    return data;
}
                                                                            

                                                                            
static __inline__ void *                                        
       push_front(list *l, void * data) {                               
       node * n;             

       if (l ==0) {return 0;}
                                   
       n = create_node(data);                                   
                                                                                                                                                              
       if (l->head == 0) { /*Primer elemento en la lista  */                
          l->head = n;                                                         
          l->tail = n;                                                         
          l->count = 0;                                                        
       }else {                                                                 
          n->next = l->head;                                                   
          l->head->previous = n;                                               
          l->head = n;                                                         
       }                                                                       
                                                                               
       l->count++;                                                             
                                                                               
       return data;                                                               
}                                                                              
                                                                               
static __inline__ void *                                        
       push_back(list *l, void * data) {                                
       node * n;                                                

       if (l ==0) {return 0;}
       
       n = create_node(data);                                   
                                                                               
       if (l->tail == 0) { /*Primer elemento en la lista */                 
          l->head = n;                                                         
          l->tail = n;                                                         
          l->count = 0;                                                        
       }else {                                                                 
          n->previous = l->tail;                                               
          l->tail->next = n;                                                   
          l->tail = n;                                                         
       }                                                                       
       l->count++;                                                             
                                                                               
       return n;                                                               
}                                                                              
                                                                               
                                                                               
static __inline__ void *                                        
       front(const list *l) {                          
        if (l == 0) {return 0;}
        if (l->head == 0) {return 0;}
        return l->head->data;                                                        
}                                                                              
                                                                               
static __inline__ void *                                        
       back(list *l) {   
       if (l == 0) {return 0;}
       if (l->tail == 0) {return 0;}                        
       return l->tail->data;                                                   
}                                                                              
                                                                               
static __inline__ void *                                        
       pop_front(list *l) {                      
       node *ret;                                               

       if (l == 0) {return (void*)0;}
                                                                               
       ret = l->head;
                                                          
       if (l->head == 0) {return (void*)0;}                                      
                                                                               
       l->head = l->head->next;                                                
                                                                               
       if (l->head == 0) {                                                  
          l->tail = 0;                                                      
       }else {                                                                 
          l->head->previous = 0;                                            
       }                                                                       
                                                                               
       l->count--;                                                             
                                                                               
       return ret->data;                                                             
}                                                                              
                                                                               
static __inline__ void *                                        
       pop_back(list *l) {                       
       node *ret;                                               
                  
       if (l ==0) {return 0;}
                                                                              
       ret = l->tail;                                                          
                                                                               
       if (ret == 0) {return ret;}                                      
                                                                               
       l->tail = l->tail->previous;                                            
                                                                               
       if (l->tail == 0) { /* Un solo elemento? */                          
          l->head = 0;                                                      
       }else {                                                                 
          l->tail->next = 0;                                                
       }                                                                       
                                                                               
       l->count--;                                                             
                                                                               
       return ret->data;                                                             
}

static __inline__ int empty(list *l) {
       return ((l== 0) || l->head==0);
}

static __inline__ node_iterator head(list *l) {
       if (l ==0) {return 0;}
       return l->head;
}

static __inline__ node_iterator tail(list *l) {
       if (l ==0) {return 0;}
       return l->tail;
}


static __inline__ node_iterator next(node_iterator it) {
       if (it == 0) {return 0;}
       return it->next;
}

static __inline__ node_iterator previous(node_iterator it) {
       if (it == 0) {return 0;}
       return it->previous;
}


#endif
