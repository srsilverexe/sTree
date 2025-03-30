#define _DEFAULT_SOURCE

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"
#define COLOR_RESET "\x1b[0m"

struct node {
	char *name;
	unsigned char type;
	struct node *next;
};

struct node *_split(struct node *head);
struct node *_merge(struct node *a, struct node *b);
struct node *mergeSort(struct node *head);

void helpCommand();

void inspectFolder(const char *path,
				   size_t deep,
				   int maxRecursion,
				   int recursionsDone);

int main(int argc, char **argv) {
	char *path = NULL;
	int maxRecursion = -1;
	int recursionsDone = 0;

	for (size_t i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			helpCommand();
			if (path != NULL)
				free((void *)path);
			return 0;
		} else if (strcmp(argv[i], "-p") == 0) {
			if (i + 1 >= argc) {
				perror("Path not passed\n");
				if (path != NULL)
					free((void *)path);
				return 1;
			}
			path = strdup(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "-m") == 0) {
			if (i + 1 >= argc) {
				perror("Max recursion not passed\n");
				if (path != NULL)
					free((void *)path);
				return 1;
			}

			maxRecursion = atoi(argv[i + 1]);
			i++;
		}
	}

	if (path == NULL) {
		path = getcwd(NULL, 0);
		if (path == NULL) {
			perror("Error to get current dir\n");
			return 1;
		}
	}
	printf("Current working dir: %s\n", path);

	inspectFolder(path, 0, maxRecursion, 0);

	free((void *)path);
	return 0;
}

struct node *_split(struct node *head) {
	struct node *fast = head->next;
	struct node *slow = head;

	while (fast != NULL && fast->next != NULL) {
		fast = fast->next->next;
		slow = slow->next;
	}

	struct node *mid = slow->next;
	slow->next = NULL;
	return mid;
}

struct node *_merge(struct node *a, struct node *b) {
	if (a == NULL)
		return b;
	if (b == NULL)
		return a;

	if (strcmp(a->name, b->name) < 0) {
		a->next = _merge(a->next, b);
		return a;
	} else {
		b->next = _merge(a, b->next);
		return b;
	}
}

struct node *mergeSort(struct node *head) {
	if (head == NULL || head->next == NULL)
		return head;

	struct node *mid = _split(head);
	struct node *left = mergeSort(head);
	struct node *right = mergeSort(mid);

	return _merge(left, right);
}

void helpCommand() {
	printf("-h\tPrint all options\n");
	printf("-p\tPass path to inspect\n");
	printf("-m\tMax recursion\n");
}

void inspectFolder(const char *path,
				   size_t deep,
				   int maxRecursion,
				   int recursionsDone) {
	DIR *dir = opendir(path);
	if (dir == NULL) {
		perror("Error to open directory\n");
		return;
	}

	struct node *list = NULL;
	struct node **tail = &list;
	struct dirent *dirEntry = NULL;

	while ((dirEntry = readdir(dir)) != NULL) {
		if (strcmp(dirEntry->d_name, ".") == 0
			|| strcmp(dirEntry->d_name, "..") == 0) {
			continue;
		}

		struct node *newItem = malloc(sizeof(struct node));
		if (newItem == NULL) {
			perror("Error to allocate memory\n");
			closedir(dir);
			while (list != NULL) {
				struct node *temp = list;
				list = list->next;
				free(temp->name);
				free(temp);
			}
			return;
		}

		newItem->name = strdup(dirEntry->d_name);
		newItem->type = dirEntry->d_type;
		newItem->next = NULL;

		*tail = newItem;
		tail = &(*tail)->next;
	}

	closedir(dir);

	list = mergeSort(list);

	struct node *current = list;
	while (current != NULL) {
		for (size_t i = 0; i < deep; i++) {
			printf("\t");
		}

		switch (current->type) {
			case DT_REG:
				printf(COLOR_GREEN "%s\n" COLOR_RESET, current->name);
				break;
			case DT_DIR:
				printf(COLOR_BLUE "%s\n" COLOR_RESET, current->name);
				break;
			case DT_FIFO:
				printf(COLOR_MAGENTA "%s\n" COLOR_RESET, current->name);
				break;
			case DT_LNK:
				printf(COLOR_CYAN "%s\n" COLOR_RESET, current->name);
				break;
			default:
				printf(COLOR_WHITE "%s\n" COLOR_RESET, current->name);
		}

		if (current->type == DT_DIR) {
			if (maxRecursion <= -1 || recursionsDone < maxRecursion) {
				char *subPath =
					malloc(strlen(path) + strlen(current->name) + 2);
				if (subPath == NULL) {
					perror("Error to allocate memory to subPath\n");
					while (list) {
						struct node *temp = list;
						list = list->next;
						free((void *)temp->name);
						free(temp);
					}
					return;
				}
				sprintf(subPath, "%s/%s", path, current->name);
				inspectFolder(
					subPath, deep + 1, maxRecursion, recursionsDone + 1);
				free(subPath);
			}
		}

		current = current->next;
	}

	current = list;
	while (current != NULL) {
		struct node *temp = current;
		current = current->next;
		free(temp->name);
		free(temp);
	}
}
