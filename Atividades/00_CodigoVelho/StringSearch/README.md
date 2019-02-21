# Busca de palavra em texto

A implementação naïve é a seguinte (C++/Pseudocódigo):

```c++
int buscaOcorrencias(string palavra, string texto) {
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
```

## Teste
Compile com `make` e execute o programa com `make test` para obter o resultado de tempo de execução (aproximado).

Para fins de comparação, o resultado obtido em um computador com processador AMD Ryzen 7 1700, 16GB de RAM DDR4 2933MHz e SSD Sata foi de 0,207 segundos.