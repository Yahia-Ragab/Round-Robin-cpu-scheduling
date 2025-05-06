#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

gboolean is_valid_non_negative_integer(const char *str, int *value) {
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return FALSE;
    }
    *value = atoi(str);
    return *value >= 0;
}

void findWaitingTime(int n, int bt[], int wt[], int rt[], int quantum) {
    int rem_bt[n];
    for (int i = 0; i < n; i++) {
        rem_bt[i] = bt[i];
        wt[i] = 0;
        rt[i] = -1;
    }

    int t = 0;
    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = 0;
                if (rt[i] == -1) rt[i] = t;

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    wt[i] = t - bt[i];
                    rem_bt[i] = 0;
                }
            }
        }
        if (done) break;
    }
}

void findTurnAroundTime(int n, int bt[], int wt[], int tat[]) {
    for (int i = 0; i < n; i++) tat[i] = bt[i] + wt[i];
}

char *calculate_rr(char *burst_input, int quantum) {
    char *token = strtok(burst_input, " ");
    int bt[100], n = 0;

    while (token) {
        if (!is_valid_non_negative_integer(token, &bt[n])) return NULL;
        n++;
        token = strtok(NULL, " ");
    }

    if (n == 0) return NULL;

    int wt[n], tat[n], rt[n];
    findWaitingTime(n, bt, wt, rt, quantum);
    findTurnAroundTime(n, bt, wt, tat);

    float total_wt = 0, total_tat = 0, total_rt = 0;
    char *result = g_malloc0(2048);
    for (int i = 0; i < n; i++) {
        total_wt += wt[i];
        total_tat += tat[i];
        total_rt += rt[i];
        char line[256];
        snprintf(line, sizeof(line), "P[%d] Burst: %d, Wait: %d, Turnaround: %d, Response: %d\n",
                 i + 1, bt[i], wt[i], tat[i], rt[i]);
        strcat(result, line);
    }

    char avg[256];
    snprintf(avg, sizeof(avg), "\nAvg WT: %.2f, \nAvg TAT: %.2f, \nAvg RT: %.2f\n",total_wt / n, total_tat / n, total_rt / n);
    strcat(result, avg);
    return result;
}

void show_message_window(GtkApplication *app, const char *title, const char *message) {
    GtkWidget *win = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(win), title);
    gtk_window_set_default_size(GTK_WINDOW(win), 400, 200);
    GtkWidget *label = gtk_label_new(message);
    gtk_window_set_child(GTK_WINDOW(win), label);
    gtk_window_present(GTK_WINDOW(win));
}

static void on_submit(GtkWidget *widget, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;
    const char *burst_text = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const char *quantum_text = gtk_editable_get_text(GTK_EDITABLE(entries[1]));

    int quantum;
    if (!is_valid_non_negative_integer(quantum_text, &quantum)) {
        show_message_window(GTK_APPLICATION(entries[2]), "Error", "Wrong entry");
        return;
    }

    char *burst_copy = g_strdup(burst_text);
    char *result = calculate_rr(burst_copy, quantum);
    g_free(burst_copy);

    if (!result) {
        show_message_window(GTK_APPLICATION(entries[2]), "Error", "Wrong entry");
    } else {
        show_message_window(GTK_APPLICATION(entries[2]), "Result", result);
        g_free(result);
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Round Robin");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *entry1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), "Burst times (space-separated)");
    gtk_box_append(GTK_BOX(box), entry1);

    GtkWidget *entry2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Time quantum");
    gtk_box_append(GTK_BOX(box), entry2);

    GtkWidget **data = g_new(GtkWidget *, 3);
    data[0] = entry1;
    data[1] = entry2;
    data[2] = (GtkWidget *)app;

    GtkWidget *submit = gtk_button_new_with_label("Submit");
    g_signal_connect(submit, "clicked", G_CALLBACK(on_submit), data);
    gtk_box_append(GTK_BOX(box), submit);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.roundrobin", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
