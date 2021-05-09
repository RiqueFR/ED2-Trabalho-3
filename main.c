#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "inputread.h"
/*Inicializa um grafo
 * inputs: quantidade de vertices e um vetor de pages
 * output: ponteiro para a grafo inicializado
 * pre-condicao: inteiro e pages existem
 * pos-condicao: grafo de retorno existe
 */
#include "page.h"

int compare_word(const void *a, const void *b);

int main(int argc, char *argv[]) {
    //Verifica se a quantidade minima de parametros foram passados
    if (argc < 2) {
        printf("Poucos argumentos fornecidos!\n");
        return 1;
    }
    char *dir = argv[1];

    //leituras/inputs
    int n_pages = input_read_num_pages(dir);
    Page **pages = input_read_page(dir, n_pages);
    Graph *graph = input_read_graph(dir, n_pages, pages);
    int n_stop_words = input_read_num_stopwords(dir);
    char **stop_words = input_read_stopwords(dir, n_stop_words);

    //ordenar paginas em ordem alfabetica
    qsort(stop_words, n_stop_words, sizeof(char *), compare_word);

    //calcula PR das paginas
    calc_PR(graph, pages);

    char *buffer;
    size_t bufsize = 100;
    int characters;
    buffer = (char *)malloc(bufsize * sizeof(char));
    Page *pages_verified[n_pages];

    if (buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    //para cada linha inserida, verificamos se a consulta esta nos arquivos
    characters = getline(&buffer, &bufsize, stdin);
    while (characters >= 0) {
        buffer[characters - 1] = '\0';
        //verifica se alguma pagina tem todas palavras da consulta
        verify_query(pages_verified, pages, buffer, n_pages, stop_words, n_stop_words, dir);

        //ordena em quesito de PR as paginas em que foram encontradas as palavras
        qsort(pages_verified, n_pages, sizeof(Page *), compare_page_rank);

        //imprime resultados
        printf_pages(pages_verified, n_pages);
        printf_prs(pages_verified, n_pages);

        characters = getline(&buffer, &bufsize, stdin);
    }

    //libera a memoria
    for (int i = 0; i < n_stop_words; i++)
        free(stop_words[i]);
    free(stop_words);

    destroy_graph(graph);
    for (int i = 0; i < n_pages; i++)
        destroy_page(pages[i]);
    free(pages);

    free(buffer);

    return 0;
}

int compare_word(const void *a, const void *b) {
    /*
    * Segundo a ordem alfabética retorna um valor:
    * > 0 -> primeiro parametro maior que o segundo
    * = 0 -> parametros iguais
    * < 0 -> segundo parametro maior que o primeiro
    */
    char *a1 = *(char **)a;
    char *a2 = *(char **)b;

    return strcasecmp(a1, a2);
}
