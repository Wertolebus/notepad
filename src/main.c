#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE       1024
#define BUFFER_SIZE     1024
#define FONT_SIZE       48
#define FONT_SPACING    (int) -(FONT_SIZE/2) 

typedef struct {
    int size;
    int buffer[LINE_SIZE];
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

void write_before_cursor(Line *line, int key){
    if (line_cursor < LINE_SIZE) {
        TraceLog(LOG_INFO, TextFormat("key -> %d", key));

        
    if (line_cursor < line->size) {
        memmove(&line->buffer[line_cursor + 1], 
                &line->buffer[line_cursor],     
                (line->size - line_cursor) * sizeof(int)); 
        }

        
    line->buffer[line_cursor] = key;

    
    line_cursor++;
    line->size++;

    trace_line(line); 
    }
}

void display_text(Font font){
    Vector2 pen = {0};
    for (int line = 0; line < buffer.size; line++){
        for (int chr = 0; chr < buffer.buffer[line]->size; chr++){
            DrawTextEx(font, TextFormat("%c", buffer.buffer[line]->buffer[chr]), pen, FONT_SIZE, FONT_SPACING, WHITE); 
            pen.x += FONT_SIZE + FONT_SPACING;
        }
        pen.x = 0;
        pen.y += FONT_SIZE;
    }
}

void draw_cursor(){
    timer++;
    if (timer < 30) DrawRectangle(line_cursor*(FONT_SIZE + FONT_SPACING), buffer_cursor*(FONT_SIZE), 3, FONT_SIZE, WHITE);
    else if (timer > 60) timer = 0;
}

void remove_char(Line *line, Buffer *buffer) {
    if (line->size > 0 && line_cursor > 0) {
        if (line_cursor < line->size) {
            memmove(&line->buffer[line_cursor - 1], 
                    &line->buffer[line_cursor], 
                    (line->size - line_cursor) * sizeof(int));
        }

        line->size--;
        line_cursor--;
    } 
    else if (line->size == 0 && buffer_cursor - 1 >= 0) {
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


int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Notepad");
    SetTargetFPS(60);
    
    TraceLog(LOG_INFO, GetWorkingDirectory());
    Font font = LoadFontEx("resources/AnonymousPro-Regular.ttf", FONT_SIZE, 0, 888);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        bool shift_down = IsKeyDown(KEY_LEFT_SHIFT);
        int key = GetKeyPressed();
        if (key){
            TraceLog(LOG_INFO, TextFormat("%d", key));
            if(key == KEY_BACKSPACE)
                remove_char(buffer.buffer[buffer_cursor], &buffer);
            else if (key == KEY_ENTER) {
                TraceLog(LOG_INFO, TextFormat("buffer.size -> %d\nbuffer_cursor -> %d", buffer.size, buffer_cursor));

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

                TraceLog(LOG_INFO, TextFormat("buffer.size -> %d\nbuffer_cursor -> %d", buffer.size, buffer_cursor));
            }
            else if (key == KEY_TAB) {
                write_before_cursor(buffer.buffer[buffer_cursor], ' ');
                write_before_cursor(buffer.buffer[buffer_cursor], ' ');
                write_before_cursor(buffer.buffer[buffer_cursor], ' ');
                write_before_cursor(buffer.buffer[buffer_cursor], ' ');
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
                    TraceLog(LOG_INFO, TextFormat("shift -> %d", shift_down));
                    if (key == '1') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '!');
                    else if (key == '2') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '@');
                    else if (key == '3') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '#');
                    else if (key == '4') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '$');
                    else if (key == '5') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '%');
                    else if (key == '6') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '^');
                    else if (key == '7') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '&');
                    else if (key == '8') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '*');
                    else if (key == '9') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '(');
                    else if (key == '0') 
                        write_before_cursor(buffer.buffer[buffer_cursor], ')');
                    else if (key == '=') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '+');
                    else if (key == '-') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '_');
                    else if (key == ',') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '<');
                    else if (key == '/') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '?');
                    else if (key == ';') 
                        write_before_cursor(buffer.buffer[buffer_cursor], ':');
                    else if (key == '\'') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '"');
                    else if (key == '.') 
                        write_before_cursor(buffer.buffer[buffer_cursor], '>');
                    else if ((key >= 32 && key < 48) || (key >= 65 && key <= 90)){
                        TraceLog(LOG_INFO, TextFormat("shift -> %d", shift_down));
                        write_before_cursor(buffer.buffer[buffer_cursor], key);
                    }
                    else if (key > 90 && key <= 95){
                        write_before_cursor(buffer.buffer[buffer_cursor], key+32);
                    }
                }
                else {
                    if (key >= 65 && key <= 90)
                        write_before_cursor(buffer.buffer[buffer_cursor], key+32);
                    else if ((key >= 32 && key < 65) || (key > 90 && key <= 122)) 
                        write_before_cursor(buffer.buffer[buffer_cursor], key);
                }
            }
        }
        display_text(font);
        draw_cursor();    
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
