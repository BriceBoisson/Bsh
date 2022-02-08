#include "lexer_tools.h"

char **split_in_words(char *input)
{
    char *new = strdup(input);
    char *save = new;
    char **words = NULL;
    int words_nb = 0;
    char *word = NULL;

    int i = 0;
    int word_len = 0;
    char quote = 0;
    while (input[i])
    {
        if (input[i] == '\'' || input[i] == '"')
        {
            if (quote == input[i])
                quote = 0;
            else if (!quote)
                quote = input[i];
        }
        if ((input[i] == ' ' || input[i] == '\t') && !quote)
        {
            if (word_len > 0)
            {
                word[word_len] = 0;
                words = realloc(words, sizeof(char *) * (words_nb + 2));
                words[words_nb] = word;
                words_nb++;
                word = NULL;
                word_len = 0;
            }
            i++;
        }
        else
        {
            word = realloc(word, sizeof(char) * (word_len + 2));
            word[word_len++] = input[i++];
        }
    }
    if (word_len > 0)
    {
        word[word_len] = 0;
        words = realloc(words, sizeof(char *) * (words_nb + 2));
        words[words_nb] = word;
        words_nb++;
    }
    if (!words)
        words = realloc(words, sizeof(char *));
    words[words_nb] = NULL;
    free(save);
    return words;
}
