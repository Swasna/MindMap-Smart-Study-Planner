#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Topic {
    int id;
    char subject[50];
    char topic[50];
    int difficulty;
    int daysLeft;
    int completed;
    struct Topic *next;
};

struct Topic *head = NULL;
int nextId = 1;

void saveTopics() {
    FILE *file = fopen("study_data.txt", "w");
    struct Topic *temp = head;

    if (file == NULL) {
        printf("Error saving data!\n");
        return;
    }

    while (temp != NULL) {
        fprintf(file, "%d|%s|%s|%d|%d|%d\n",
                temp->id,
                temp->subject,
                temp->topic,
                temp->difficulty,
                temp->daysLeft,
                temp->completed);

        temp = temp->next;
    }

    fclose(file);
}

void loadTopics() {
    FILE *file = fopen("study_data.txt", "r");
    struct Topic *newTopic;

    if (file == NULL)
        return;

    while (1) {
        newTopic = malloc(sizeof(struct Topic));

        if (newTopic == NULL)
            break;

        if (fscanf(file, "%d|%49[^|]|%49[^|]|%d|%d|%d\n",
                   &newTopic->id,
                   newTopic->subject,
                   newTopic->topic,
                   &newTopic->difficulty,
                   &newTopic->daysLeft,
                   &newTopic->completed) != 6) {
            free(newTopic);
            break;
        }

        newTopic->next = head;
        head = newTopic;

        if (newTopic->id >= nextId)
            nextId = newTopic->id + 1;
    }

    fclose(file);
}

void addTopic() {
    struct Topic *newTopic;
    struct Topic *temp;

    newTopic = malloc(sizeof(struct Topic));

    if (newTopic == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    newTopic->id = nextId++;

    printf("Enter subject name: ");
    scanf(" %49[^\n]", newTopic->subject);

    printf("Enter topic name: ");
    scanf(" %49[^\n]", newTopic->topic);

    printf("Difficulty (1-Easy, 2-Medium, 3-Hard): ");
    if (scanf("%d", &newTopic->difficulty) != 1 ||
        newTopic->difficulty < 1 ||
        newTopic->difficulty > 3) {
        printf("Invalid difficulty!\n");
        while (getchar() != '\n');
        free(newTopic);
        return;
    }

    printf("Days left for exam: ");
    if (scanf("%d", &newTopic->daysLeft) != 1 ||
        newTopic->daysLeft < 0) {
        printf("Invalid number of days!\n");
        while (getchar() != '\n');
        free(newTopic);
        return;
    }

    newTopic->completed = 0;
    newTopic->next = NULL;

    if (head == NULL) {
        head = newTopic;
    } else {
        temp = head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newTopic;
    }

    saveTopics();
    printf("Topic added successfully! ID: %d\n", newTopic->id);
}

void displayTopics() {
    struct Topic *temp = head;

    if (temp == NULL) {
        printf("No study topics available.\n");
        return;
    }

    printf("\n----------- ALL STUDY TOPICS -----------\n");

    while (temp != NULL) {
        printf("\nID: %d\n", temp->id);
        printf("Subject: %s\n", temp->subject);
        printf("Topic: %s\n", temp->topic);

        printf("Difficulty: ");
        if (temp->difficulty == 1)
            printf("Easy\n");
        else if (temp->difficulty == 2)
            printf("Medium\n");
        else
            printf("Hard\n");

        printf("Days left: %d\n", temp->daysLeft);
        printf("Status: %s\n",
               temp->completed ? "Completed" : "Pending");

        temp = temp->next;
    }
}

void studyPlan() {
    struct Topic *temp;
    struct Topic *current;
    struct Topic *next;
    struct Topic *sorted = NULL;

    if (head == NULL) {
        printf("No study topics available.\n");
        return;
    }

    temp = head;

    while (temp != NULL) {
        current = malloc(sizeof(struct Topic));

        if (current == NULL) {
            printf("Memory allocation failed!\n");

            while (sorted != NULL) {
                next = sorted->next;
                free(sorted);
                sorted = next;
            }

            return;
        }

        *current = *temp;
        current->next = NULL;

        if (!current->completed) {
            if (sorted == NULL ||
                (current->daysLeft * 10 - current->difficulty * 3) <
                (sorted->daysLeft * 10 - sorted->difficulty * 3)) {
                current->next = sorted;
                sorted = current;
            } else {
                next = sorted;

                while (next->next != NULL &&
                       (next->next->daysLeft * 10 -
                        next->next->difficulty * 3) <=
                       (current->daysLeft * 10 -
                        current->difficulty * 3)) {
                    next = next->next;
                }

                current->next = next->next;
                next->next = current;
            }
        } else {
            free(current);
        }

        temp = temp->next;
    }

    printf("\n----------- SMART STUDY PLAN -----------\n");

    if (sorted == NULL)
        printf("All topics are completed!\n");

    while (sorted != NULL) {
        printf("\nSubject: %s\n", sorted->subject);
        printf("Topic: %s\n", sorted->topic);
        printf("Days left: %d\n", sorted->daysLeft);
        printf("Difficulty: %d/3\n", sorted->difficulty);

        sorted->next = sorted->next;
        current = sorted;
        sorted = sorted->next;
        free(current);
    }
}

void markCompleted() {
    int id;
    struct Topic *temp = head;

    printf("Enter topic ID to mark completed: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID!\n");
        while (getchar() != '\n');
        return;
    }

    while (temp != NULL) {
        if (temp->id == id) {
            temp->completed = 1;
            saveTopics();
            printf("Topic marked as completed!\n");
            return;
        }

        temp = temp->next;
    }

    printf("Topic ID not found!\n");
}

int main() {
    int choice;
    struct Topic *temp;
    struct Topic *next;

    loadTopics();

    do {
        printf("\n===== MINDMAP: SMART STUDY PLANNER =====\n");
        printf("1. Add Study Topic\n");
        printf("2. View All Topics\n");
        printf("3. Generate Smart Study Plan\n");
        printf("4. Mark Topic as Completed\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addTopic();
                break;

            case 2:
                displayTopics();
                break;

            case 3:
                studyPlan();
                break;

            case 4:
                markCompleted();
                break;

            case 5:
                saveTopics();
                printf("Thank you for using MindMap!\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 5);

    temp = head;

    while (temp != NULL) {
        next = temp->next;
        free(temp);
        temp = next;
    }

    return 0;
}

