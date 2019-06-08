#include <crypt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  char pass[100];
  char *result;
  int ok;

  printf("Inform the pass\n");
  scanf("%s", pass);

  result = crypt(getpass("Password:"), pass);
  ok = strcmp(result, pass) == 0;

  puts(ok ? "Access granted." : "Access denied.");
  return ok ? 0 : 1;
}
