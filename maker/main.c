#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<ctype.h>

typedef uint16_t index_t;

FILE* f = 0;
FILE* o = 0;
const char* default_file = "data.txt";
int curc = 0;

int nextc() {
    curc = fgetc(f);
    //putchar(curc);
    return curc;
}

void close_all() {
    if (f != NULL) {
        fclose(f);
        f = NULL;
    }
    if (o != NULL) {
        fclose(o);
        o = NULL;
    }
}

void print_error(const char* s) {
    fprintf(stderr, "\nerror: %s\n", s);
    close_all();
}

void o_index(index_t i) {
    fputc(i >> 8, o);
    fputc(i & 0xFF, o);
}

void f_comment() {
    //puts("<comment>");
    while (curc != EOF && curc != ']') nextc();
}

void f_trash() {
    //puts("<trash>");
    while (curc != EOF) {
        if (!isspace(curc)) {
            if (curc == '[') {
                f_comment();
            }
            else {
                return;
            }
        }
        nextc();
    }
}

int f_text() {
    //puts("<text>");
    if (curc != '{') {
        print_error("text not found");
        return 0;
    }

    long len_pos = ftell(o);
    o_index(0);
    index_t len = 0;

    nextc();

    while (curc != EOF && curc != '}') {
        len++;
        fputc(curc, o);
        nextc();
    }

    nextc(); // skip }

    if (len == 0) {
        print_error("null text length");
        return 0;
    }

    long end_pos = ftell(o);
    fseek(o, len_pos, SEEK_SET);
    o_index(len);
    fseek(o, end_pos, SEEK_SET);

    return 1;
}

int f_index() {
    //puts("<index>");
    index_t i = 0;
    while (isdigit(curc)) {
        i = i * 10 + (curc - '0');
        nextc();
    }
    o_index(i);
    return 1;
}

int f_var() {
    //puts("<var>");
    if (!f_index()) return 0;
    f_trash();
    if (!f_text()) return 0;
    return 1;
}

int f_screen() {
    //puts("<screen>");
    f_trash();
    if (!f_text()) return 0;

    long vnum_pos = ftell(o);
    o_index(0);
    index_t vnum = 0;

    for (;;) {
        f_trash();
        if (curc == ',' || curc == EOF) break;
        if (!f_var()) return 0;
        vnum++;
    }

    long end_pos = ftell(o);
    fseek(o, vnum_pos, SEEK_SET);
    o_index(vnum);
    fseek(o, end_pos, SEEK_SET);
    
    return 1;
}

int main(int argc, char** argv) {
    if (argc > 1) f = fopen(argv[1], "r");
    if (f == NULL) f = fopen(default_file, "r");
    if (f == NULL) {
        print_error("file not found");
        return 1;
    }
    o = fopen("./data.sift", "wb");

    long snum_pos = ftell(o);
    o_index(0);
    index_t snum = 0;
    nextc();

    for (;;) {
        if (!f_screen()) return 1;
        snum++;
        if (curc == EOF) break;
        nextc();
    }

    long end_pos = ftell(o);
    fseek(o, snum_pos, SEEK_SET);
    o_index(snum);
    fseek(o, end_pos, SEEK_SET);

    close_all();

    return 0;
}