#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;
using std::string;

std::vector<string> loadWords(string file){
    std::ifstream wordFile(file);
    std::vector<string> wordList;

    if(!wordFile.is_open()){
        throw std::invalid_argument("File cannot be opened");
    }
    string word;
    while(wordFile >> word){
        wordList.push_back(word);
    }

    return wordList;
}

int getIndex(char c){
    return c - 'a';
}

//according to frequency a great first word is arose
void freqCheck(std::vector<string>& wordList, int freqArr[]){
    for(string &word : wordList){
        for(unsigned long int i = 0; i<word.length(); i++){
            freqArr[getIndex(word[i])]++;
        }
    }
}

/*
given a string and its green, yellow, and grey properties:
    green characters: remove all words from the vector whose char at index i doesnt match the green index
    yellow: remove all words who do not contain yellow character
    grey: remove all words that contain grey character if they dont have a green for that character
*/

bool contains(string value, char letter){
    for(unsigned long int i = 0; i<value.length(); i++){
        if(value[i] == letter){return true;}
    }
    return false;
}

void removeWords(std::vector<string>& wordList, string guessWord, string properties){
    std::vector<string> newList;
    int greenLetters[26] = {0};
    for(string &word: wordList){
        for(unsigned long int i = 0; i<properties.length(); i++){
            if(properties[i] == 'g'){
                greenLetters[getIndex(guessWord[i])]++;
            }
        }
        for(unsigned long int i = 0; i<properties.length(); i++){
            if(properties[i] == 'g' && word[i]!=guessWord[i]){break;}
            else if(properties[i] == 'y' && word[i] == guessWord[i]){break;}
            else if(properties[i] == 'y' && !contains(word,guessWord[i])){break;}
            else if(properties[i] == 'd' && contains(word,guessWord[i])){
                if(!greenLetters[getIndex(guessWord[i])]){break;}
            }
            else if(i == 4){newList.push_back(word);}
        }
    }
    wordList = newList;
}

/*
To find the next guess we can repopulate the frequency array on the reduced list of words and find a word 
that has the highest sum of frequency
*/

string nextGuess(std::vector<string>& wordList){
    int freqArr[26] = {0};
    freqCheck(wordList,freqArr);
    int sum = 0;
    int maxSum = 0;
    string maxString;
    for(string &word: wordList){
        int usedLetters[26] = {0};
        for(unsigned long i = 0; i<word.length(); i++){
            usedLetters[getIndex(word[i])]++;
            if(usedLetters[getIndex(word[i])] == 1){
                sum +=freqArr[getIndex(word[i])];
            }
        }
        if(sum > maxSum){
            maxSum = sum;
            maxString = word;
        }
        sum = 0;
    }
    return maxString;
    
}
/* for each word in the file we need to start with arose and continue until the word is found from guess word
    if it takes more than 5 guesses then the word is not found have variable not found for this to count those words
    other than that we can add the number of guesses it took to the total then average at the end of the loop
*/
string consProperties(const string& word, const string& guess){
    string properties = "";
    for(unsigned long i = 0; i<word.length(); i++){
        if(word[i] == guess[i]){properties+='g';}
        else if(contains(word,guess[i])){properties+='y';}
        else{properties+='d';}
    }
    return properties;
}

double testWords(string file){
    int count = 0;
    int numLoss = 0;
    double totalCount = 0;
    string guess;
    std::vector<string> guessWords = loadWords(file);
    for(string &word: guessWords){
        std::vector<string> wordList = loadWords("wordle-answers-alphabetical.txt");
        guess = "arose";
        removeWords(wordList,guess,consProperties(word,guess));
        count = 1;
        while(guess!=word){
            guess = nextGuess(wordList);
            removeWords(wordList,guess,consProperties(word,guess));
            count++;
        }
        if(count>5){numLoss++;}
        totalCount+=count;
    }
    double average = totalCount/guessWords.size();
    return average;

}


int main(){
    std::vector<string> wordList = loadWords("wordle-answers-alphabetical.txt");
    string guess;
    string properties;
    int count = 0;
    cout<<endl;
    cout<<"WORDLE SOLVER"<<endl;
    cout<<"-------------"<<endl;
    while(wordList.size()>1 && count<5){
        cout<<"Enter guess and properties: ";
        std::cin>>guess;
        if(guess == "exit"){break;}
        std::cin>>properties;
        removeWords(wordList,guess,properties);
        string next = nextGuess(wordList);
        cout<<"Next best guess is "<<next<<endl;
        count++;
        if(count == 5){
            cout<<"The word can be any of these: "<<endl;
            for(string &word: wordList){
                cout<<word<<" ";
            }
        }
    }
    return 0;
}