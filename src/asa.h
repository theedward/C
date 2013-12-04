#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <limits>
#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))
// #define NILL 0
#define INF std::numeric_limits<int>::max()
#define MAX 100000
std::vector<int> grafo[MAX];
int numAlunos=0, numEmpregos=0;
//std::vector<std::list<int> >  *grafo;
int NILL;

bool BFS(int* pairG1, int* pairG2,int numAlunos,int numEmpregos, int* dist){
      std::cout << "«««««««««« BFS: " << std::endl;

	int nill_pos = NILL;
	/*function BFS ()
	    for v in G1
	        if Pair_G1[v] == NIL
	            Dist[v] = 0
	            Enqueue(Q,v)
	        else
	            Dist[v] = ∞
	    Dist[NIL] = ∞
	    while Empty(Q) == false
	        v = Dequeue(Q)
	        if Dist[v] < Dist[NIL]
	            for each u in Adj[v]
	                if Dist[ Pair_G2[u] ] == ∞
	                    Dist[ Pair_G2[u] ] = Dist[v] + 1
	                    Enqueue(Q,Pair_G2[u])
	    return Dist[NIL] != ∞*/
	
	std::queue<int> queue;
// 	int v;
	for(int av = 0; av< numAlunos; av++){
	  			

		if(pairG1[av] == NILL){
			dist[av]=0;
			queue.push(av);
			//dist[i] = 0;
		}
		else{
			dist[av]=INF;
		}
	  std::cout << " BFS : for av " << av << " --- pairG1[v]: " << pairG1[av] << " -- dist[v] " << dist[av] << std::endl;
	}
		  std::cout << "BFS : queue.size() " << queue.size() << std::endl;

	dist[nill_pos] = INF;
	while(!queue.empty()){
		int v = queue.front();
		queue.pop();

		if( dist[v] < dist[nill_pos]){
			std::cout << "BFS : int v = queue.front(); " << queue.front() << " ---------------- dist[v] < dist[nill_pos] " << dist[v] << " < " << dist[nill_pos] << std::endl;

			int len = grafo[v].size();
			for(int j=0; j< len; j++){
				int u = grafo[v][j];
				std::cout << "BFS: u " << u << " --- pairG2[u]: " << pairG2[u] << " -- v:  "  << v << " dist[v]+1 " << dist[v]+1 <<"   --  dist[pairG2[u]] --   " <<dist[pairG2[u]] << std::endl;
				if(dist[pairG2[u]] == INF){
					dist[pairG2[u]] = dist[v]+ 1;
					queue.push(pairG2[u]);
				}
			}
		}
	}
	std::cout << " BFS : INF " << INF << " --------- !=  ------- dist[nill_pos] " << dist[nill_pos] << std::endl;
	return dist[nill_pos] != INF;
}

void print(int *i, std::string s){
  int size = NELEMS(i);
  std::cout << " "<< s << ":" << std::endl;
  for(int j = 0; j< size; j++)
    std::cout << " j "<< j << " " << i[j] << " --- ";
  std::cout << std::endl; 
}

