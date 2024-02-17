#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input>", argv[0]);
        return 1;
    }

    char *input = argv[1];
    const char *output = "logid.cfg";

    FILE *outputFile = fopen(output, "w");
    if (outputFile == NULL)
    {
        printf("Failed to open output file: %s\n", output);
        return 1;
    }

    fprintf(outputFile, "%s", input);

    fclose(outputFile);

    return 0;
}
