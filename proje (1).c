#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_THREADS 6
#define MAX_LINES 20
#define MAX_LINE_LENGTH 4 // two character in every line and termination character "\0"
#define MALZ 38

sem_t mutex;
int satir = 0;
int finish = 0; // 38 malzeme var. 'finish' 38'e ulaştığında döngü duracak, stok bitmiş olacak.
char malzemeler[MAX_LINES][MAX_LINE_LENGTH];  // Stoktaki malzemeleri tutan dizi



// Anahtar-değer çiftini temsil eden yapı
typedef struct {
    char* key;
    char* value;
} KeyValuePair;

// Anahtar-değer çiftini depolamak için kullanılan veri yapısı
typedef struct {
    KeyValuePair* pairs;
    size_t size;
} Dictionary;


// Global Dictionary ler 
Dictionary* dict1;
Dictionary* dict2;


// Yeni bir dictionary oluştur
Dictionary* createDictionary() {
    Dictionary* dict = (Dictionary*)malloc(sizeof(Dictionary));
    dict->size = 0;
    dict->pairs = NULL;
    return dict;
}

// Anahtar ve değeri dictionary'ye ekle
void addToDictionary(Dictionary* dict, const char* key, const char* value) {
    dict->size++;
    dict->pairs = (KeyValuePair*)realloc(dict->pairs, dict->size * sizeof(KeyValuePair));
    dict->pairs[dict->size - 1].key = strdup(key); // Anahtar için bellek tahsisi
    dict->pairs[dict->size - 1].value = strdup(value); // Değer için bellek tahsisi
}

// Anahtarın değerini dictionary'den al
const char* getFromDictionary(Dictionary* dict, const char* key) {
    for (size_t i = 0; i < dict->size; i++) {
        if (strcmp(dict->pairs[i].key, key) == 0) {
            return dict->pairs[i].value;
        }
    }
    return NULL; // Eğer anahtar bulunamazsa NULL döndür
}

// Dictionary bellekten temizle
void freeDictionary(Dictionary* dict) {
    for (size_t i = 0; i < dict->size; i++) {
        free(dict->pairs[i].key); // Anahtar için tahsis edilen belleği serbest bırak
        free(dict->pairs[i].value); // Değer için tahsis edilen belleği serbest bırak
    }
    free(dict->pairs); // Dictionary'ye tahsis edilen belleği serbest bırak
    free(dict); // Dictionary yapısının kendisini serbest bırak
}


// İlk dictionary'yi kullanarak veri ekleme
void useFirstDictionary() {
    	addToDictionary(dict1, "Y", "yumurta");
    	addToDictionary(dict1, "S", "süt");
    	addToDictionary(dict1, "K", "kakao");
    	addToDictionary(dict1, "U", "un");
}

// İkinci dictionary'yi kullanarak veri ekleme
void useSecondDictionary() {
    	addToDictionary(dict2, "usta1", "SK");
    	addToDictionary(dict2, "usta2", "UY");
    	addToDictionary(dict2, "usta3", "KU");
    	addToDictionary(dict2, "usta4", "YK");
    	addToDictionary(dict2, "usta5", "SY");
    	addToDictionary(dict2, "usta6", "SU");
}




int arraySearch(const char* materials, const char* usta_isim) {

	int found1 = 0, found2 = 0;
	
	/*
    	char char1 = materials[0];
    	char char2 = materials[1];
    	*/
    	
    	
    	char char1[2] = { materials[0], '\0' };
	char char2[2] = { materials[1], '\0' };
    
    	
	// Gerekli malzemeleri bulmak için ilgili dizide arama yapılması
    	for (int i = 0; i < MAX_LINES; i++) {
        	for (int j = 0; j < MAX_LINE_LENGTH; j++) {
		    
		    if (malzemeler[i][j] == materials[0] && !found1) {
		    	finish++;
		        printf("%s, %s 'yı teslim aldı.\n", usta_isim, getFromDictionary(dict1, char1));
		        malzemeler[i][j] = 'd'; // 'd', 'done' anlamındadır.  
		        found1 = 1; // Bulunursa flag 1 yapılır.
		    }
		    if (malzemeler[i][j] == materials[1] && !found2) {
		    	finish++;
		        printf("%s, %s'yı teslim aldı.\n", usta_isim, getFromDictionary(dict1, char2));
		        malzemeler[i][j] = 'd'; // "d" means "done".
		        found2 = 1;
		    }

		    if (found1 && found2) {
		        //printf("Diğer işleme geçildi\n");
		        return 1; 
            }
          }
       }





	
	/*
	for (int i = 0; i < MAX_LINES; i++) {
    		for(int j=0; j < MAX_LINE_LENGTH; j++) {
    		
    		// işlemler... 
    		
        	//  printf("%c", malzemeler[i][j]);        	
        }
    }
    */


}


