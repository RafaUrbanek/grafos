#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

//------------------------------------------------------------------------------
// Máximo de caracteres em uma linha da entrada de dados.

#define MAX_LINHA 2047
#define BUFFER_SIZE 50
#define INF 1000000000

//------------------------------------------------------------------------------
// Estrutura de dados para representar os vizinhos de um vertices.

typedef struct vizinho {
    struct vertice *v;
    struct vizinho *proximo;
    int peso;
} vizinho;

//------------------------------------------------------------------------------
// Estrutura de dados para representar um vértice de um grafo.

typedef struct vertice {
    char *nome;
    vizinho *vizinhos;
    struct vertice *proximo;
    int visitado;
    int cor;
    int discovery, low, parent;
} vertice;

//------------------------------------------------------------------------------
// Estrutura de dados para representar um grafo.

struct grafo {
    char *nome;
    vertice *vertices;
    int n_vertices;
    int n_arestas;
};

//------------------------------------------------------------------------------
// Função utilitária para verificar se o vertice já existe.

static vertice *busca_vertice(grafo *g, const char *nome) {
    for (vertice *v = g->vertices; v != NULL; v = v->proximo) {   
        if (v != NULL) {
            if (strcmp(v->nome, nome) == 0)
                return v;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
// Funçãos utilitária para adicionar um vértice no grafo.

static vertice *adiciona_vertice(grafo *g, const char *nome) {
    vertice *v = busca_vertice(g, nome);
    if (v) return v;

    v = malloc(sizeof(vertice));
    v->nome = (char *)malloc(strlen(nome) + 1);
    strcpy(v->nome, nome);
    v->vizinhos = NULL;
    v->visitado = 0;
    v->cor = -1;
    v->discovery = -1;
    v->low = -1;
    v->parent = -1;
    v->proximo = g->vertices;
    g->vertices = v;
    g->n_vertices++;
    return v;
}

//------------------------------------------------------------------------------
// Funçãos utilitária para adicionar uma aresta no grafo.

static void adiciona_aresta(grafo *g, vertice *v1, vertice *v2, int peso) {
    vizinho *n1 = malloc(sizeof(vizinho));
    n1->v = v2;
    n1->peso = peso;
    n1->proximo = v1->vizinhos;
    v1->vizinhos = n1;

    vizinho *n2 = malloc(sizeof(vizinho));
    n2->v = v1;
    n2->peso = peso;
    n2->proximo = v2->vizinhos;
    v2->vizinhos = n2;

    g->n_arestas++;
}

//------------------------------------------------------------------------------
// leitura do grafo.

grafo *le_grafo(FILE *f) {
    char linha[MAX_LINHA];
    grafo *g = malloc(sizeof(grafo));
    g->nome = NULL;
    g->vertices = NULL;
    g->n_vertices = 0;
    g->n_arestas = 0;

    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '/' && linha[1] == '/') continue;

        char *p = strchr(linha, '\n');
        if (p) *p = '\0';

        char tmp1[BUFFER_SIZE], tmp2[BUFFER_SIZE];
        int peso = 1;

        char *v1;
        v1 = (char *)malloc(BUFFER_SIZE);
        char *v2;
        v2 = (char *)malloc(BUFFER_SIZE);
       
        if (sscanf(linha, "%s -- %s %d", tmp1, tmp2, &peso) >= 2) {
            strcpy(v1, tmp1);
            strcpy(v2, tmp2);
            vertice *vert1 = adiciona_vertice(g, v1);
            vertice *vert2 = adiciona_vertice(g, v2);
            adiciona_aresta(g, vert1, vert2, peso);
        } else if (strlen(linha) > 0 && g->nome == NULL) {
            g->nome = (char *)malloc(strlen(linha) + 1);
            strcpy(g->nome, linha);
        } else if (strlen(linha) > 0) {
            adiciona_vertice(g, linha);
        }
        free(v1);
        free(v2);
    }
    return g;
}

//------------------------------------------------------------------------------
// Libera toda a memória alocada para o grafo.

unsigned int destroi_grafo(grafo *g) {
    if (!g) return 0;
    while (g->vertices) {
        vertice *v = g->vertices;
        g->vertices = v->proximo;
        while (v->vizinhos) {
            vizinho *vz = v->vizinhos;
            v->vizinhos = vz->proximo;
            free(vz);
        }
        free(v->nome);
        free(v);
    }
    free(g->nome);
    free(g);
    return 1;
}

//------------------------------------------------------------------------------
// Retorna o nome do grafo.

char *nome(grafo *g) {
    return g->nome;
}

//------------------------------------------------------------------------------
// Retorna o número de vértices do grafo.

unsigned int n_vertices(grafo *g) {
    return g->n_vertices;
}

//------------------------------------------------------------------------------
// Retorna o número de arestas do grafo.

unsigned int n_arestas(grafo *g) {
    return g->n_arestas;
}

//------------------------------------------------------------------------------
// Funçãos utilitária para colorir um grafo.

static void dfs_componentes(vertice *v, int cor) {
    v->visitado = 1;
    v->cor = cor;
    for (vizinho *vz = v->vizinhos; vz; vz = vz->proximo)
        if (!vz->v->visitado)
            dfs_componentes(vz->v, cor);
}

//------------------------------------------------------------------------------
// Retorna a quantidade de componentes do grafo.

unsigned int n_componentes(grafo *g) {
    int cor = 0;
    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->cor = -1;
    }
    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado)
            dfs_componentes(v, cor++);
    }
    return cor;
}

