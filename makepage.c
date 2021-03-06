#include "str.h"
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static DIR *
eopendir(const char *dir)
{
        DIR *d = opendir(dir);
        if (!d) {
                fprintf(stderr, "Cannot open dir: %s\n", dir);
                exit(1);
        };

        return d;
}

static FILE *
efopen(const char *file, const char *mode)
{
        FILE *fp = fopen(file, mode);
        if (!fp) {
                fprintf(stderr, "Cannot open file: %s\n", file);
                exit(1);
        }

        return fp;
}

static void
makepagefile(char *fname, char *dirpath)
{
        char pagefile[256];
        FILE *codefp = efopen(fname, "r");
        strnjoin(fname, ".html", pagefile, ARR_LEN(pagefile));
        FILE *pagefp = efopen(pagefile, "w");

        fprintf(
            pagefp,
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
            dirpath, dirpath, pagefile);

        char lineBuff[1000] = {0};
        char *lt = "<";
        char *gt = ">";
        char *fourspaces = "    ";
        int i = 1;

        while (fgets(lineBuff, ARR_LEN(lineBuff), codefp) != NULL) {
                if (strstr(lineBuff, lt) && strstr(lineBuff, gt)) {
                        char parse[1000] = {0};
                        char parsesec[1000] = {0};

                        char *ltesc = "&lt;";
                        char *gtesc = "&gt;";

                        strnrpl(lineBuff, lt, ltesc, parse, ARR_LEN(parse));
                        strnrpl(parse, gt, gtesc, parsesec, ARR_LEN(parsesec));

                        fprintf(pagefp,
                                "<div><a href=#l%d "
                                "id=l%d>%d</a>%s</div>",
                                i, i, i, parse);
                        goto increment;
                }
                if (strstr(lineBuff, fourspaces)) {
                        fprintf(pagefp,
                                "<div><a href=#l%d "
                                "id=l%d>%d</a>&emsp;&emsp;%s</div>",
                                i, i, i, lineBuff);
                        goto increment;
                }
                fprintf(pagefp, "<div><a href=#l%d id=l%d>%d</a>%s</div>", i,
                        i, i, lineBuff);
        increment:
                i++;
        }
        fprintf(pagefp, "%s", "</div></body></html>");

        fclose(codefp);
        fclose(pagefp);
}

static void
makefilesindir(DIR *d, char *dirname)
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
        makefilesindir(sourcedir, dirname);
        makeindexfile(sourcedir, dirname);
}
