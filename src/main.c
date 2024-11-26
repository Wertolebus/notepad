#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINE_SIZE       1024
#define BUFFER_SIZE     1024
#define LETTERS_SIZE    1024
#define FONT_SIZE       24
#define FONT_SPACING    (int) -(FONT_SIZE/2) 
#define BG_COLOR        (Color) {34, 35, 46, 255}

typedef struct {
    int key;
    Color color;
} Letter;

typedef struct {
    int size;
    Letter *buffer[LINE_SIZE];
} Line;

typedef struct {
    int size;
    Line *buffer[BUFFER_SIZE];
} Buffer;

Buffer buffer = {.size = 1, .buffer = {&(Line) {0, {0}}}};

int line_cursor = 0;
int buffer_cursor = 0;
int timer = 0;
void trace_line(Line *line) {
    for(int i = 0; i < line->size; i++){
        TraceLog(LOG_INFO, TextFormat("Tracing line %d -> %d", i, line->buffer[i]));
    }
}

void write_before_cursor(Line *line, Letter letter){
    if (line_cursor < LINE_SIZE) {
        // TraceLog(LOG_INFO, TextFormat("key -> %d", key));

        
    if (line_cursor < line->size) {
        memmove(&line->buffer[line_cursor + 1], 
                &line->buffer[line_cursor],
                (line->size - line_cursor) * sizeof(Letter)); 
    }

    line->buffer[line_cursor] = malloc(sizeof(Letter));     
    *line->buffer[line_cursor] = (Letter) {letter.key, WHITE};
    
    
    line_cursor++;
    line->size++;

    // trace_line(line); 
    }
}

void display_text(Font font){
    Vector2 pen = {0};
    for (int line = 0; line < buffer.size; line++){
        for (int chr = 0; chr < buffer.buffer[line]->size; chr++){
            DrawTextEx(font, TextFormat("%c", buffer.buffer[line]->buffer[chr]->key), pen, FONT_SIZE, FONT_SPACING, buffer.buffer[line]->buffer[chr]->color); 
            pen.x += FONT_SIZE + FONT_SPACING;
        }
        pen.x = 0;
        pen.y += FONT_SIZE;
    }
}

void draw_cursor(){
    timer++;
    if (timer < GetFPS() / 2) DrawRectangle(line_cursor*(FONT_SIZE + FONT_SPACING), buffer_cursor*(FONT_SIZE), 3, FONT_SIZE, WHITE);
    else if (timer > GetFPS()) timer = 0;
}

void remove_char(Line *line, Buffer *buffer) {
    if (line->size > 0 && line_cursor > 0) {
        if (line_cursor < line->size) {
            memmove(&line->buffer[line_cursor - 1],
                    &line->buffer[line_cursor],
                    (line->size - line_cursor) * sizeof(Letter*));
        }
        line->buffer[line->size - 1] = 0;

        line->size--;
        line_cursor--;
    } else if (line->size == 0 && buffer_cursor - 1 >= 0) {
        free(buffer->buffer[buffer_cursor]);

        if (buffer_cursor < buffer->size - 1) {
            memmove(&buffer->buffer[buffer_cursor],
                    &buffer->buffer[buffer_cursor + 1],
                    (buffer->size - buffer_cursor - 1) * sizeof(Line *));
        }

        buffer->size--;
        buffer_cursor--;
        line_cursor = buffer->buffer[buffer_cursor]->size;
    }
}


void update_camera(Camera2D *camera, float targetX, float targetY, float lerpSpeed, Line *line) {
    camera->target.x = Lerp(camera->target.x, targetX, lerpSpeed); 
    camera->target.y = Lerp(camera->target.y, targetY, lerpSpeed); 
}

void string_to_chars(char* string, Buffer *buffer) {
    TraceLog(LOG_INFO, TextFormat("string_to_chars -> %d", strlen(string)));
    for (int i = 0; i < strlen(string); i++) {
        TraceLog(LOG_INFO, TextFormat("%c", string[i]));
        if (string[i] == '\n') {
            // TraceLog(LOG_INFO, TextFormat("buffer.size -> %d\nbuffer_cursor -> %d", buffer.size, buffer_cursor));

            buffer->size++;

            if (buffer_cursor < buffer->size - 1) {
                memmove(&buffer->buffer[buffer_cursor + 1], 
                        &buffer->buffer[buffer_cursor], 
                        (buffer->size - buffer_cursor - 1) * sizeof(Line *));
            }

            buffer_cursor++;
            line_cursor = 0;

            buffer->buffer[buffer_cursor] = malloc(sizeof(Line));
            *buffer->buffer[buffer_cursor] = (Line) {0, {0}};
        }
        else {
            Letter letter = (Letter) {string[i], WHITE};
            write_before_cursor(buffer->buffer[buffer_cursor], letter);
        }
    }
}

void loadFile(char* filename){
    FILE *file = fopen(filename, "r");
    char fileContent[LINE_SIZE];
    if (file == NULL) {
        return;
    }
    while (fgets(fileContent, LINE_SIZE, file)) {
        string_to_chars(fileContent, &buffer);
    }

    fclose(file);  
    line_cursor = 0;
    buffer_cursor = 0;
    return;
}

void saveFile(char* filename){
    if (!filename) filename = "output.txt";
    FILE *file = fopen(filename, "w");

    if (!file) {
        TraceLog(LOG_ERROR, "Failed to open file!");
    } else {
        for (int i = 0; i < buffer.size; i++) {
            for (int j = 0; j < buffer.buffer[i]->size; j++) {
                fwrite(&buffer.buffer[i]->buffer[j]->key, 1, 1, file);
            }
            if (i < buffer.size-1) fwrite(&(char){'\n'}, 1, 1, file);
        }

        fclose(file);
    
        TraceLog(LOG_INFO, "File saved!");
    }
}

