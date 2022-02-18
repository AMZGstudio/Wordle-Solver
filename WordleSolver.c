#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "words.h"

#define INPUT_SIZE 7
#define ENGLISH_VOCAB_STRING 27
#define DIVIDE_AMOUNT 2.3
#define PERFER_WORDS_AMOUNT 1.10
#define AMOUNT_EXTRA_PER_ROUND 1.10
#define DEBUG_MODE 1

//reset variables
void resetVariables(float showingPriority[], char blacklist[], char greenLetters[], char yellowLettersAll[][ARRAY_SIZE], char array[][ARRAY_SIZE])
{
    for(int i=0; i<ARRAY_LENGTH; i++)
    {
        for(int t=0; t<5; t++)
        {
            array[i][t] = 0;
            greenLetters[t] = ' ';
            for(int g=0; g<5; g++)
                yellowLettersAll[t][g] = ' ';
        }
        showingPriority[i] = 0;
        strcat(array[i], words[i]);
    }
    for(int i=0; i<ENGLISH_VOCAB_STRING; i++)
        blacklist[i] = 0;
}
//function gets the results of the user input, and adds the required letters to the blacklist, greenlist, and yellowlist
void getResultAndAsign(char input[7], char blackList[], char greenLetters[], char yellowLetters[])
{
    char results[6] = {0};
    do
    {
        printf("Enter your results: (y for yellow g for green and b for black)\n");
        fgets(results, INPUT_SIZE, stdin);
        results[strcspn(results, "\n")] = 0;
    } while(strlen(results)!=5);

    char temporaryString[2] = {0};
    for(int i=0; i<5; i++)
    {
        int blackListContains=0;
        for(int d=0; d<26; d++)
        {
            if(blackList[d]==input[i])
                blackListContains=1;
        }
        if(results[i] == 'b' && blackListContains==0)
        {
            temporaryString[0] = input[i];
            strcat(blackList, temporaryString);
        }
        else if(results[i] == 'g')
            greenLetters[i] = input[i];
        
        else if(results[i] == 'y')
            yellowLetters[i] = input[i];
    }
}
//function get a word from the user
void getWordInput(char input[], char array[][ARRAY_SIZE])
{
    int notInList=1;
    while(notInList==1)
    {
        printf("Enter your five letter word:\n");
        fgets(input, INPUT_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;
        
        for(int i=0; i<ARRAY_LENGTH && notInList==1; i++)
        {
            if(strcmp(input, array[i])==0)
                notInList=0;
        }
    }
}
//function goes through the words in the dictionary, and calculates which of them follow the rules
void calculateAllowedWords(char array[][6], char blacklist[26], char greenLetters[], char yellowLettersAll[][ARRAY_SIZE])
{
    for(int i=0; i<ARRAY_LENGTH; i++)
    {   
        int illegalWord=0;
        //each letter in array
        for(int g=0; g<5; g++)
        {
            //each green letter
            if(greenLetters[g]!=' ' && array[i][g]!=greenLetters[g])
                illegalWord=1;

            //each blacklist letter
            for(int f=0; f<26; f++)
            {
                int overrideBlackList=0;
                //cycle through every yellow letter
                for(int c=0; c<5; c++)
                {
                    for(int m=0; m<5; m++)
                    {
                        if(blacklist[f]==yellowLettersAll[c][m] || (greenLetters[g]==blacklist[f]))
                             overrideBlackList=1;
                    }
                }
                if(array[i][g]==blacklist[f] && overrideBlackList==0) 
                    illegalWord=1;
            }
            //for each yellow letter, cycle through each rounds yellow letters
            for(int x=0; x<5; x++)
            {
                if(yellowLettersAll[x][g]!=' ' && array[i][g]==yellowLettersAll[x][g])
                     illegalWord=1;
                else if(yellowLettersAll[x][g]!=' ' && ((yellowLettersAll[x][g]!=array[i][0]) && (yellowLettersAll[x][g]!=array[i][1]) && (yellowLettersAll[x][g]!=array[i][2]) && (yellowLettersAll[x][g]!=array[i][3]) && (yellowLettersAll[x][g]!=array[i][4])))
                     illegalWord=1;
            }
        }
        //empty a string because it doesnt follow all the rules
        if(illegalWord==1)
        for(int d=0; d<5; d++)
            array[i][d] = '\0';
    }
}
//function calculates the points for a word
void calculatePoints(float priority[], int priorityLoc, int amountEachLetter[][26], float amountPoints, int amountLetterLoc)
{
    if(amountEachLetter[priorityLoc][amountLetterLoc]==0)
    {
        priority[priorityLoc] += amountPoints;
        amountEachLetter[priorityLoc][amountLetterLoc]++;
    }
    else
    {
        for(int i=0; i<amountEachLetter[priorityLoc][amountLetterLoc]; i++)
            amountPoints=amountPoints/DIVIDE_AMOUNT;
        
        priority[priorityLoc] += amountPoints;
        amountEachLetter[priorityLoc][amountLetterLoc]++;
    }
}
//function calculates the priority(amount of points it has) of all the words
void priorityCal(float priority[], char array[][ARRAY_SIZE], char perferedArray[][ARRAY_SIZE])
{
    int amtEachLetter[ARRAY_LENGTH][26] = {0};
    float eachLetterValue[26] = {43.31, 10.56, 23.13, 17.25, 56.88, 9.24, 12.59, 15.31, 38.45, 1.01, 5.61, 27.98, 15.36, 33.92, 36.51, 16.14, 1.00, 38.64, 29.23, 35.43, 18.51, 5.13, 6.57, 1.48, 9.06, 1.39};
    for(int i=0; i<ARRAY_LENGTH; i++)
    {
        for(int t=0; t<5; t++)
        {  
            int whichLetter=30;
            for(int d=0; d<26; d++)
            {
                if(array[i][t]==allLetters[d])
                    whichLetter=d;
            }
            calculatePoints(priority, i, amtEachLetter, eachLetterValue[whichLetter], whichLetter);
            for(int f=0; f<PERFER_ARRAY_LEN; f++)
            {
                if(strcmp(array[i], perferedArray[f])==0)
                    priority[i]*=PERFER_WORDS_AMOUNT;
            }
        }
    }         
}
//function calculates the points of the perfered words
void priorityCalPerfer(float priority[], char array[][ARRAY_SIZE], char perferedArray[][ARRAY_SIZE], int round)
{
    for(int i=0; i<ARRAY_LENGTH; i++)
    {
        for(int f=0; f<PERFER_ARRAY_LEN; f++)
        {
            if(strcmp(array[i], perferedArray[f])==0)
                priority[i]*=AMOUNT_EXTRA_PER_ROUND;
        }
    }
}
//function does a turn (getting inputs, and calculating results)
int doATurn(char yellowLetters[], char yellowLettersAll[][ARRAY_SIZE], char blacklist[], char greenLetters[], char array[][ARRAY_SIZE], float priority[], int amountWords, char perferedArray[][ARRAY_SIZE], int round)
{
    int temp=0, highestNumLoc=ARRAY_LENGTH, secHighestNumLoc=ARRAY_LENGTH, thrdHighestNumLoc=ARRAY_LENGTH;
    amountWords=0;
    char input[INPUT_SIZE] = {0};

    getWordInput(input, array);
    getResultAndAsign(input, blacklist, greenLetters, yellowLetters);
    calculateAllowedWords(array, blacklist, greenLetters, yellowLettersAll);
    priorityCalPerfer(priority, array, perferedArray, round);
    
    if(DEBUG_MODE==1)
        printf("\nblacklist is:%s\ngreen letters is:%s\nYellow letters is:%s\n\nWords it could be:\n", blacklist, greenLetters, yellowLetters);
    
    for(int i=0; i<ARRAY_LENGTH; i++)
    {
        if(array[i][0]!='\0')
        {
            if(DEBUG_MODE==1) {printf("%s %.3f\n", array[i], priority[i]);}
            amountWords++;

            if(priority[i]>priority[highestNumLoc])
            {
                thrdHighestNumLoc=secHighestNumLoc;
                secHighestNumLoc=highestNumLoc;
                highestNumLoc=i;
            }
        }
    }
    printf("\nAmount of Word Possibilites is: %d\n", amountWords);
    if(amountWords>0)
    {
        printf("Top picks(bigger number is better): \n-%s %.3f\n", array[highestNumLoc], priority[highestNumLoc]);
        if(array[secHighestNumLoc][0]!='\0')
        printf("-%s %.3f\n", array[secHighestNumLoc], priority[secHighestNumLoc]);
        if(array[thrdHighestNumLoc][0]!='\0')
        printf("-%s %.3f\n", array[thrdHighestNumLoc], priority[thrdHighestNumLoc]);
    }
    return amountWords;
}

int main()  
{
    float showingPriority[ARRAY_LENGTH+1] = {0};
    int amountOfWords=ARRAY_LENGTH, timesRan=0;
    char blacklist[ENGLISH_VOCAB_STRING] = {0}, array[ARRAY_LENGTH+1][ARRAY_SIZE] = {0}, greenLetters[ARRAY_SIZE] = {0}, yellowLettersAll[5][ARRAY_SIZE] = {0};
    
    do
    {
        printf("\x1b[H\x1b[Jcalculating...\n\n");
        resetVariables(showingPriority, blacklist, greenLetters, yellowLettersAll, array);
        priorityCal(showingPriority, array, perferedArray);
        timesRan=0, amountOfWords=ARRAY_LENGTH;

        printf("Welcome to Wordle Solver Made By AMZG studios!\nUse the Word 'arise' or 'retia' to begin! \n(note only type lowercase)\n");
        while(amountOfWords!=0 && amountOfWords!=1 && timesRan<6)
        {
            amountOfWords=doATurn(yellowLettersAll[timesRan], yellowLettersAll, blacklist, greenLetters, array, showingPriority, amountOfWords, perferedArray, timesRan);
            timesRan++;
        }
        printf("To play again, press 'y':");
        
    } while(getch()=='y');
    return 0;
}