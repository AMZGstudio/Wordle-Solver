#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

// Tweaks
#define DIVIDE_AMOUNT 2.3
#define PERFER_WORDS_AMOUNT 1.10
#define AMOUNT_EXTRA_PER_ROUND 1.10

#define NUM_TOTAL_WORDS 12972
#define NUM_BEST_WORDS 4630
#define BUF_SIZE 15

#define NUM_TRIES 5
#define WORD_SIZE 5
#define NO_WORD_FOUND -1
#define DETAILED false

typedef struct {
    char _word[6];
    float priority;
    bool qualifies;
} Word;

/*~~~~~~~~~~~~~~~~~~~~~~~ USER INPUT FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~*/
void getString(char* string)
{
    fgets(string, BUF_SIZE, stdin);
    string[strcspn(string, "\n")] = 0;
    for (; *string; ++string) *string = tolower(*string); // converts string to lower case
}

void getResult(char* result)
{
    while (strlen(result) != 5)
    {
        printf("Enter result: (y-yellow, g-green, b-black)\n");
        getString(result);
    }
}

void getWord(char* input, Word* words)
{
    while (true)
    {
        printf("Enter word:\n");
        while (strlen(input) != WORD_SIZE) getString(input);

        for (int i = 0; i < NUM_TOTAL_WORDS; i++)
            if (strcmp(input, words[i]._word) == 0)
                return;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~ WORD MANAGEMENT FUCNTIONS ~~~~~~~~~~~~~~~~~~~~~~*/
float getWordPoints(const char* word, Word* bestWords)
{
    float points = 0, eachLetterValue[26] = { 43.31, 10.56, 23.13, 17.25, 56.88, 9.24, 12.59, 15.31, 38.45, 1.01, 5.61, 27.98, 15.36, 33.92, 36.51, 16.14, 1.00, 38.64, 29.23, 35.43, 18.51, 5.13, 6.57, 1.48, 9.06, 1.39 };

    //printf("Word: %s\n", word);

    // loop through letters
    for (int i = 0; i < 5; i++)
    {
        int letterIndex = word[i] - 97;
        float letterPoints = eachLetterValue[letterIndex];

        // count the amount of times that letter appeared before this time.
        int amntLetterExists = 0;
        for (int j = i - 1; j >= 0; j--)
            if (letterIndex == word[j] - 97) // check if the letter at point j equals letterIndex.
                amntLetterExists++;

        // for every time that letter was previosuly seen, give less points;
        for (int t = 0; t < amntLetterExists; t++)
            letterPoints = letterPoints / DIVIDE_AMOUNT;

        points += letterPoints;

        for (int f = 0; f < NUM_BEST_WORDS; f++)
            if (!strcmp(word, bestWords[f]._word))
                points *= PERFER_WORDS_AMOUNT;
    }
    return points;
}

Word* loadWords(const char* path, bool calculatePoints, Word* bestWords)
{
    Word* words = (Word*)malloc(sizeof(Word) * NUM_TOTAL_WORDS);

    FILE* f = fopen(path, "r");
    if (f != NULL)
    {
        for (int i = 0; i < NUM_TOTAL_WORDS; i++)
        {
            char buff[BUF_SIZE] = { 0 };
            fgets(buff, BUF_SIZE, f);
            buff[strcspn(buff, "\n")] = 0;

            strcpy(words[i]._word, buff);

            words[i].priority = calculatePoints ? getWordPoints(buff, bestWords) : -1;
            words[i].qualifies = true;
        }
        fclose(f);
    }
    return words;
}

void swap(Word* w1, Word* w2)
{
    Word temp = *w2;
    *w2 = *w1;
    *w1 = temp;
}

void quicksort(Word* words, int first, int last)
{
    Word temp; int pivot, i, j;
    if (first < last)
    {
        pivot = first, i = first, j = last;
        while (i < j)
        {
            while (words[i].priority <= words[pivot].priority && i < last) i++;
            while (words[j].priority > words[pivot].priority) j--;

            if (i < j) swap(words + i, words + j);
        }
        swap(words + pivot, words + j);
        quicksort(words, first, j - 1);
        quicksort(words, j + 1, last);
    }
}

void findIllegalWords(Word* words, char* blacklist, char* greenlist, char* yellowLetters) // made this function half as big as original (21 vs 42)
{
    for (int i = 0; i < NUM_TOTAL_WORDS; i++) // each word  
    {
        for (int j = 0; j < 5; j++) // each letter
        {
            if (isalpha(greenlist[j]) && words[i]._word[j] != greenlist[j])// each green letter
                words[i].qualifies = false;

            if (strchr(blacklist, words[i]._word[j]) && !strchr(yellowLetters, words[i]._word[j])) // if the letter exists in the blacklist, and does not exist in the yellow list
                words[i].qualifies = false;
        }
        for (int y = 0; y < NUM_TRIES; y++) // loop through yellow letters
            for (int x = 0; x < WORD_SIZE; x++)
                // check to see if a letter exists there
                if (isalpha(yellowLetters[y * WORD_SIZE + x]))
                    // nullify the word if the letter does not exist in the word, or it exists in the same spot as the yellow letter
                    if ((!strchr(words[i]._word, yellowLetters[y * WORD_SIZE + x])) || (words[i]._word[x] == yellowLetters[y * WORD_SIZE + x]))
                        words[i].qualifies = false;
    }
}

void incBetterWords(Word* totalWords, Word* bestWords)
{
    for (int i = 0; i < NUM_TOTAL_WORDS; i++)
        for (int j = 0; j < NUM_BEST_WORDS; j++)
            if (!strcmp(totalWords[i]._word, bestWords[j]._word))
                totalWords[i].priority *= AMOUNT_EXTRA_PER_ROUND;
}
/*~~~~~~~~~~~~~~~~~~~~~ BACKEND FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~*/
void initalizeMemory(char* yellowlist)
{
    for (int i = 0; i < NUM_TRIES * WORD_SIZE; i++) // yellow letters
        yellowlist[i] = ' ';
}

void AssignResult(char* wordInput, char* resultInput, char* blackList, char* greenlist, char* yellowLetters)
{
    for(int i = 0; i < WORD_SIZE; i++)
    {
        if (resultInput[i] == 'b' && !strchr(blackList, wordInput[i]))
            strncat(blackList, &wordInput[i], 1);
        
        else if (resultInput[i] == 'g')
            greenlist[i] = wordInput[i];
        
        else if (resultInput[i] == 'y')
            yellowLetters[i] = wordInput[i];
    }
}

int getLegalWordIndex(Word* totalWords, int lastIndex)
{
    for (int i = lastIndex-1; i >= 0; i--)
        if (totalWords[i].qualifies)
            return i;

    return NO_WORD_FOUND;
}

void printWords(Word* totalWords)
{
    int amountWords = 0;
    int word1 = getLegalWordIndex(totalWords, NUM_TOTAL_WORDS);
    int word2 = getLegalWordIndex(totalWords, word1);
    int word3 = getLegalWordIndex(totalWords, word2);

    Word first = totalWords[word1], second = totalWords[word2], third = totalWords[word3];

    if (DETAILED) printf("All legal words:\n");
    for (int i = 0; i < NUM_TOTAL_WORDS; i++)
    {
        if (!totalWords[i].qualifies)
            continue;

        amountWords++;
        if (DETAILED) printf("%s %.3f\n", totalWords[i]._word, totalWords[i].priority);
    }
    if (amountWords == 1)
        printf("You won!\n");
    else
    {
        printf("\nAmount of Word Possibilites is: %d\nTop picks (bigger number is better): \n", amountWords);
        if (word1 != NO_WORD_FOUND) printf("-%s %.3f\n", first._word, first.priority);
        if (word2 != NO_WORD_FOUND) printf("-%s %.3f\n", second._word, second.priority);
        if (word3 != NO_WORD_FOUND) printf("-%s %.3f\n", third._word, third.priority);
    }
}

int main()
{
    printf("Wordle-Solver made by AMZG studios!\nCalculating...\n\n");

    Word* bestWords = loadWords("best_words.txt", false, NULL);
    Word* totalWords = loadWords("total_words.txt", true, bestWords);
    quicksort(totalWords, 0, NUM_TOTAL_WORDS-1);

    do
    {
        char greenlist[WORD_SIZE+1] = {0}, blacklist[27] = {0}, yellowlist[NUM_TRIES*WORD_SIZE+1] = {0};
        initalizeMemory(yellowlist);
        
        printf("Use the Word 'arise' or 'irate' to begin!\n");

        for (int round = 0; round < NUM_TRIES; round++)
        {
            char word[BUF_SIZE] = { 0 }, result[BUF_SIZE] = {0};

            getWord(word, totalWords);
            getResult(result);
            AssignResult(word, result, blacklist, greenlist, yellowlist+(WORD_SIZE*round));
            
            findIllegalWords(totalWords, blacklist, greenlist, yellowlist);
            incBetterWords(totalWords, bestWords);
            
            printWords(totalWords);
            if (DETAILED) printf("Blacklist: %s\nYellow list: %s\nGreen list: %s\n", blacklist, yellowlist, greenlist);
        }
        printf("To play again, press y:");
        
    } while(_getch() == 'y');
    return 0;
}