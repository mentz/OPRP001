#include "crypt/crypt.cuh"
#include "utils.h"
#include "wordgen.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
// #include <mpi.h>
#include <omp.h>
#include <set>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

// #include "crypt/crypt_r.h"

typedef unsigned long long ull;
typedef long long ll;

#define WAIT_TIME 100

int stop = 0;
int num_cifras = 0;
std::set<int> falta = std::set<int>();
int mpi_rank = 0;
int mpi_size = 1;
// MPI_Comm comm = MPI_COMM_WORLD;
int falta_size = 0;

// void mpi_master_relay() {
//   std::set<int> done;
//   int next_done;
//   MPI_Request request;
//   MPI_Status status;

//   fprintf(stderr, "P%d iniciando mpi_master_relay\n", mpi_rank);

//   // Receber notificação de cifra K quebrada,
//   //   broadcast de K para todos workers.
//   while (!stop) {
//     int flag;
//     MPI_Irecv(&next_done, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &request);
//     sleep_for(WAIT_TIME);
//     MPI_Test(&request, &flag, &status);
//     if (flag) {
//       done.insert(next_done);

// // Processar a lista para que o master também retire os prontos
// #pragma omp critical(falta_global)
//       {
//         if (falta.count(next_done) > 0)
//           falta.erase(next_done);
//         falta_size = falta.size();
//       }

//       fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);

//       // Replicar para os workers
//       MPI_Bcast(&next_done, 1, MPI_INT, 0, comm);
//     }
//   }
// }

// void mpi_worker_listener() {
//   // Seção de sincronização de progresso
//   std::set<int> done;
//   int next_done;
//   MPI_Request request;
//   MPI_Status status;

//   fprintf(stderr, "P%d iniciando mpi_worker_listener\n", mpi_rank);

//   while (falta.size() > 0 && !stop) {
//     int flag;
//     // Receber int K do broadcast do root
//     //   adicionar esse int K no set done.
//     MPI_Ibcast(&next_done, 1, MPI_INT, 0, comm, &request);
//     sleep_for(WAIT_TIME);
//     MPI_Test(&request, &flag, &status);
//     if (flag) {
// // Processar a lista para que o master também retire os prontos
// #pragma omp critical(falta_global)
//       {
//         if (falta.count(next_done) > 0)
//           falta.erase(next_done);
//         falta_size = falta.size();
//       }
//       fprintf(stderr, "P%d removendo cifra %d\n", mpi_rank, next_done);
//     }
//   }
// }

void force_stop(int signal) {
  fprintf(stderr, "Encerramento forçado: sinal %d\n", signal);
  stop = 1;
}

