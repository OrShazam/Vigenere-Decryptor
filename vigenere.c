#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX_INT 0x7fffffff
#define MAX_KEY_LENGTH 20
#define LETTER_TO_VALUE(c) (c - 'A')
#define VALUE_TO_LETTER(v) ('A' + v)

struct CharList {
	char ch;
	struct CharList *next;
};
char* tostring(CharList* list){
	
}
double english_freq(char ch){
	if (ch == 'A') return 0.0815;
	if (ch == 'B') return 0.0144;
	if (ch == 'C') return 0.0276;
}
void free_list_memory(struct CharList *list){
	// there's probably a better way to do it
	if (list == NULL) return;
	struct CharList* prev = list;
	struct CharList* next = list->next;
	while (next != NULL){
		free(prev);
		next = next->next;
		prev = next;	
	}
	
}

struct CharList* initList(void) {
	struct CharList* root = malloc(sizeof(struct CharList));
	root->next = NULL;
	return root;
}
struct CharList* appendChar(char ch, struct CharList *list){
	struct CharList *next = initList();
	next->ch = ch;
	list->next = next;
	return list->next;
}

void print_fancy(char* message){
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 14);
	printf("[");
	SetConsoleTextAttribute(console,10);
	printf("+");
	SetConsoleTextAttribute(console, 14);
    printf("] %s\n", message);
	
}
int strsearch_till_idx(char* str, int idx, int len){
	for (int i = 0; i < idx; i++) {
		for (int j = 0; j < len; j++){
			if (str[i + j] != str[idx + i + j])
				break;
			if (j == len - 1){
				return i;
			}
		}
	}
	return -1;
}
int guess_key_length(char* encrypted){
	//Babbage's idea, 3 is a good enough check for repetitions 
	int probableLengthsCount[MAX_KEY_LENGTH + 1];
	for (int i = 0; i < MAX_KEY_LENGTH + 1; i++){
		probableLengthsCount[i] = 0;
	}
	int maxCount = 0, maxCountIndex, tmp_idx, spacing;
	
	for (int i = 3; i < strlen(encrypted); i++){
		tmp_idx = strsearch_till_idx(encrypted, i, 3);
		if (tmp_idx != -1) {
			spacing = i - tmp_idx;
			for (int j = 2; j < 21; j++){
				if (spacing % j == 0){
					probableLengthsCount[j]++;
				}
			}
		}
	}
	for (int i = 2; i < MAX_KEY_LENGTH + 1; i++){
		if (probableLengthsCount[i] > maxCount){
			maxCount = probableLengthsCount[i];
			maxCountIndex = i;
		}
	}
	return maxCountIndex;
}

void get_freq(struct CharList* list, int* temp_freq) {
	int freq[26];
	for (int i = 0; i < 26; i++){ freq[i] = 0;}
	while (list != NULL){
		freq[LETTER_TO_VALUE(list->ch)]++;
		list = list->next;
	}
}
char chi_test(char* letters) {
	char chi_letter, shift_letter;
	int smallest_chi = INT_MAX, shift_val,temp_val;
	double chi_val = 0, curr_freq_val, eng_freq, temp_chi;
	struct CharList* temp_list;
	struct CharList* temp_root;
	int temp_freq[26];
	for (int i = 0; i < 26; i++){
		temp_list = initList();
		temp_root = temp_list;
		shift_val = i;
		shift_letter = VALUE_TO_LETTER(i);
		for (int j = 0; j < strlen(letters); j++){			
			temp_val = LETTER_TO_VALUE(letters[j]) + 26;
			temp_val -= shift_val;
			temp_val = temp_val % 26;
			temp_list = appendChar(VALUE_TO_LETTER(temp_val), temp_list);		
		}
		get_freq(temp_list, temp_freq);
		for (int k = 0; k < 26; k++){
			curr_freq_val = (double)temp_freq[k] / strlen(letters);
			eng_freq = english_freq(LETTER_TO_VALUE(k));
			temp_chi = (curr_freq_val - eng_freq) * \
				(curr_freq_val - eng_freq) / eng_freq;
			chi_val += temp_chi;	
		}
		if (chi_val < smallest_chi){
			smallest_chi = chi_val;
			chi_letter = shift_letter;
		}	
		free_list_memory(temp_root);
	}
	return chi_letter;
}
char* decrypt_vigenere(char* encrypted, int key_length){
	// based on the solution for this kata:
	// https://www.codewars.com/kata/544e5d75908f2d5eb700052b/ 
	// by 'dexamusx'
	struct CharList* temp_list;
	struct CharList* temp_root;
	char* key = (char*)malloc(key_length);
	int startAddr = &key;
	for (int i = 0; i < key_length; i++){
		temp_list = initList();
		temp_root = temp_list;
		for (int j = 0; j < strlen(encrypted); j++){
			if (j % key_length == 0){
				appendChar(encrypted[j], temp_list);
			}
			*key = chi_test(tostring(temp_list));
			key++;
			
		}
		free_list_memory(temp_root);
	}
	return (char*)startAddr;
}

int main(int argc, char** argv) {
	if (argc < 2){
		printf("Usage: %s <encrypted>", argv[0]);
		return 0;
	}
	int key_length = guess_key_length(argv[1]);
	char message[50];
	sprintf(message, "Guessed key length of %d...", key_length);
	print_fancy(message);
	memset(message,'\0',50);
	char* key = decrypt_vigenere(argv[1], key_length);
	sprintf(message, "Guessed key as %s", key);
	print_fancy(message);
	// TODO: 
	// using CharList is kind of idiotic
	// actually decrypt
	// hardcode the english_freq() 
	
}