int main(int argc, char* argv[]){
    char* file = NULL;
    if (argc >= 1) {
        file = argv[1];
    }
    if (file) {
        loadFile(file);
    }
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    //SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 600, "Notepad");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.target = (Vector2) {line_cursor*FONT_SIZE, buffer_cursor*FONT_SIZE};
    camera.offset = (Vector2) {GetScreenWidth() / 2.f, (GetScreenHeight() / 2.f)-FONT_SIZE};
    camera.zoom = 1.f;
    // TraceLog(LOG_INFO, GetWorkingDirectory());
    Font font = LoadFontEx("resources/AnonymousPro-Regular.ttf", FONT_SIZE, 0, 888);
    
    while (!WindowShouldClose()) {
        
        update_camera(&camera, line_cursor*FONT_SIZE/2.f, buffer_cursor * FONT_SIZE, .05, buffer.buffer[buffer_cursor]);
        camera.offset = (Vector2) {GetScreenWidth() / 2.f, (GetScreenHeight() / 2.f)-FONT_SIZE*2};

        BeginDrawing();
        ClearBackground(BG_COLOR);
        BeginMode2D(camera);
        bool shift_down = IsKeyDown(KEY_LEFT_SHIFT);
        int key = GetKeyPressed();
        int mwheel = GetMouseWheelMove();
        if (mwheel) {
            if (mwheel < 0) {
                if (buffer_cursor < buffer.size - 1) buffer_cursor += 1;
                if (buffer.buffer[buffer_cursor]->size < line_cursor) line_cursor = buffer.buffer[buffer_cursor]->size;
            }
            else {
                if (buffer_cursor > 0) buffer_cursor -= 1;
                if (buffer.buffer[buffer_cursor]->size < line_cursor) line_cursor = buffer.buffer[buffer_cursor]->size;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE)) remove_char(buffer.buffer[buffer_cursor], &buffer);
        if (key){
            // TraceLog(LOG_INFO, TextFormat("%d", key));
            if (key == KEY_ENTER) {
                buffer.size++;

                if (buffer_cursor < buffer.size - 1) {
                    memmove(&buffer.buffer[buffer_cursor + 1], 
                            &buffer.buffer[buffer_cursor], 
                            (buffer.size - buffer_cursor - 1) * sizeof(Line *));
                }

                buffer_cursor++;
                line_cursor = 0;

                buffer.buffer[buffer_cursor] = malloc(sizeof(Line));
                *buffer.buffer[buffer_cursor] = (Line) {0, {0}};

                // TraceLog(LOG_INFO, TextFormat("buffer.size -> %d\nbuffer_cursor -> %d", buffer.size, buffer_cursor));
            }
            else if (key == KEY_TAB) {
                write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {' ', WHITE});
                write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {' ', WHITE});
                write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {' ', WHITE});
                write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {' ', WHITE});
            }
            else if(key == KEY_LEFT) {
                if (line_cursor > 0) line_cursor -= 1;
            }
            else if(key == KEY_RIGHT) {
                if (line_cursor < buffer.buffer[buffer_cursor]->size) line_cursor += 1;
            }
            else if(key == KEY_UP) {
                if (buffer_cursor > 0) buffer_cursor -= 1;
                if (buffer.buffer[buffer_cursor]->size < line_cursor) line_cursor = buffer.buffer[buffer_cursor]->size;
            }
            else if(key == KEY_DOWN) {
                if (buffer_cursor < buffer.size - 1) buffer_cursor += 1;
                if (buffer.buffer[buffer_cursor]->size < line_cursor) line_cursor = buffer.buffer[buffer_cursor]->size;
            }
            else {
                if (shift_down) {
                    // TraceLog(LOG_INFO, TextFormat("shift -> %d", shift_down));
                    if (key == '1') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'!', WHITE});
                    else if (key == '2') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'@', WHITE});
                    else if (key == '3') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'#', WHITE});
                    else if (key == '4') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'$', WHITE});
                    else if (key == '5') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'%', WHITE});
                    else if (key == '6') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'^', WHITE});
                    else if (key == '7') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'&', WHITE});
                    else if (key == '8') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'*', WHITE});
                    else if (key == '9') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'(', WHITE});
                    else if (key == '0') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {')', WHITE});
                    else if (key == '=') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'+', WHITE});
                    else if (key == '-') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'_', WHITE});
                    else if (key == ',') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'<', WHITE});
                    else if (key == '/') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'?', WHITE});
                    else if (key == ';') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {':', WHITE});
                    else if (key == '\'') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'"', WHITE});
                    else if (key == '.') 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {'>', WHITE});
                    else if ((key >= 32 && key < 48) || (key >= 65 && key <= 90)){
                        // TraceLog(LOG_INFO, TextFormat("shift -> %d", shift_down));
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {key, WHITE});
                    }
                    else if (key > 90 && key <= 95){
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {key+32, WHITE});
                    }
                } else if (IsKeyDown(KEY_LEFT_CONTROL) && key == KEY_S) {
                    saveFile(file);
                } else {
                    if (key >= 65 && key <= 90)
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {key+32, WHITE});
                    else if ((key >= 32 && key < 65) || (key > 90 && key <= 122)) 
                        write_before_cursor(buffer.buffer[buffer_cursor], (Letter) {key});
                }
            }
        }
        display_text(font);
        draw_cursor(); 
        EndMode2D();
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
