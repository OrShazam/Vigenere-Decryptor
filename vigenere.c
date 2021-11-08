#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAX_INT 0x7fffffff
#define MAX_KEY_LENGTH 20
#define LETTER_TO_VALUE(c) (c - 'A')
#define VALUE_TO_LETTER(v) ('A' + v)

void print_fancy_bad(char* message){
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 14);
	printf("[");
	SetConsoleTextAttribute(console,12);
	printf("-");
	SetConsoleTextAttribute(console, 14);
    printf("] %s\n", message);	
}
void print_fancy_good(char* message){
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 14);
	printf("[");
	SetConsoleTextAttribute(console,10);
	printf("+");
	SetConsoleTextAttribute(console, 14);
    printf("] %s\n", message);	
}
void check_input(char* input){
	if (strlen(input) < 20){
		print_fancy_bad("Analysis is useless for short messages.");
		exit(0);
	}
	for (int i = 0; i < strlen(input); i++){
		if (!(input[i] <= 'Z' && input[i] >= 'A')){
			print_fancy_bad("Input not in the correct format.");
			print_fancy_bad("Should be only uppercase.");
			exit(1);
		}
	}
	
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

void get_freq(char* letters , int* temp_freq) {
	int freq[26];
	for (int i = 0; i < 26; i++){ freq[i] = 0;}
	for (int i = 0; i < strlen(letters); i++){
		temp_freq[LETTER_TO_VALUE(letters[i])]++;
	}
}
char chi_test(char* letters, double* english_freq_table) {
	char chi_letter, shift_letter;
	int smallest_chi = INT_MAX, shift_val,temp_val;
	double chi_val = 0, curr_freq_val, eng_freq, temp_chi;
	int alloc_size = 1000;
	char* temp_letters = (char*)malloc(alloc_size);
	int temp_letters_count;
	int temp_freq[26];
	for (int i = 0; i < 26; i++){
		shift_val = i;
		shift_letter = VALUE_TO_LETTER(i);
		temp_letters_count = 0;
		memset(temp_letters,'\0',alloc_size);
		for (int j = 0; j < strlen(letters); j++){			
			temp_val = LETTER_TO_VALUE(letters[j]) + 26;
			temp_val -= shift_val;
			temp_val = temp_val % 26;
			if (temp_letters_count++ > alloc_size){
				alloc_size *= 2;
				realloc(temp_letters,alloc_size);
			}
			temp_letters[temp_letters_count] = VALUE_TO_LETTER(temp_val);	
		}
		get_freq(temp_letters, temp_freq);
		for (int k = 0; k < 26; k++){
			curr_freq_val = (double)temp_freq[k] / strlen(letters);
			eng_freq = english_freq_table[k];
			temp_chi = (curr_freq_val - eng_freq) * \
				(curr_freq_val - eng_freq) / eng_freq;
			chi_val += temp_chi;	
		}
		if (chi_val < smallest_chi){
			smallest_chi = chi_val;
			chi_letter = shift_letter;
		}	
	}
	free(temp_letters);
	return chi_letter;
}
char* get_vigenere_key(char* encrypted, int key_length){
	// based on the solution for this kata:
	// https://www.codewars.com/kata/544e5d75908f2d5eb700052b/ 
	// by 'dexamusx'
	double eng_freq_table[26] = { 0.0815,0.0144,0.0276,0.0379,0.1311,0.0292,0.0199,0.0526, \
		0.0635,0.0013,0.0042,0.0339,0.0254,0.0710,0.0800,0.0198,0.0012,0.0683,0.0610,0.1047, \
		0.0246,0.0092,0.0154,0.0017,0.0198,0.0008};
	int alloc_size = 1000;
	int count_letters;
	int key_count = 0;
	char* letters = (char*)malloc(alloc_size);
	char* key = (char*)malloc(key_length);
	for (int i = 0; i < key_length; i++){
		memset(letters,'\0',alloc_size);
		count_letters = 0;
		for (int j = 0; j < strlen(encrypted); j++){
			if (j % key_length == 0){
				if (count_letters++ > alloc_size){
					alloc_size *= 2;
					realloc(letters,alloc_size);
				}
				letters[count_letters] = encrypted[j];
			}
		}
		key[key_count++] = chi_test((char*)letters, eng_freq_table);
	}
	free(letters);
	return key;
}
char* vigenere_decrypt(char* encrypted, char* key){
	int temp_val;
	char* decrypted = (char*)malloc(strlen(encrypted));
	int key_len = strlen(key);
	for (int i = 0; i < strlen(decrypted); i++){
		temp_val = encrypted[i] - key[i % key_len];
		temp_val += 26;
		temp_val = temp_val % 26;
		*(decrypted + i) = VALUE_TO_LETTER(temp_val);
	}
	return decrypted;
}
int main(int argc, char** argv) {
	if (argc < 2){
		printf("Usage: %s <encrypted>", argv[0]);
		return 0;
	}
	check_input(argv[1]);
	int key_length = guess_key_length(argv[1]);
	char message[50];
	sprintf(message, "Guessed key length of %d...", key_length);
	print_fancy_good(message);
	memset(message,'\0',50);
	char* key = get_vigenere_key(argv[1], key_length);
	sprintf(message, "Guessed key as %s...", key);
	print_fancy_good(message);
	char* decrypted = vigenere_decrypt(argv[1],key);
	print_fancy_good("Decrypted Message.");
	printf(decrypted);
	return 0;
}
