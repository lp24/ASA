/* Luis Pedro Oliveira Ferreira Nr. 83500 */

#include <stdio.h>
#include <stdlib.h> 

typedef struct Vertex{
    struct Node *adj; /*Adjacent List*/
    int id;
    /*for DFS*/
    int prev;
    int discovery;
    int colour; /*0 white, 1 grey, 2 black. Tambem guarda a sub-rede a que pertence*/
	/*for articulation points*/
	int low;
	int is_ap;
}Vertex;

typedef struct Node{
    Vertex *data; 
    struct Node *next;
}Node;

Node *add_element(Node *head, Vertex *v){ /* adds at the start of the list, returns pointer to added node */   
    Node *node=malloc(sizeof(Node));
    if(node==NULL){
        fprintf(stderr,"\nErro ao alocar memoria\n");
        return NULL; 
    }   
    node->next=head;
    node->data=v;
    return node;
}

int min(int a,int b){
	if(a > b){return b;}
	return a;
}
int maximo(int a,int b){
	if(a > b){return a;}
	return b;
}

void DFS();
void dfs_visit(Vertex *v);
void DFS_ap();
void dfs_visit_ap(Vertex *v);

Vertex *graph;

int sub_redes=0;
int nr_vertex;
int time=1;
int ap=0; /*number articulation points*/
int max=0; /*maximo nr de vertices de uma sub-rede removendo os aps*/
int current=0; /*contador para nr de vertices atuais da sub-rede*/

int main(){
    int nr_edges;
    int v, u, i;    
  
    scanf("%d",&nr_vertex);      
    if (nr_vertex<2){
        fprintf(stderr, "Argumentos inválidos");
        return -1;
    }
    
    graph=malloc(nr_vertex*sizeof(Vertex));    
    if(graph==NULL){
        fprintf(stderr,"\nErro ao alocar memoria\n");
        return -1; 
    }
    
    for(i=0;i<nr_vertex;i++){ /*inicializar Vertices*/
        graph[i].id=i+1;
		graph[i].prev=-1;
        graph[i].colour=0; /*2 black, 1 grey, 0 white*/
        graph[i].adj=NULL;
		graph[i].is_ap=0;
    }
    
    scanf("%d",&nr_edges); /*Ler arcos*/
    
    if (nr_edges<1){
        fprintf(stderr, "Argumentos inválidos");
        return -1;
    }
    
    for(i=0; i<nr_edges; i++){ /*Adicionar Arcos ao grafo*/
        scanf("%d %d",&v, &u);
        graph[v-1].adj= add_element(graph[v-1].adj,&graph[u-1]);
        graph[u-1].adj= add_element(graph[u-1].adj,&graph[v-1]);
    }
    
    DFS();  
	  
    printf("%d\n", sub_redes);
    for(i=0;i<nr_vertex;i++){
        if(graph[i].colour>1){
            if(graph[i].colour!=1+sub_redes){
                printf(" ");
            }
            printf("%d",graph[i].id);
        }
        graph[i].colour=0;
    }
    
    DFS_ap();

	printf("\n%d\n%d\n", ap, max);
        
    return 0;
}

void DFS(){
	int i;
    for(i=nr_vertex-1;i>=0;i--){
        if(graph[i].colour==0){
            dfs_visit(&graph[i]);
            sub_redes++;
            graph[i].colour=1+sub_redes;
        }
    }
}

void dfs_visit(Vertex *v){
	int children=0;
    v->colour=1;
    v->discovery=time;
	v->low= ++time;
    Node *adj=v->adj;
    while(adj!=NULL){
        if(adj->data->colour==0){
            adj->data->prev=v->id;	
			children++;
            dfs_visit(adj->data);			
			v->low=min(v->low , adj->data->low);
			/*Articulation point if root and more than 1 children, or child with low{child)>discover(v)*/
            if( (v->prev == -1 && children>1) || (v->prev != -1 && adj->data->low >= v->discovery) ){
			   v->is_ap=1;
			}   
        }   
        else if (adj->data->id != v->prev){
			v->low  = min(v->low, adj->data->discovery);
        }
        adj=adj->next;
    }
    time++;
}

void DFS_ap(){
    int i;
    for(i=0;i<nr_vertex;i++){
		if(graph[i].is_ap==1){ap++;}
        else if(graph[i].colour==0){
			current=0;
            dfs_visit_ap(&graph[i]);
			max=maximo(max, current);

        }
    }
}

void dfs_visit_ap(Vertex *v){
    v->colour=1;
	current++;
    Node *adj=v->adj;
    while(adj!=NULL){
        if(adj->data->colour==0 && adj->data->is_ap==0){
            dfs_visit_ap(adj->data);
        }
        adj=adj->next;
    }
} 