//------------------------------------------------------------------------------
// Retorna 1 se o grafo for bipartido e 0 caso contrário.

unsigned int bipartido(grafo *g) {
    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->cor = -1;
    }
    
    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) {
            v->cor = 0;
            vertice *pilha[1024];
            int topo = 0;
            pilha[topo++] = v;
            
            while (topo) {
                vertice *u = pilha[--topo];
                u->visitado = 1;
                for (vizinho *vz = u->vizinhos; vz; vz = vz->proximo) {
                    if (vz->v->cor == -1) {
                        vz->v->cor = 1 - u->cor;
                        pilha[topo++] = vz->v;
                    } else if (vz->v->cor == u->cor) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

//------------------------------------------------------------------------------
// Função utilitária para encontrar o índice de um determinado vértice dentro de uma array.

static int indice_de(vertice *vert, int n, vertice **vet_verts) {
    for (int i = 0; i < n; i++) {
        if (vet_verts[i] == vert) return i;
    }
    return -1;
}

//------------------------------------------------------------------------------
// Função utilitária dijkstra para calcular o diametro considerando os pesos das aretas.

static void dijkstra(vertice *start, vertice **vet_verts, int n, int *distancias) {
    for (int i = 0; i < n; i++) distancias[i] = INF;

    distancias[indice_de(start,n,vet_verts)] = 0;
    int *visitado = calloc(n, sizeof(int));

    for (int count = 0; count < n; count++) {
        int u = -1;
        int min_dist = INF;
        for (int i = 0; i < n; i++) {
            if (!visitado[i] && distancias[i] < min_dist) {
                min_dist = distancias[i];
                u = i;
            }
        }
        
        if (u == -1) break;
        
        visitado[u] = 1;
        
        vertice *vu = vet_verts[u];
        for (vizinho *viz = vu->vizinhos; viz != NULL; viz = viz->proximo) {
            int v_idx = indice_de(viz->v,n,vet_verts);
            if (v_idx != -1 && !visitado[v_idx]) {
                int ndist = distancias[u] + viz->peso;
                if (ndist < distancias[v_idx]) {
                    distancias[v_idx] = ndist;
                }
            }
        }
    }
    
    free(visitado);
}

//------------------------------------------------------------------------------
// Função utilitária para comparar dois inteiros.

static int compara_ints(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    if (ia < ib) return -1;
    else if (ia > ib) return 1;
    else return 0;
}

//------------------------------------------------------------------------------
// Retorna o diametro de todas componentes do grafo.

char *diametros(grafo *g) {
    if (!g || g->n_vertices == 0) return strdup("");

    int n = g->n_vertices;
    vertice *v = g->vertices;

    vertice **vet_verts = malloc(sizeof(vertice*) * n);
    int idx = 0;
    for (vertice *cur = v; cur != NULL; cur = cur->proximo) {
        vet_verts[idx++] = cur;
        cur->visitado = 0;
    }

    int *diametros_componentes = malloc(sizeof(int) * n);
    int n_componentes = 0;

    for (int i = 0; i < n; i++) {
        if (vet_verts[i]->visitado) continue;

        vertice **fila = malloc(sizeof(vertice*) * n);
        int inicio = 0, fim = 0;
        fila[fim++] = vet_verts[i];
        vet_verts[i]->visitado = 1;

        while (inicio < fim) {
            vertice *u = fila[inicio++];
            for (vizinho *viz = u->vizinhos; viz != NULL; viz = viz->proximo) {
                if (!viz->v->visitado) {
                    viz->v->visitado = 1;
                    fila[fim++] = viz->v;
                }
            }
        }

        int *distancias = malloc(sizeof(int) * n);

        dijkstra(vet_verts[i], vet_verts, n, distancias);

        int max_dist = -1;
        vertice *farthest = NULL;
        for (int k = 0; k < n; k++) {
            if (vet_verts[k]->visitado && distancias[k] != INF) {
                if (distancias[k] > max_dist) {
                    max_dist = distancias[k];
                    farthest = vet_verts[k];
                }
            }
        }

        for (int k = 0; k < n; k++) distancias[k] = INF;
        dijkstra(farthest, vet_verts, n, distancias);

        int diametro = -1;
        for (int k = 0; k < n; k++) {
            if (vet_verts[k]->visitado && distancias[k] != INF) {
                if (distancias[k] > diametro) diametro = distancias[k];
            }
        }

        diametros_componentes[n_componentes++] = diametro;

        free(distancias);
        free(fila);
    }

    free(vet_verts);

    qsort(diametros_componentes, n_componentes, sizeof(int), compara_ints);

    int size_str = n_componentes * 12;
    char *res = malloc(size_str);
    res[0] = '\0';

    for (int i = 0; i < n_componentes; i++) {
        char buffer[12];
        sprintf(buffer, "%d", diametros_componentes[i]);
        strcat(res, buffer);
        if (i < n_componentes - 1) strcat(res, " ");
    }

    free(diametros_componentes);
    return res;
}

//------------------------------------------------------------------------------
// Função utilitária que calcula dfs para os vertices de corte.

static void dfs_corte(vertice *u, int *tempo, char **cut_vertices, int *count) {
    u->visitado = 1;
    u->discovery = u->low = ++(*tempo);
    int filhos = 0;

    for (vizinho *vz = u->vizinhos; vz; vz = vz->proximo) {
        vertice *v = vz->v;
        if (!v->visitado) {
            filhos++;
            v->parent = u;
            dfs_corte(v, tempo, cut_vertices, count);

            u->low = (u->low < v->low) ? u->low : v->low;

            if ((u->parent == -1 && filhos > 1) ||
                (u->parent != -1 && v->low >= u->discovery)) {
                cut_vertices[(*count)++] = u->nome;
            }
        } else if (u->parent != v) {
            u->low = (u->low < v->discovery) ? u->low : v->discovery;
        }
    }
}

//------------------------------------------------------------------------------
// Retorna os vértices de corte do grafo.

char *vertices_corte(grafo *g) {
    char **cut_vertices = malloc(sizeof(char *) * g->n_vertices);
    int count = 0, tempo = 0;

    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->parent = -1;
    }

    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) {
            dfs_corte(v, &tempo, cut_vertices, &count);
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(cut_vertices[i], cut_vertices[j]) > 0) {
                char *tmp = cut_vertices[i];
                cut_vertices[i] = cut_vertices[j];
                cut_vertices[j] = tmp;
            }
        }
    }

    char *res = malloc(4096);
    res[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(res, cut_vertices[i]);
        strcat(res, " ");
    }
    if (strlen(res) > 0) res[strlen(res) - 1] = '\0';
    free(cut_vertices);
    return res;
}

