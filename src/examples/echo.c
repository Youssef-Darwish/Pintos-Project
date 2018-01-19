#include <stdio.h>
#include <syscall.h>
int
main (int argc, char **argv)
{
  int i;
    if(argc==9){
        while (true){

        }
    }
   // printf ("arguments are %d\n", argc);

    for (i = 0; i < argc; i++)
        printf ("%s ", argv[i]);
  printf ("lol\n");
    exit(0);
  return EXIT_SUCCESS;
}
