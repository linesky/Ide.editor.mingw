#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_ENTRY "new.c"
//sudo apt-get install gtk+3.0
//sudo apt-get install gtk+-3.0
//gcc -o ide idelinux.c `pkg-config --cflags --libs gtk+-3.0`
// Função para substituir a extensão do arquivo
void replace_extension(char *filename, const char *new_ext) {
    char *dot = strrchr(filename, '.');
    if (dot) {
        strcpy(dot, new_ext);
    } else {
        strcat(filename, new_ext);
    }
}

// Função para carregar o arquivo
void load_file(const char *filename, GtkTextBuffer *buffer) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *content = (char *)malloc(length + 1);
        fread(content, 1, length, file);
        content[length] = '\0';
        gtk_text_buffer_set_text(buffer, content, -1);
        free(content);
        fclose(file);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Could not open file: %s", filename);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Função para salvar o arquivo
void save_file(const char *filename, GtkTextBuffer *buffer) {
    FILE *file = fopen(filename, "w");
    if (file) {
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        fwrite(text, 1, strlen(text), file);
        fclose(file);
        g_free(text);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Could not save file: %s", filename);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Callback para o botão "New"
void on_btn_new_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
    gtk_text_buffer_set_text(buffer, "", -1);
}

// Callback para o botão "Load"
void on_btn_load_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(((gpointer *)user_data)[0]);
    GtkEntry *entry = GTK_ENTRY(((gpointer *)user_data)[1]);
    const char *filename = gtk_entry_get_text(entry);
    load_file(filename, buffer);
}

// Callback para o botão "Save"
void on_btn_save_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(((gpointer *)user_data)[0]);
    GtkEntry *entry = GTK_ENTRY(((gpointer *)user_data)[1]);
    const char *filename = gtk_entry_get_text(entry);
    save_file(filename, buffer);
}

// Callback para o botão "Compile"
void on_btn_compile_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    char filename[260];
    strcpy(filename, gtk_entry_get_text(entry));
    char command[300];
    sprintf(command, "gcc %s -o ", filename);
    replace_extension(filename, ".out");
    strcat(command, filename);
    system(command);
}

// Callback para o botão "Run"
void on_btn_run_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    char filename[260];
    strcpy(filename, gtk_entry_get_text(entry));
    replace_extension(filename, ".out");
    char command[300];
    sprintf(command, "xterm -e ./%s &", filename);
    system(command);
}

// Função principal
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Criação da janela principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "IDE");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Criação do container principal
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Definição da cor de fundo
    GdkRGBA yellow = {1, 1, 0, 1};
    gtk_widget_override_background_color(vbox, GTK_STATE_FLAG_NORMAL, &yellow);

    // Criação da área de texto
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Criação da entrada para o nome do arquivo
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), FILENAME_ENTRY);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // Criação dos botões
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *btn_new = gtk_button_new_with_label("New");
    g_signal_connect(btn_new, "clicked", G_CALLBACK(on_btn_new_clicked), buffer);
    gtk_box_pack_start(GTK_BOX(hbox), btn_new, FALSE, FALSE, 0);

    GtkWidget *btn_load = gtk_button_new_with_label("Load");
    gpointer load_data[2] = { buffer, entry };
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_btn_load_clicked), load_data);
    gtk_box_pack_start(GTK_BOX(hbox), btn_load, FALSE, FALSE, 0);

    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    gpointer save_data[2] = { buffer, entry };
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_btn_save_clicked), save_data);
    gtk_box_pack_start(GTK_BOX(hbox), btn_save, FALSE, FALSE, 0);

    GtkWidget *btn_compile = gtk_button_new_with_label("Compile");
    g_signal_connect(btn_compile, "clicked", G_CALLBACK(on_btn_compile_clicked), entry);
    gtk_box_pack_start(GTK_BOX(hbox), btn_compile, FALSE, FALSE, 0);

    GtkWidget *btn_run = gtk_button_new_with_label("Run");
    g_signal_connect(btn_run, "clicked", G_CALLBACK(on_btn_run_clicked), entry);
    gtk_box_pack_start(GTK_BOX(hbox), btn_run, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