bool DFS(int v, int * dist, int* pairG1, int* pairG2){
  std::cout << " ++++++++++ DFS: v: " << v << std::endl;
	/*function DFS (v)
	    if v != NIL
	        for each u in Adj[v]
	            if Dist[ Pair_G2[u] ] == Dist[v] + 1
	                if DFS(Pair_G2[u]) == true
	                    Pair_G2[u] = v
	                    Pair_G1[v] = u
	                    return true
	        Dist[v] = ∞
	        return false
	    return true*/
	if(v != NILL){
		int len = grafo[v].size();
		std::cout << ": v " << v << " --- grafo[v].size(): " << grafo[v].size() << std::endl;
		for(int j=0; j< len; j++){
			int u = grafo[v][j];
			std::cout << " u " << u << " --- pairG2[u]: " << pairG2[u] << " -- v:  "  << v << " dist[v]+1 " << dist[v]+1 << std::endl;
			if(dist[pairG2[u]] == dist[v]+1){
			  std::cout << " ¨¨¨¨¨¨¨¨¨¨¨   ¨¨¨¨¨¨¨¨¨¨  u " << u << " --- pairG2[u]: " << pairG2[u] << " -- v:  "  << v << std::endl;
				if(DFS(pairG2[u], dist, pairG1, pairG2)){
					pairG2[u] = v;
					pairG1[v] = u;
					return true;
				}
			}
		dist[v] = INF;
		return false;
		}
	}
	return true;

}
int Hopcroft_Karp(int numAlunos, int numEmpregos, int* pairG1, int* pairG2){
	int dist[numAlunos+numEmpregos+1]; //+1 para o nill
	/* function Hopcroft-Karp
	    for each v in G
	        Pair_G1[v] = NIL
	        Pair_G2[v] = NIL
	    matching = 0
	    while BFS() == true
	        for each v in G1
	            if Pair_G1[v] == NIL
	                if DFS(v) == true
	                    matching = matching + 1
	    return matching*/
	int matching = 0;
	/*Inicializa os vectores*/
	std::cout << "Inicializa os vectores"  << std::endl;
// 	int size =  NELEMS(pairG1);
	for(int i=0; i < numAlunos /*+1 = Nill*/ ;i++){
		pairG1[i]=NILL;
		std::cout << "????????????? i " << i << " --- pairG1[i]: " << pairG1[i] << " --- numAlunos: " << numAlunos<< std::endl;
	}
	
	std::cout << " numEmpregos" << numEmpregos << std::endl;
	for(int i=0; i < numEmpregos; i++){
		pairG2[i]= NILL;
		std::cout << " i ?????????????  " << i << " --- pairG2[i]: " << pairG2[i] << " --- numEmpregos: " << numEmpregos <<std::endl;
	}
	//Corre o algoritmo
	while (BFS(pairG1, pairG2, numAlunos, numEmpregos, dist)){
		std::cout << " while _______________________________________________________________%$$%" << std::endl;
		for(int v =0; v< numAlunos ;v++ ){
		  print(pairG1, "pairG1");
		std::cout << "pairG1 v " << v << " --- numAlunos: " << numAlunos <<std::endl;

			if(pairG1[v]==NILL){
				std::cout << "DFS begin ............................................................ %$$%" << std::endl;

				if(DFS(v , dist, pairG1, pairG2)){
					matching = matching+1;
				}
				std::cout << " DFS end ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;%$$%" << std::endl;

			}
		}
		std::cout << " while ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||%$$%" << std::endl;

	}
	return matching;
}
int main(){
	
	scanf("%d %d",&numAlunos, &numEmpregos);
	NILL = numAlunos+numEmpregos;
	//printf("Numero de alunos %d  numero Empregos  %d", numAlunos, numEmpregos);
	int pairG1[numAlunos];
	int pairG2[numEmpregos];
	for(int i = 0; i < numAlunos; i++){
		int numProps = 0;
		scanf("%d", &numProps);
		for(int j = 0; j < numProps; j++){
			//printf("O numero de empregos é %d\n", numProps);
			int propostaEmprego;
			scanf("%d", &propostaEmprego);
			grafo[i].push_back(propostaEmprego);
// 			grafo[propostaEmprego+1+numAlunos].push_back(i);
			/*grafo[i+1].push_back(propostaEmprego+1+numAlunos);
			grafo[numAlunos+propostaEmprego+1].push_back(i+1);*/

		}
		scanf("\n");

	}
	printf("%d\n",Hopcroft_Karp(numAlunos, numEmpregos, pairG1, pairG2) );
// 	int resultado = Hopcroft_Karp(numAlunos, numEmpregos, pairG1, pairG2);
// 	printf("%d\n", resultado);
	return 0;
}
