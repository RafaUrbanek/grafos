Rafael Urbanek Laurentino GRR:20224381
Vitor Lorenzo Cumim GRR:20224757

Trabalho de Implementação

Descrição: Este projeto implementa uma biblioteca em C para manipulação e análise de grafos não direcionados e ponderados. Inclui funcionalidades para leitura, análise estrutural e identificação de propriedades relevantes do grafo, como componentes conexas, bipartição, diâmetro, vértices e arestas de corte.

Estruturas de Dados:
	-grafo: Estrutura principal que armazena a lista de vértices, nome do grafo, número de vértices e arestas.
	-vértice: Cada vértice armazena seu nome, lista de vizinhos, status de visitação (usado em buscas), cor (para bipartição), além de variáveis auxiliares para algoritmos de DFS.
	-vizinho: Lista ligada de vizinhos para cada vértice, contendo ponteiro para o vértice vizinho e peso da aresta.

Funções utilitárias internas (static)
Essas funções são usadas somente dentro do grafo.c.

	1- busca_vertice
	Procura um vértice pelo nome na lista de vértices do grafo. Retorna ponteiro para o vértice se encontrado, senão retorna NULL.

	2- adiciona_vertice
	Adiciona um novo vértice ao grafo. Se o vértice já existe, retorna ele; senão, aloca e inicializa o novo vértice.

	3- adiciona_aresta
	Adiciona uma aresta entre dois vértices (bidirecional) com um peso especificado.

	4- dfs_componentes
	Faz uma busca em profundidade (DFS) a partir de um vértice, marcando todos os vértices do mesmo componente com uma cor.

Funções auxiliares para diâmetro:
	1- dijkstra
	Implementa o algoritmo de Dijkstra para encontrar as menores distâncias de um vértice inicial a todos os outros.

	2- compara_ints
	Função de comparação usada para ordenar inteiros (diâmetros).

	3- indice_de
	Retorna o índice de um vértice em um vetor de ponteiros para vértices.

Funções para vértices e arestas de corte:
	1- dfs_corte
	Implementa uma DFS modificada para encontrar vértices de corte (articulation points) em um grafo não direcionado.
	Marca o tempo de descoberta e o menor tempo atingível (low), determinando se um vértice é crítico.

	2- vertices_corte
	Inicializa os vértices, executa dfs_corte para todos os vértices não visitados, armazena e ordena os nomes dos vértices de corte e retorna uma string com os nomes em ordem 	alfabética.

	3- dfs_aresta_corte
	Implementa DFS para identificar arestas de corte (pontes), analisando se a remoção de uma aresta aumenta o número de componentes conexas.

	4- arestas_corte
	Inicializa os vértices, executa dfs_aresta_corte, agrupa pares de vértices representando as pontes, ordena e retorna uma string formatada com as arestas de corte em ordem 	alfabética.


Observações Técnicas:
	Todas as funções públicas são declaradas em grafo.h e implementadas em grafo.c
	As funções de leitura assumem que o arquivo está corretamente formatado conforme especificado no cabeçalho (grafo.h).
	A biblioteca utiliza algoritmos clássicos para análise de grafos, como DFS para componentes e vértices de corte, BFS para bipartição e Dijkstra para cálculo de diâmetro.