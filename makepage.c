#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static void
setfend(char *fname)
{
        strcat(fname, ".html");
}

static DIR *
eopendir(const char *dir)
{
        DIR *d = opendir(dir);
        if (!d) { fprintf(stderr, "Cannot open dir: %s\n", dir); }

        return d;
}

static int
strnjoin(const char *first, const char *sec, char *dest, int destsize)
{
        int firstlen = strlen(first);
        int seclen = strlen(sec);
        if (firstlen + seclen + 1 > destsize) {
                fprintf(stderr, "Dest must hold atleast '%d' chars",
                        firstlen + seclen + 1);
                return -1;
        }

        strncpy(dest, first, destsize);
        int restsize = destsize - firstlen - 1;
        strncat(dest, sec, restsize);
        dest[firstlen + seclen] = '\0';

        return 1;
}

static void
makepagefile(char *fname, char *dirpath)
{
        FILE *fp = fopen(fname, "r");
        char fileName[256];
        strcpy(fileName, fname);
        char lineBuff[1000];
        int dirpathlen = strlen(dirpath);
        char dirname[dirpathlen];
        strcpy(dirname, dirpath);
        dirname[dirpathlen - 1] = '\0';

        setfend(fname);
        FILE *nf = fopen(fname, "w");

        if (!fp || !nf) { fprintf(stderr, "Cannot open file: %s\n", fname); }

        fprintf(
            nf,
            "<!DOCTYPE html><html lang=en><head><title>%s"
            "</title><meta charset=UTF-8><meta name=viewport "
            "content=width=device-width initial-scale=1> <style> body { "
            "background-color: #292929; color: hsla(0, 0%%, 100%%, 0.9); "
            "display: flex; "
            "flex-direction: column; flex-wrap: wrap; } h1 { color: hsla(0, "
            "0%%, "
            "100%%, "
            "0.9); } p { width: 100%%; border: solid rgb(88, 88, 88); "
            "border-width: 0 0 "
            "2px 0; } .code a { padding-left: 30px; padding-right: 5px; "
            "border: "
            "solid "
            "rgb(24, 24, 24); border-width: 2px 2px 0 2px; text-decoration: "
            "none "
            "!important; color: hsla(0, 0%%, 100%%, 0.9); } .code a:focus { "
            "background-color: #3d3d3d; } </style></head><body> <h1>%s"
            "</h1><a href=index.html>Menu</a> <p>File: %s</p> <div "
            "class=code>",
            dirname, dirname, fileName);
        int i = 1;
        while (fgets(lineBuff, 1000, fp) != NULL) {
                fprintf(nf, "<div><a href=#l%d id=l%d>%d</a>%s</div>", i, i, i,
                        lineBuff);
                i++;
        }
        fprintf(nf, "%s", "</div></body></html>");

        fclose(fp);
        fclose(nf);
}

static void
makeallpagefiles(DIR *d, char *dirname)
{
        struct dirent *dir;

        while ((dir = readdir(d)) != NULL) {
                if (dir->d_type != 8 || strstr(dir->d_name, ".html")) continue;

                char fullPath[256];
                strcpy(fullPath, dirname);
                strcat(fullPath, dir->d_name);

                makepagefile(fullPath, dirname);
        }
        closedir(d);
}

static void
makeindexfile(DIR *d, char *dirname)
{
        struct dirent *dir;
        d = eopendir(dirname);
        char indexfilepath[256];
        strnjoin(dirname, "index.html", indexfilepath, sizeof(indexfilepath));

        FILE *indexfp = fopen(indexfilepath, "w");
        fprintf(
            indexfp,
            "<!DOCTYPE html> <html lang=en> <head> <title>%s</title> <meta "
            "charset=UTF-8> <meta name=viewport content=width=device-width, "
            "initial-scale=1> <style> body { background-color: #292929; "
            "color: "
            "hsla(0, 0%%, 100%%, 0.9); display: flex; flex-direction: column; "
            "flex-wrap: wrap; } h1 { color: hsla(0, 0%%, 100%%, 0.9); width: "
            "100%%; border: solid rgb(88, 88, 88); border-width: 0 0 2px 0; } "
            "</style> </head> <body> <h1>%s</h1> <table> <tr> <td>File</td> "
            "<td>Author</td> <td>Last changed</td> </tr>",
            dirname, dirname);

        while ((dir = readdir(d)) != NULL) {
                if (dir->d_type != 8 || strstr(dir->d_name, ".html")) continue;

                char fullfilepath[255];
                strcpy(fullfilepath, dirname);
                strcat(fullfilepath, dir->d_name);

                struct stat filestat;
                stat(fullfilepath, &filestat);

                struct passwd *fowner = getpwuid(filestat.st_uid);

                fprintf(indexfp,
                        "<tr> <td> <a href=%s.html>%s</a></td> <td>%s</td> "
                        "<td>%s</td> </tr>",
                        dir->d_name, dir->d_name, fowner->pw_name,
                        ctime(&filestat.st_mtim.tv_sec));
        }
        fprintf(indexfp, "</table> </body> </html>");

        closedir(d);
        fclose(indexfp);
}

int
main(int argc, char *argv[])
{
        char dirname[256];

        if (!argv[1]) {
                fprintf(stderr, "Please provide a directory\n");
                exit(1);
        }
        strncpy(dirname, argv[1], sizeof(dirname));

        DIR *sourcedir = eopendir(dirname);
        makeallpagefiles(sourcedir, dirname);
        makeindexfile(sourcedir, dirname);
}
