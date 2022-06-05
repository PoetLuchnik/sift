#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>

// typedef

typedef uint16_t index_t;

typedef struct {
    index_t link;
    char* text;
} var_t;

typedef struct {
    char* text;
    index_t vnum;
    var_t* vars;
} screen_t;

typedef struct {
    /* exept 0 */
    index_t snum;
    screen_t* screens;
} game_t;

// global vars

const char* default_file = "./data.sift";
game_t game;

// functions

void print_error(const char* s) {
    fprintf(stderr, "\nerror: %s\n", s);
}

void var_free(var_t* v) {
    if (v->text != NULL) free(v->text);
}

void screen_free(screen_t* s) {
    if (s->vars != NULL) {
        for (index_t i = 0; i < s->vnum; i++) {
            var_free(&(s->vars[i]));
        }
    }
    if (s->text != NULL) free(s->text);
}

void game_free() {
    if (game.screens != NULL) {
        for (index_t i = 0; i < game.snum; i++) {
            screen_free(&(game.screens[i]));
        }
    }
}

int load_index(FILE* f, index_t* i) {
    int v;
    if ((v = fgetc(f)) == EOF) return 0;
    *i = v << 8;
    if ((v = fgetc(f)) == EOF) return 0;
    *i = v;
    return 1;
}

int load_text(FILE* f, char** t) {
    index_t len = 0;

    if (!(load_index(f, &len) && len != 0)) {
        print_error("invalid text length!");
        return 0;
    }

    *t = (char*)malloc(sizeof(char) * (len + 1));

    if (*t == NULL) {
        print_error("malloc returns null");
        return 0;
    }

    if (fread(*t, sizeof(char), len, f) != len) {
        print_error("invalid text data");
        return 0;
    }

    (*t)[len] = 0;

    return 1;
}

int load_var(FILE* f, var_t* v) {
    if (!(load_index(f, &(v->link)))) {
        print_error("invalid link!");
        return 0;
    }

    if (!load_text(f, &(v->text))) {
        print_error("invalid var text");
        return 0;
    }

    return 1;
}

int load_screen(FILE* f, screen_t* s) {
    if (!load_text(f, &(s->text))) {
        print_error("invalid screen text");
        return 0;
    }

    if (!(load_index(f, &(s->vnum)))) {
        print_error("var num invalid!");
        return 0;
    }

    if (s->vnum == 0) {
        s->vars = NULL;
        return 1;
    }

    s->vars = (var_t*)malloc(sizeof(var_t) * s->vnum);

    if (s->vars == NULL) {
        print_error("malloc returns null");
        return 0;
    }

    for (index_t i = 0; i < s->vnum; i++) {
        if (!load_var(f, &(s->vars[i]))) return 0;
    }

    return 1;
}

int load_game(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        print_error("file not found!");
        return 0;
    }

    if (!(load_index(f, &(game.snum)) && game.snum != 0)) {
        print_error("screen num invalid!");
        fclose(f);
        return 0;
    }

    game.screens = (screen_t*)malloc(sizeof(screen_t) * game.snum);

    if (game.screens == NULL) {
        print_error("malloc returns null");
        fclose(f);
        return 0;
    }

    for (index_t i = 0; i < game.snum; i++) {
        if (!load_screen(f, &(game.screens[i]))) {
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    return 1;
}

void show_vars(screen_t* s) {
    for (index_t i = 0, j = 1; i < s->vnum; i++, j++) {
        printf("%i) %s\n", j, s->vars[i].text);
    }
}

int getnextc() {
    int c = getchar();
    if (c == '\n' || c == '\r' || c == EOF || c == 0)
        return 0;
    return c;
}

int select_var() {
    int n = 0, d = 0;
    while ((d = getnextc()) != 0) {
        n = n * 10 + (d - '0');
    }
    return n - 1;
}

int main(int argc, char** argv) {
    if (argc > 1 && load_game(argv[1])) {}
    else if (load_game(default_file)) {}
    else {
        game_free();
        return 1;
    }

    index_t curs = 0;
    for (;;) {
        curs = curs % game.snum;

        screen_t s = game.screens[curs];

        puts(s.text);

        if (s.vnum == 0) break;

        show_vars(&s);

        int vari = select_var();

        curs = s.vars[vari % s.vnum].link;
    }

    game_free();

    return 0;
}