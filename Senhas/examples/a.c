#include <crypt.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  char salt[256];
  char *password;

  printf("Insira o sal: ");
  scanf("%s", salt);
  password = crypt(getpass("Digite a senha: "), salt);

  puts(password);
  return 0;
}
