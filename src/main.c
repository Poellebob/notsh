#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

char *get_xdg_data_home(void) {
  const char *xdg = getenv("XDG_DATA_HOME");
  if (xdg && xdg[0] != '\0') {
    return (char *)xdg;
  }

  const char *home = getenv("HOME");
  if (!home || home[0] == '\0') {
    return NULL;
  }

  static char path[4096];
  snprintf(path, sizeof(path), "%s/.local/share", home);
  return path;
}

char *create_temp(void) {
  static char tmpTemplate[4096];
  snprintf(tmpTemplate, sizeof(tmpTemplate), "/tmp/note-XXXXXX");
  int fd = mkstemp(tmpTemplate);
  if (fd == -1) {
    perror("mkstemp");
    return NULL;
  }
  close(fd);

  static char tmpFile[4096];
  snprintf(tmpFile, sizeof(tmpFile), "%s.md", tmpTemplate);
  if (rename(tmpTemplate, tmpFile) != 0) {
    perror("rename");
    unlink(tmpTemplate);
    return NULL;
  }
  return tmpFile;
}

int open_in_editor(const char *filePath) {
  const char *editor = getenv("EDITOR");
  if (!editor || editor[0] == '\0') {
    editor = "vi";
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    execlp(editor, editor, filePath, NULL);
    perror("execlp");
    _exit(1);
  }

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    return 1;
  }

  if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
    fprintf(stderr, "Editor exited with error code %d\n", WEXITSTATUS(status));
    return 1;
  }
  return 0;
}

void ask_filename(char *out, size_t out_size) {
  char input[256];
  int got_name = 0;

  while (!got_name) {
    printf("Enter a name for the note (press Enter for timestamp): ");
    if (!fgets(input, sizeof(input), stdin)) {
      fprintf(stderr, "EOF received, aborting.\n");
      exit(1);
    }

    input[strcspn(input, "\n")] = '\0';

    int is_empty = 1;
    for (char *p = input; *p; p++) {
      if (*p != ' ' && *p != '\t') {
        is_empty = 0;
        break;
      }
    }

    if (is_empty) {
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      snprintf(out, out_size, "%d-%02d-%02d-%02d_%02d_%02d.md",
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
               tm.tm_hour, tm.tm_min, tm.tm_sec);
      got_name = 1;
    } else {
      for (char *p = input; *p; p++) {
        if (*p == ' ') *p = '-';
      }
      snprintf(out, out_size, "%s.md", input);
      got_name = 1;
    }
  }
}

void list_notes(const char *notes_dir) {
  DIR *dir = opendir(notes_dir);
  if (!dir) {
    perror("opendir");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      char *name = entry->d_name;
      size_t len = strlen(name);

      if (len > 3 && strcmp(name + len - 3, ".md") == 0) {
        name[len - 3] = '\0';
        printf("%s\n", name);
      } else {
        printf("%s\n", name);
      }
    }
  }
  closedir(dir);
}

int open_note(const char *notes_dir, const char *name) {
  char filepath[4096];
  snprintf(filepath, sizeof(filepath), "%s/%s.md", notes_dir, name);

  struct stat st;
  if (stat(filepath, &st) == -1) {
    return 1;
  }

  return open_in_editor(filepath);
}

void show_help(const char *progname) {
  printf("Usage: %s [command] [arguments]\n\n", progname);
  printf("Commands:\n");
  printf("  (no arguments)   Create a new note\n");
  printf("  open <name>      Open (or create) an existing note\n");
  printf("  list             List all notes (without .md extension)\n");
  printf("  --help           Show this help message\n");
}

int main(int argc, char *argv[]) {
  char *dataHome = get_xdg_data_home();
  if (!dataHome) {
    fprintf(stderr, "Could not determine XDG_DATA_HOME or HOME\n");
    return 1;
  }

  char notesDir[4096];
  snprintf(notesDir, sizeof(notesDir), "%s/notsh", dataHome);

  struct stat st;
  if (stat(notesDir, &st) == -1) {
    if (mkdir(notesDir, 0755) == -1) {
      perror("mkdir");
      return 1;
    }
  }

  if (argc == 1) {
    char *tempfile = create_temp();
    if (!tempfile) return 1;

    if (open_in_editor(tempfile) != 0) {
      unlink(tempfile);
      return 1;
    }

    struct stat st_temp;
    if (stat(tempfile, &st_temp) == 0 && st_temp.st_size == 0) {
      printf("Note is empty, not saving.\n");
      unlink(tempfile);
      return 0;
    }

    char filename[256];
    ask_filename(filename, sizeof(filename));

    char destPath[4096];
    snprintf(destPath, sizeof(destPath), "%s/%s", notesDir, filename);

    if (rename(tempfile, destPath) != 0) {
      perror("rename");
      unlink(tempfile);
      return 1;
    }

    printf("Note saved as: %s\n", destPath);
    return 0;
  }

  if (strcmp(argv[1], "open") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Error: missing note name.\nUsage: %s open <name>\n", argv[0]);
      return 1;
    }

    char name[256];
    strncpy(name, argv[2], sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    for (char *p = name; *p; p++) {
      if (*p == ' ') *p = '-';
    }
    
    if (open_note(notesDir, name)){ 
      printf("Error: invalid name");
      return 1;
    };
    return 0;
  }

  if (strcmp(argv[1], "list") == 0) {
    list_notes(notesDir);
    return 0;
  }

  if (strcmp(argv[1], "--help") == 0) {
    show_help(argv[0]);
    return 0;
  }

  fprintf(stderr, "Unknown command: %s\n", argv[1]);
  show_help(argv[0]);
  return 2;
}