//------------------------------------------------------------------------------
// Função utilitária que calcula a dfs para as arestasa de corte.

static void dfs_aresta_corte(vertice *u, int *tempo, char **cut_edges, int *ecount) {
    u->visitado = 1;
    u->discovery = u->low = ++(*tempo);

    for (vizinho *vz = u->vizinhos; vz; vz = vz->proximo) {
        vertice *v = vz->v;
        if (!v->visitado) {
            v->parent = u;
            dfs_aresta_corte(v, tempo, cut_edges, ecount);
            u->low = (u->low < v->low) ? u->low : v->low;

            if (v->low > u->discovery) {
                if (strcmp(u->nome, v->nome) < 0) {
                    cut_edges[(*ecount)++] = u->nome;
                    cut_edges[(*ecount)++] = v->nome;
                } else {
                    cut_edges[(*ecount)++] = v->nome;
                    cut_edges[(*ecount)++] = u->nome;
                }
            }
        } else if (u->parent != v) {
            u->low = (u->low < v->discovery) ? u->low : v->discovery;
        }
    }
}

//------------------------------------------------------------------------------
// Retorna as arestas de corte do grafo.

char *arestas_corte(grafo *g) {
    char **cut_edges = malloc(sizeof(char *) * 2 * g->n_arestas);
    int tempo = 0, ecount = 0;

    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->parent = -1;
    }

    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) {
            dfs_aresta_corte(v, &tempo, cut_edges, &ecount);
        }
    }

    int npairs = ecount / 2;
    char **pares = malloc(sizeof(char *) * npairs);
    for (int i = 0; i < npairs; i++) {
        char *par = malloc(strlen(cut_edges[2 * i]) + strlen(cut_edges[2 * i + 1]) + 2);
        sprintf(par, "%s %s", cut_edges[2 * i], cut_edges[2 * i + 1]);
        pares[i] = par;
    }

    for (int i = 0; i < npairs - 1; i++) {
        for (int j = i + 1; j < npairs; j++) {
            if (strcmp(pares[i], pares[j]) > 0) {
                char *tmp = pares[i]; pares[i] = pares[j]; pares[j] = tmp;
            }
        }
    }

    char *res = malloc(4096);
    res[0] = '\0';
    for (int i = 0; i < npairs; i++) {
        strcat(res, pares[i]);
        strcat(res, " ");
        free(pares[i]);
    }
    if (strlen(res) > 0) res[strlen(res) - 1] = '\0';

    free(pares);
    free(cut_edges);
    return res;
}