void *cook(void *arg) {

	int num = *((int*)arg); // Usta id si
	char usta_isim[10];
	sprintf(usta_isim, "usta%d", num); 
	
	
	sem_wait(&mutex);
	// Critical section
	
	// İLgili ustanın ihtiyacı olan malzemeler
	const char* materials = getFromDictionary(dict2, usta_isim); // !!!
	
	if(num==1) {
		// usta1
		while(finish<=MALZ) {
		 
		printf("%s, %s ve %s 'yu bekliyor.\n", usta_isim, getFromDictionary(dict1, "S"), getFromDictionary(dict1, "K"));
		int h = arraySearch(materials, usta_isim);  // Eğer gerekli malzeme bulunursa
		//sem_post(&mutex);
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3); // Preparation time
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
		
	}
	
	else if(num==2) {
		// usta2
		
		while(finish<=MALZ) {
		
		printf("%s, %s ve %s 'yı bekliyor.\n", usta_isim, getFromDictionary(dict1, "U"), getFromDictionary(dict1, "Y"));
		int h = arraySearch(materials, usta_isim);
		//sem_post(&mutex);
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3);
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
	}
	

	else if(num==3) {
		// usta3
		
		while(finish<=MALZ) {
		
		printf("%s, %s ve %s 'u bekliyor.\n", usta_isim, getFromDictionary(dict1, "K"), getFromDictionary(dict1, "U"));
		int h = arraySearch(materials, usta_isim);
		//sem_post(&mutex);
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3);
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
	}
	
	else if(num==4) {
		// usta4
		
		while(finish<=MALZ) {
		
		printf("%s, %s ve %s 'yu bekliyor.\n", usta_isim, getFromDictionary(dict1, "Y"), getFromDictionary(dict1, "K"));
		int h = arraySearch(materials, usta_isim);
		//sem_post(&mutex);
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3);
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
	}
	
	else if(num==5) {
		// usta5
		
		while(finish<=MALZ) {
		
		printf("%s, %s ve %s 'yı bekliyor.\n", usta_isim, getFromDictionary(dict1, "S"), getFromDictionary(dict1, "Y"));
		int h = arraySearch(materials, usta_isim);
		//sem_post(&mutex);
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3);
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
	}
	
	else if(num==6) {
		// usta6
		 
		while(finish<=MALZ) {
		
		printf("%s, %s ve %s 'yu bekliyor.\n", usta_isim, getFromDictionary(dict1, "S"), getFromDictionary(dict1, "U"));
		int h = arraySearch(materials, usta_isim);
		
		if(h==1) { // two ingredients have been found.
			printf("%s, gerekli malzemeleri aldı.\n", usta_isim);
			printf("%s, pastayı hazırlıyor.\n", usta_isim);
			sleep(3);
			printf("%s, pastayı teslim etti.\n", usta_isim);
			printf("Toptancı, pastayı aldı ve satmaya gitti.\n");
			}
		}
		printf("%s dükkanı kapattı.\n", usta_isim);
		free(arg);
		pthread_exit(NULL);
	}
	
	sem_post(&mutex);
	
	
	
}


int main() {
	
	pthread_t threads[NUM_THREADS];
	sem_init(&mutex, 0, 1);
	
	
	
	// Text dosyasının okunması
	FILE *fp = fopen("/home/ubuntu/Desktop/Sistem_Programlama/donemOdev/malzeme.txt", "r");
	if(fp==NULL) {
		printf("Dosya açma hatası.\n");
		return 1;
	}
	
	while(fgets(malzemeler[satir], MAX_LINE_LENGTH, fp) != NULL) {
		satir++;
		if(satir>=MAX_LINES) {
			break;
		}
	}
	fclose(fp);
	
	
	
	// Dictionarylerin oluşturulması
	 dict1 = createDictionary();
    	 dict2 = createDictionary();
    	 
    	 useFirstDictionary();
    	 useSecondDictionary();
       
	/*
	Örnek:
    	char* value = getFromDictionary(dict, "key1");
    	if (value != NULL) {
    	    printf("Value of key1: %s\n", value);
   	 } 
   	 else {
    	    printf("Key not found\n");
    	}
    	*/

    	
	
	// threadlerin oluşturulması
	for(int i=0;i<NUM_THREADS;i++) {
		int* thread_arg = malloc(sizeof(int)); // Bellekten bir alan ayır
    		*thread_arg = i + 1; // İstenen değeri ayarla	
		pthread_create(&threads[i], NULL, cook, (void*)thread_arg);	
	}
 

	// threadlerin joinlenmesi
	for(int i=0;i<NUM_THREADS;i++) {
		pthread_join(threads[i], NULL);
	}
	
	printf("Gün bitti.\n");
	
	sem_destroy(&mutex);
	
	freeDictionary(dict1);
    	freeDictionary(dict2);

return 0;
}
