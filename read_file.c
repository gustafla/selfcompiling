size_t read_file_to_str(char **dst, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    if (!len) {
        return 0; // Don't read and malloc an empty file
    }
    *dst = (char*)malloc(len+1);
    fseek(file, 0, SEEK_SET);

    size_t read = fread(*dst, sizeof(char), len, file);
    fclose(file);

    (*dst)[len] = '\0';

    if (read != len) {
        free(*dst);
        fprintf(stderr, "Failed to read file %s\n", filename);
        return 0;
    }
    return len;
}
