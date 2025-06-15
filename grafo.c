#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

// -------------------------------
// Funções utilitárias

static vertice *busca_vertice(grafo *g, const char *nome) {
    for (vertice *v = g->vertices; v != NULL; v = v->proximo) {   
        if (v != NULL) {
            if (strcmp(v->nome, nome) == 0)
                return v;
        }
    }
    return NULL;
}

static vertice *adiciona_vertice(grafo *g, const char *nome) {
    vertice *v = busca_vertice(g, nome);
    if (v) return v;

    v = malloc(sizeof(vertice));
    v->nome = strdup(nome);
    if (v->nome == NULL) {
        perror("Erro fatal: Falha ao alocar memoria para o nome do vertice");
        free(v); // Libera o vértice alocado para evitar leak imediato
        exit(EXIT_FAILURE); // Aborta o programa
    }
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

// -------------------------------
// leitura

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

        char v1[1023], v2[1023];
        int peso;

        if (sscanf(linha, "%s -- %s %d", v1, v2, &peso) >= 2) {
            vertice *vert1 = adiciona_vertice(g, v1);
            vertice *vert2 = adiciona_vertice(g, v2);
            adiciona_aresta(g, vert1, vert2, peso);
        } else if (strlen(linha) > 0 && g->nome == NULL) {
            g->nome = strdup(linha);
        } else if (strlen(linha) > 0) {
            adiciona_vertice(g, linha);
        }
    }
    return g;
}

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

char *nome(grafo *g) {
    return g->nome;
}

unsigned int n_vertices(grafo *g) {
    return g->n_vertices;
}

unsigned int n_arestas(grafo *g) {
    return g->n_arestas;
}

static void dfs_componentes(vertice *v, int cor) {
    v->visitado = 1;
    v->cor = cor;
    for (vizinho *vz = v->vizinhos; vz; vz = vz->proximo)
        if (!vz->v->visitado)
            dfs_componentes(vz->v, cor);
}

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

static int bfs_max_dist(vertice *inicio) {
    vertice *queue[2048];
    int dist[2048];
    int front = 0, back = 0;
    int index = 0;
    for (vertice *v = inicio; v; v = v->proximo) {
        v->visitado = 0;
        dist[index++] = 0;
    }

    inicio->visitado = 1;
    queue[back++] = inicio;
    dist[0] = 0;

    int maxd = 0;
    while (front < back) {
        vertice *u = queue[front++];
        for (vizinho *vz = u->vizinhos; vz; vz = vz->proximo) {
            if (!vz->v->visitado) {
                vz->v->visitado = 1;
                dist[back] = dist[front - 1] + 1;
                if (dist[back] > maxd) maxd = dist[back];
                queue[back++] = vz->v;
            }
        }
    }
    return maxd;
}

char *diametros(grafo *g) {
    int componentes = n_componentes(g);
    int *diametros = calloc(componentes, sizeof(int));
    for (vertice *v = g->vertices; v; v = v->proximo) v->visitado = 0;

    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) {
            int d = bfs_max_dist(v);
            if (d > diametros[v->cor]) diametros[v->cor] = d;
        }
    }

    char *res = malloc(componentes * 12);
    res[0] = '\0';
    for (int i = 0; i < componentes; i++) {
        char buf[12];
        sprintf(buf, "%d", diametros[i]);
        strcat(res, buf);
        if (i < componentes - 1) strcat(res, " ");
    }
    free(diametros);
    return res;
}

static void dfs_corte(vertice *u, int *tempo, char ***cut_vertices, int *count, char ***cut_edges, int *ecount) {
    u->visitado = 1;
    u->discovery = u->low = ++(*tempo);
    int filhos = 0;
    for (vizinho *vz = u->vizinhos; vz; vz = vz->proximo) {
        vertice *v = vz->v;
        if (!v->visitado) {
            filhos++;
            v->parent = u;
            dfs_corte(v, tempo, cut_vertices, count, cut_edges, ecount);
            u->low = (u->low < v->low) ? u->low : v->low;
            if ((u->parent == -1 && filhos > 1) || (u->parent != -1 && v->low >= u->discovery)) {
                (*cut_vertices)[(*count)++] = u->nome;
            }
            if (v->low > u->discovery) {
                (*cut_edges)[(*ecount)++] = strdup(u->nome);
                (*cut_edges)[(*ecount)++] = strdup(v->nome);
            }
        } else if (v != u->parent) {
            u->low = (u->low < v->discovery) ? u->low : v->discovery;
        }
    }
}

char *vertices_corte(grafo *g) {
    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->discovery = v->low = 0;
        v->parent = -1;
    }
    char **cut = malloc(g->n_vertices * sizeof(char *));
    int count = 0, tempo = 0;
    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) dfs_corte(v, &tempo, &cut, &count, NULL, NULL);
    }
    qsort(cut, count, sizeof(char *), (int (*)(const void *, const void *))strcmp);
    char *res = malloc(2048); res[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(res, cut[i]);
        if (i < count - 1) strcat(res, " ");
    }
    free(cut);
    return res;
}

char *arestas_corte(grafo *g) {
    for (vertice *v = g->vertices; v; v = v->proximo) {
        v->visitado = 0;
        v->discovery = v->low = 0;
        v->parent = -1;
    }
    char **edges = malloc(2 * g->n_arestas * sizeof(char *));
    int count = 0, tempo = 0;
    for (vertice *v = g->vertices; v; v = v->proximo) {
        if (!v->visitado) dfs_corte(v, &tempo, NULL, NULL, &edges, &count);
    }
    char *res = malloc(2048); res[0] = '\0';
    for (int i = 0; i < count; i += 2) {
        if (strcmp(edges[i], edges[i + 1]) > 0) {
            char *tmp = edges[i]; edges[i] = edges[i + 1]; edges[i + 1] = tmp;
        }
    }
    qsort(edges, count / 2, 2 * sizeof(char *), (int (*)(const void *, const void *))strcmp);
    for (int i = 0; i < count; i++) {
        strcat(res, edges[i]);
        if (i < count - 1) strcat(res, " ");
        free(edges[i]);
    }
    free(edges);
    return res;
}