/* Luis Pedro Oliveira Ferreira Nr. 83500 */

#include <stdio.h>
#include <stdlib.h> 

typedef struct Vertex{
    struct Edge *adj; /*Adjacent List*/
    int h; /*height*/
    int e; /*excess flow*/
    int id;
    int isMinCut;
    int isVisited; /*For DFS*/
    struct Node *qNode; /*Node for Relabel Q*/
    int inQ; /*boolean to know if v is in Q*/
}Vertex;

typedef struct Edge{
    Vertex *u;
    Vertex *v;
    int flow;
    int cap;
    struct Edge *next; /*pointer for next edge in adj list*/
    struct Edge *vu; /*pointer to parallel edge*/
    int isMinCut;
}Edge;

typedef struct Node{ /*Struct for Relabel Q*/
    Vertex *v;
    struct Node *next;
}Node;

Vertex *graph;
int nr_vertex;
int change; /*value to add to Vin to know Vout */
int nr_fornecedores;

int minimo(int a,int b){
	if(a > b){return b;}
	return a;
}
        
Node* createNode(Vertex *v){
    Node *node=malloc(sizeof(Node));
    node->v=v;
    node->next=NULL;
    return node;
}

Node* addToFifo(Node *last, Node *node){
    node->v->inQ=1;
    last->next=node;
    node->next=NULL;
    return node;
}

Node *rmvFromFifo(Node *head){
    head->v->inQ=0;
    Node *temp=head->next;
    head->next=NULL;
    return temp;
}
    

void addEdge(int u, int v, int cap){ /*Adds both uv and vu*/
    Edge *edge=malloc(sizeof(Edge)); 
    Edge *inverse=malloc(sizeof(Edge)); 
    
    edge->u=&graph[u];
    edge->v=&graph[v];
    
    inverse->u=&graph[v];
    inverse->v=&graph[u];

    edge->vu=inverse;
    inverse->vu=edge;
    
    if(u==0){ /*if adding vertex from source, start flooding */
        edge->flow=cap;
        inverse->flow=-cap;
        graph[v].e=cap;
        graph[0].e-=cap;
    }
    else{
        edge->flow=0;
        inverse->flow=0;
    }    
    
    edge->cap=cap;
    inverse->cap=0;
    
    edge->isMinCut=0;
    inverse->isMinCut=0;
    
    edge->next=graph[u].adj;
    inverse->next=graph[v].adj;
    
    graph[u].adj=edge;
    graph[v].adj=inverse;
}
    
/*Move maximo fluxo possivel atravez do arco uv. Apply if u.e>0, uv.cap>uv.flow and u.h>v.h */
void push(Edge *uv){
    int f=minimo(uv->u->e, (uv->cap - uv->flow));
    
    uv->flow+=f;
    uv->vu->flow-=f;
    
    uv->u->e-=f;
    uv->v->e+=f;
}

/*Aumenta h de um vertice. Apply if v.e>0 and v.h<u.h for all edges in residual graph */
void relabel(Vertex *v){ 
    Edge *uv=v->adj;
    int min=-1;
    while(uv!=NULL){
        if(uv->cap > uv->flow){
            if(min==-1){
                min=uv->v->h;
            }
            else{
                min=minimo(min,uv->v->h);
            }
        }
        uv=uv->next;
    }
    if(min!=-1){
        v->h=min+1;
    }
}

Node* discharge(Vertex *v, Node *last){
    Edge *current=v->adj;
    while(v->e > 0 ){
        if(current==NULL){
            relabel(v);
            current=v->adj;
        }
        else if(current->cap > current->flow && v->h == current->v->h+1){
            push(current);
            if(current->v->inQ==0 && current->v->id>1){ /*vertice para o qual fez push fica com excesso de fluxo, logo adiciona a queue caso nao seja source nem sink*/
               last=addToFifo(last,current->v->qNode);
            }
        }
        else{
            current=current->next;
        }
    }
    return last;
}
  
void relabelToFront(){
    int i;
    Node *queue=graph[2].qNode;
    Node *last=queue;

    for(i=2;i<nr_fornecedores+2;i++){ /*initalizes queue with providers, which are the vertices with excess flow*/
        last=addToFifo(last, graph[i].qNode);
    }
        
    while(queue!=NULL){
        last=discharge(queue->v, last);
        queue=rmvFromFifo(queue);
    }
}

