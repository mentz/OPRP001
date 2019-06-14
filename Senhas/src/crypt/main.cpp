#include "./crypt_r.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  char salt[256];
  char password[256];
  crypt_des_data crypt_data;
  char *encrypted;

  printf("Insira o sal: ");
  scanf("%s", salt);
  printf("Digite a senha: ");
  scanf("%s", password);
  encrypted = crypt_des(password, salt, &crypt_data);

  puts(encrypted);
  return 0;
}