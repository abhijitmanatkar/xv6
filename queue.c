#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

node* alloc_node(){
    for(int i = 0; i<64; i++){
        if(NODES[i].used)
            continue;
        NODES[i].used = 1;
        return &NODES[i];
    }
    return 0;
}

void free_node(node* n){
    n->used = 0;
}

node* new_node(struct proc* p){
    node* new = alloc_node();//(node*) malloc(sizeof(node));
    new->p = p;
    new->next = 0;
    new->prev = 0;
    return new;
}

void delete_node(node* n){
    n->p = 0;
    n->next = 0;
    n->prev = 0;
    free_node(n);
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

    while(trav != 0 && trav->p != p)
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

void init_nodes(){
    for(int i = 0; i<64; i++)
        NODES[i].used = 0;
}