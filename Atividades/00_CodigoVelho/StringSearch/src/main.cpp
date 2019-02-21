#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

typedef unsigned int uint;

uint buscaOcorrencias(std::string palavra, std::string texto) {
    int ocorrencias = 0;
    bool deuMatch = false;

    for (int i = 0; i < texto.size(); i++) {
        deuMatch = true;
        for (int j = 0; j < palavra.size() && i+j < texto.size(); j++) {
        	if (texto[i+j] != palavra[j]) {
            	deuMatch = false;
            }
        }

        if (deuMatch) {
        	ocorrencias++;
        }
    }
    
    return ocorrencias;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Comando incorreto. Modo de uso:");
        printf("%s \"palavra\" arquivo_texto.txt\n", argv[0]);
        return 1;
    }
    
    std::string palavra, texto;
    std::ostringstream ss_texto;
    palavra = argv[1];
    std::ifstream arq_texto(argv[2], std::ifstream::in);
    if (arq_texto.good())
        ss_texto << arq_texto.rdbuf();
    texto = ss_texto.str();

    printf("%u ocorrências de \"%s\" no texto.\n", buscaOcorrencias(palavra, texto), palavra.data());


    return 0;
}