void DFS(Vertex *v){
    v->isVisited=1;
    Edge *e= v->adj;
    while(e!=NULL){
        if(!e->v->isVisited && e->vu->cap > e->vu->flow){ /*If Inverse Edge is Saturated, it isnt in residual graph */
            DFS(e->v);
        }
        e=e->next;
    }
}

void findMinCut(){
    
    /*DFS no grafo transposto da rede residual a partir do hipermercado. Os arcos entre um vertice visitado e um nao visitado correspondem ao corte minimo mais proximo do hiper */
    DFS(&graph[1]);
    
    /*Depois da DFS, verifica vertices vizinhos com isVisited diferentes */
    int i;
    for(i=0;i<nr_vertex;i++){
        Edge *e=graph[i].adj;
        while(e!=NULL){
            if(graph[i].isVisited!=e->v->isVisited && e->flow==e->cap){ /*segunda condicao e necessaria caso o Vout tenha ligacao para outro fornecedor e o corte minimo seja vertice */
                if(e->v->id==1 || i==1){                   /*Arco pra hipermercado */
                    e->isMinCut=1;
                }
                else if(abs(e->v->id-i==change)){        /*Restricao na capacidade de um abastecimento, o valor mais pequeno e o vertice inicial. (Tambem identifica limitacao dos fornecedores) */
                    graph[minimo(e->v->id,i)].isMinCut=1;
                }
                else{                                    /*Restricao de um arco */
                    e->isMinCut=1;
                }
            }
            e=e->next;
        }
    }
}        

int main(){
     /* Como vertices tambem tem capacidades, cada vertice vai ser dividido em 2, o vertice i (Vin) e o vertice (i+f+e) (Vout), em que existe um arco entre eles com capacidade igual a capacidade do vertice, sendo assim um problema classico de fluxos maximos. 
    Assim, 2 a f+1 sao fornecedores (entrada), 2+f+e a 2+2f+e sao fornecedores(saida), 1 e o Hiper, 0 e a source. */
    int i,j, capacity;
    int origem, destino;
    int nr_abastecimento, nr_ligacoes;

    scanf("%d %d %d",&nr_fornecedores, &nr_abastecimento, &nr_ligacoes);
    nr_vertex=(nr_fornecedores+nr_abastecimento+1)*2;  /*For each vertex has Vin e Vout, hiper with source */
    
    change=nr_fornecedores+nr_abastecimento;
    
    graph=malloc(nr_vertex*sizeof(Vertex)); 
    for(i=0;i<nr_vertex;i++){
        graph[i].adj=NULL;
        graph[i].id=i;
        graph[i].h=0;
        graph[i].e=0;
        graph[i].isMinCut=0;
        graph[i].isVisited=0;
        graph[i].qNode=createNode(&graph[i]);
        graph[i].inQ=0;
    }
    graph[0].h=nr_vertex;
    
    for(i=0; i < nr_fornecedores; i++){
        scanf("%d", &capacity);
        addEdge(i+2, i+2+change, capacity); /*(source id, finish id, capacity)*/
        addEdge(0,i+2,capacity+1); /*add edge to source*/
    }
    
    for(i=0; i < nr_abastecimento; i++){
        scanf("%d", &capacity);
        addEdge(i+2+nr_fornecedores, i+2+nr_fornecedores+change, capacity); /*(source id, finish id, capacity)*/
    }
    
    for(i=0; i < nr_ligacoes; i++){
        scanf("%d %d %d", &origem, &destino, &capacity);
        addEdge(origem+change, destino, capacity);
    }
    
    relabelToFront();
    findMinCut();

    int isFirst=1;
    printf("%d\n",graph[1].e);
    for(i=2+nr_fornecedores;i<2+nr_fornecedores+nr_abastecimento;i++){
        if(graph[i].isMinCut==1){
            if(!isFirst){
                printf(" ");
            }
            isFirst=0;
            printf("%d",i);
        }
    }
    printf("\n");
    
    int *minCut=malloc(nr_vertex*sizeof(int)); /*to Sort edges*/
    
    for(i=1;i<nr_vertex;i++){
        Edge *e=graph[i].adj;
        while(e!=NULL){
            if(e->cap>0 && e->isMinCut==1){
                minCut[e->v->id]=1;
            }
            e=e->next;
        }
        for(j=0;j<nr_vertex;j++){
            if(minCut[j]==1){
                printf("%d %d\n",i-change, j);
            }
            minCut[j]=0;
        }
    }
    return 0;
}

    
    