__global__ void cuda_do_des(char *cifra, bool *resolvido, char *solucao,
                            int *sal_por_cifra, int num_cifras, char *sal,
                            int num_sais, ll inicio, int intervalo,
                            int run_count) {
  int thread = threadIdx.x;
  ll start = threadIdx.x + inicio;
  ll step = blockDim.x * intervalo;

  printf("GPU thread %d starting at %lld with step %lld!\n", thread, start,
         step);

  crypt_des_data meu_crypt_data;

  char senha[16] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  char *result;
  char *ssenha;
  char *cifrados = (char *)malloc(sizeof(char) * 16 * num_sais);

  ll vetor[8] = {start, -1, -1, -1, -1, -1, -1, -1};
  static int maxSize = 64;

  while (run_count--) {
    // Atualizar nova senha, corrigindo a base do número
    int overflow, pos = 0;
    while ((overflow = (vetor[pos] / maxSize)) > 0 && pos < 8) {
      vetor[pos] = vetor[pos] % maxSize;
      // senha[pos] = ascii64[vetor[pos]];
      vetor[++pos] += overflow;
    }
    // senha[pos] = ascii64[vetor[pos]];

    senha[0] = bit7[vetor[0] + 1];
    senha[1] = bit7[vetor[1] + 1];
    senha[2] = bit7[vetor[2] + 1];
    senha[3] = bit7[vetor[3] + 1];
    senha[4] = bit7[vetor[4] + 1];
    senha[5] = bit7[vetor[5] + 1];
    senha[6] = bit7[vetor[6] + 1];
    senha[7] = bit7[vetor[7] + 1];

    for (int i = 0; i < num_sais; i++) {
      int desloc = i * 16;
      result = crypt_des_cuda(senha, &sal[desloc], &meu_crypt_data);

      // Copiar para o vetor
      for (int kkk = 0; kkk < 13; kkk++) {
        cifrados[desloc + kkk] = result[kkk];
      }
      cifrados[desloc + 13] = 0;
      // printf("%s + %s -> %s\n", senha, &sal[desloc], &cifrados[desloc]);
    }

    for (int i = 0; i < num_cifras; i++) {
      if (!resolvido[i]) {
        ssenha = &cifrados[sal_por_cifra[i] * 16];
        if (strncmp_cuda(ssenha, &cifra[i * 16], 13) == 0) {
          resolvido[i] = true;
          solucao[i * 16 + 0] = senha[0];
          solucao[i * 16 + 1] = senha[1];
          solucao[i * 16 + 2] = senha[2];
          solucao[i * 16 + 3] = senha[3];
          solucao[i * 16 + 4] = senha[4];
          solucao[i * 16 + 5] = senha[5];
          solucao[i * 16 + 6] = senha[6];
          solucao[i * 16 + 7] = senha[7];
          solucao[i * 16 + 8] = '\0';

          printf("%s %s\n", &cifra[i * 16], senha);
        }
      }
    }

    // Passar para próxima senha
    vetor[pos] += step;
  }
}

