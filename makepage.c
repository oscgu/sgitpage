#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char head[] = "<!DOCTYPE html><html lang=en><head><title>Static git page generator</title><meta charset=UTF-8><meta name=viewport content=width=device-width initial-scale=1> <style> body { background-color: #292929; color: hsla(0, 0%, 100%, 0.9); display: flex; flex-direction: column; flex-wrap: wrap; } h1 { color: hsla(0, 0%, 100%, 0.9); } p { width: 100%; border: solid rgb(88, 88, 88); border-width: 0 0 2px 0; } .code a { padding-left: 30px; padding-right: 5px; border: solid rgb(24, 24, 24); border-width: 2px 2px 0 2px; text-decoration: none !important; color: hsla(0, 0%, 100%, 0.9); } .code a:focus { background-color: #3d3d3d; } </style></head><body> <h1>Static git page generator</h1> <p>File: name</p> <div class=code>";
static char foot[] = "</div></body></html>";

static void
setfend(char *fname)
{
  char fend[] = ".html\0";
  char *pos = strstr(fname, ".c");
  int distance = &pos[0] - &fname[0];

  int i;
  for (i = 0; i < strlen(fend) + 1; i++) {
    fname[distance + i] = fend[i];
  }
}

static void rfile(char *fname) {
  FILE *fp = fopen(fname, "r");
  char lineBuff[1000];

  setfend(fname);
  FILE *nf = fopen(fname, "w");

  if (!fp || !nf) {
    fprintf(stderr, "Cannot open file: %s\n", fname);
  }

  fprintf(nf, "%s", head);
  int i = 1;
  while (fgets(lineBuff, 1000, fp) != NULL) {
    fprintf(nf, "<div><a href=#l%d id=l%d>%d</a>%s</div>", i, i, i, lineBuff);
    i++;
  }
  fprintf(nf, "%s", foot);

  fclose(fp);
  fclose(nf);
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Missing argument");
        exit(1);
    }


  char name[256];
  strcpy(name, argv[1]);
  rfile(name);
}