#include "proc.h"

node* new_node(struct proc* p){
    node* new = (node*) malloc(sizeof(node));
    new->p = p;
    new->next = 0;
    new->prev = 0;
    return new;
}

void delete_node(node* n){
    n->p = 0;
    n->next = 0;
    n->prev = 0;
    free(n);
}

node* push_proc(node* head, struct proc* p){
    
    node* new = new_node(p);

    if(head == 0)
        return new;

    node* trav = head;
    while(trav->next != 0)
        trav = trav->next;
    
    new->prev = trav;
    trav->next = new;

    return head;
}

node* pop_proc(node* head){
    if(head == 0){
        return head;
    }
    node* temp = head;
    head = head->next;
    if(head != 0)
        head->prev = 0;
    delete_node(temp);
    return head;
}

node* del_proc(node* head, struct proc* p){
    node* trav = head;

    while(trav->p != p && trav != 0)
        trav = trav->next;

    if(trav == 0)
        return head;
    
    node* tprev = trav->prev;
    
    if(tprev == 0){
        if(trav->next != 0)
            (trav->next)->prev = tprev;
        head = trav->next;
        delete_node(trav);
        return head;
    }

    if(trav->next != 0)
        (trav->next)->prev = tprev;
    tprev->next = trav->next;
    delete_node(trav);
    return head;
}

void init_queues(){
    for(int i = 0; i < 5; i++){
        QUEUES[i] = 0;
    }
}