int main(int argc, char *argv[]) {
  signal(SIGINT, force_stop);

  // int thread_level;
  // MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &thread_level);

  // mpi_rank = 0;
  // mpi_size = 1;
  // MPI_Comm comm = MPI_COMM_WORLD;
  // MPI_Comm_rank(comm, &mpi_rank);
  // MPI_Comm_size(comm, &mpi_size);

  // Obter comprimento máximo
  int comprimento = 0;
  ll maximo = 64L;
  if (argc == 2) {
    sscanf(argv[1], "%d", &comprimento);
    comprimento = MIN(8, comprimento);
    for (int i = 1; i < comprimento; i++) {
      maximo++;
      maximo *= (ll)maxSize;
    }
  } else {
    fprintf(stderr, "Falta argumento: %s <comprimento_maximo> [início]\n",
            argv[0]);
    fprintf(stderr, "Uso: Informe pela entrada padrão o número de cifras, "
                    "número de threads  e em "
                    "seguida digite\n");
    fprintf(stderr, "     as cifras uma por linha.\n");
    exit(1);
  }

  // Ler senhas e sincronizar com outros processos MPI
  num_cifras = 0;
  std::map<std::string, int> sais;
  std::set<std::string> sais_vistos;
  std::map<int, std::string> sal_por_cifra;
  std::vector<int> sal_por_indice;
  char **cifras;
  char *cbloco;
  if (mpi_rank == 0) {
    // ROOT
    std::cin >> num_cifras;
    // MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    getchar();
    sal_por_indice = std::vector<int>(num_cifras);
    cifras = new char *[num_cifras];
    cbloco = new char[num_cifras * 32];
    std::string cifra;
    std::vector<std::string> vec_cifras;
    for (int i = 0; i < num_cifras; i++) {
      getline(std::cin, cifra);
      falta.insert(i);
      vec_cifras.push_back(cifra);
    }
    std::sort(vec_cifras.begin(), vec_cifras.end());
    for (int i = 0; i < num_cifras; i++) {
      cifras[i] = &cbloco[i * 32];
      strncpy(cifras[i], vec_cifras[i].data(), 16);
      if (!sais.count(vec_cifras[i].substr(0, 2))) {
        sais_vistos.insert(vec_cifras[i].substr(0, 2));
        sais[vec_cifras[i].substr(0, 2)] = sais.size();
      }
      sal_por_cifra[i] = vec_cifras[i].substr(0, 2);
      sal_por_indice[i] = sais[vec_cifras[i].substr(0, 2)];
    }
    // MPI_Bcast(cifras[0], num_cifras * 32, MPI_CHAR, 0, comm);
    // } else {
    // // Not root
    // MPI_Bcast(&num_cifras, 1, MPI_INT, 0, comm);
    // cifras = new char *[num_cifras];
    // cbloco = new char[num_cifras * 32];
    // sal_por_indice = std::vector<int>(num_cifras);
    // MPI_Bcast(cbloco, num_cifras * 32, MPI_CHAR, 0, comm);
    // for (int i = 0; i < num_cifras; i++) {
    //   cifras[i] = &cbloco[i * 32];
    //   std::string cifra(&cbloco[i * 32], &cbloco[i * 32 + 16]);
    //   falta.insert(i);
    //   if (!sais.count(cifra.substr(0, 2))) {
    //     sais_vistos.insert(cifra.substr(0, 2));
    //     sais[cifra.substr(0, 2)] = sais.size();
    //   }
    //   sal_por_cifra[i] = cifra.substr(0, 2);
    //   sal_por_indice[i] = sais[cifra.substr(0, 2)];
    // }
  }

  // falta_size = falta.size();

  int num_sais = sais.size();
  char *todos_sais = new char[16 * num_sais];
  int sad = 0;
  for (auto &sal : sais) {
    // std::cout << sal.first << " e " << sal.second << "\n";
    // strncpy(&todos_sais[16 * sad], sal.first.data(), 4);
    sal.first.copy(&todos_sais[16 * sad], sal.first.size() + 1);
    todos_sais[16 * sad + sal.first.size()] = '\0';
    sad++;
  }

  for (int i = 0; i < num_sais; i++) {
    printf("%s\n", &todos_sais[i * 16]);
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);

  char *g_cifra, *g_solucao, *g_sal;
  cudaMalloc((void **)&g_cifra, sizeof(char) * 16 * num_cifras);
  cudaMalloc((void **)&g_solucao, sizeof(char) * 16 * num_cifras);
  cudaMalloc((void **)&g_sal, sizeof(char) * 16 * num_sais);

  cudaMemcpy(g_cifra, cifras[0], sizeof(char) * 16 * num_cifras,
             cudaMemcpyHostToDevice);
  cudaMemcpy(g_sal, todos_sais, sizeof(char) * 16 * num_sais,
             cudaMemcpyHostToDevice);

  bool *g_resolvido;
  cudaMalloc((void **)&g_resolvido, sizeof(bool) * num_cifras);

  int *g_sal_por_cifra;
  cudaMalloc((void **)&g_sal_por_cifra, sizeof(int) * num_cifras);
  cudaMemcpy(g_sal_por_cifra, sal_por_indice.data(), sizeof(int) * num_cifras,
             cudaMemcpyHostToDevice);

  dim3 grid(1);
  dim3 blok(64);

  cuda_do_des<<<grid, blok>>>(g_cifra, g_resolvido, g_solucao, g_sal_por_cifra,
                              num_cifras, g_sal, num_sais, 0ll, 1, 266305);

  //   // Iniciar thread de sincronização entre MPI workers
  //   std::thread *sync_thread;
  //   if (mpi_size > 1) {
  //     if (mpi_rank == 0)
  //       sync_thread = new std::thread(mpi_master_relay);
  //     else {
  //       sync_thread = new std::thread(mpi_worker_listener);
  //     }
  //   }

  //   // Usar todos threads disponíveis
  //   int num_threads = omp_get_max_threads();
  //   // num_threads = 1;
  //   omp_set_num_threads(num_threads);
  //   fprintf(stderr, "p%d Usando %d threads\n", mpi_rank, num_threads);

  //   std::map<std::string, std::string> solucoes;
  //   ll i = 0L, counter = 0;
  // #pragma omp parallel reduction(+ : counter)
  //   {
  //     // Inicializar sais (aceleração grande)
  //     std::map<std::string, crypt_data> crypt_data_por_sal;
  //     // std::map<std::string, crypt_des_data> crypt_data_por_sal;
  //     for (auto &ss : sais) {
  //       crypt_data_por_sal[ss] = crypt_data();
  //       // crypt_data_por_sal[ss] = crypt_des_data();
  //     }
  //     crypt_data *crypt_pointer;
  //     // crypt_des_data *crypt_pointer;
  //     char *result;
  //     int thread_rank = omp_get_thread_num();
  //     int inicio = (mpi_rank * omp_get_num_threads()) +
  //     omp_get_thread_num(); int passo = mpi_size * omp_get_num_threads();
  //     fprintf(stderr, "p%d t%d inicia em %d (passo %d), existem %d
  //     threads\n",
  //             mpi_rank, thread_rank, inicio, passo, omp_get_num_threads());
  //     Senha senha(inicio);
  //     ll thread_i;
  //     std::set<int> thread_falta(falta);
  //     int thread_falta_size = num_cifras;
  //     for (thread_i = inicio; thread_i < maximo && !stop; thread_i +=
  //     passo) {
  //       // if ((falta.size() < thread_falta.size())) {
  //       if (falta_size < thread_falta_size) {

  // #pragma omp critical(falta_global)
  //         thread_falta = falta;

  //         thread_falta_size = thread_falta.size();
  //       }
  //       for (auto &e : thread_falta) {
  //         // printf("p%d t%d %s %s\n", mpi_rank, thread_rank, cifras[e],
  //         //        senha.getSenha());
  //         crypt_pointer = &(crypt_data_por_sal[sais[e]]);
  //         result = crypt_r(senha.getSenha(), cifras[e], crypt_pointer);
  //         // result = crypt_des(senha.getSenha(), cifras[e],
  //         crypt_pointer); int ok = strncmp(result, cifras[e], 14) == 0;

  //         if (ok) {
  //           // printf("p%*d, t%*d @ %2.f%%: %s = %s\n",
  //           // (int)ceil(log10(mpi_size)),
  //           //        mpi_rank, (int)ceil(log10(passo)), thread_rank,
  //           //        (thread_i / (double)maximo) * 100, cifras[e],
  //           //        senha.getSenha());
  //           printf("%s %s\n", cifras[e], senha.getSenha());
  //           fflush(stdout);
  //           // solucoes[cifras[e]] = senha.getSenha();
  //           int next_done = e;

  //           if (mpi_rank == 0 || mpi_size == 1) {
  // #pragma omp critical(falta_global)
  //             if (falta.count(e) > 0) {
  //               falta.erase(e);
  //             }

  //             // Replicar para os workers
  //             MPI_Bcast(&next_done, 1, MPI_INT, 0, comm);
  //           } else {
  //             MPI_Send(&next_done, 1, MPI_INT, 0, 0, comm);
  //           }
  //         }
  //       }

  //       if (((thread_i + 1) % 50000) == 0) {
  //         fprintf(stderr, "Realizado %2.f%% ou %llu de %llu\n",
  //                 (thread_i / (double)maximo) * 100, thread_i + 1, maximo);
  //       }

  //       senha.prox(passo);
  //       counter++;
  //     }
  // #pragma omp barrier
  //   }
  stop = true;
  //   if (mpi_size > 1) {
  //     sync_thread->join();
  //     delete sync_thread;
  //   }
  // fprintf(stderr, "[%d] terminou em %llu iterações!!!!\n", mpi_rank,
  // counter); for (auto &e : solucoes) {
  //   printf("%s %s\n", e.first.data(), e.second.data());
  // }

  //   MPI_Finalize();
  cudaDeviceSynchronize();

  return 0;
}
