#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argv, char *argc[])
{
long entrada;
/* variavel para armazenar o conteudo da variavel de ambiente QUERY_STRING
   - o conteudo enviado pelo metodo GET */
char *dados;

/* obtendo as variaveis enviadas */
dados = getenv("QUERY_STRING");

/* verifica se tem conteudo enviado, caso nao tenha e porque
   o formulario ainda nao foi enviado */
if (strcmp(dados,"") == 0)
{
  printf("Content-type:text/html\n\n");
  printf("<html><head><title>Tyne</title></head>\n");
  printf("<body>");
  printf("<form method=get action=entrada>\n");
  printf("Coloque o codigo a ser analisado <br>");
  printf("<input type=text maxlength=3 size=5 name=entrada>");
  printf("<input type=submit value='continuar'>");
  printf("</form></body></html>");
}
else
{
  printf("Content-type:text/html\n\n");
  printf("Linguagem Tyne\n");
}

return 0;
}