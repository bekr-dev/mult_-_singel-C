#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
/* set this values for MAX_STUDENTS = n STUDENTS  &  MAX_T = (thread/2) */
/*
#define MAX_STUDENTS 8700000
#define MAX_T 2 // create (MAX_T*2) thread [like MAX_T=1 create 2 threads]
#define BY 3 // by 1 = by_average by 2 = by_name by 3 = search
#define NAME1 "Omar"
#define NAME2 "Mahdi"
*/
//structs declaration 
struct date {int day, month, year;};

struct opsys {float td, tp, exam;};

struct student {    int id;    char name[50];    struct date birth;    struct opsys opsys;};

struct moy {    int id;    float moy;};

struct ThreadArgs{    struct moy *arr;    int left;    int right;};
//gobal varibales
int MAX_STUDENTS,MAX_T,BY;
char NAME1[25],NAME2[25];
struct student *s = NULL,ns; struct moy *av = NULL;
int threads = 0, show_student = 0, nst=15;
char zero[]="\0";
static int global_n;
static int global_np;
static int global_k, global_j;
char *student_names[100] = {
    "Omar", "Mahdi", "Ahmad", "Youssef", "Fatima", "Zineb", "Khaled", "Sarah", "Mohamed", "Amine",
    "Layla", "Hassan", "Nour", "Ibrahim", "Salma", "Rami", "Huda", "Bilal", "Meriem", "Karim",
    "Sami", "Nadia", "Tariq", "Yasmin", "Adel", "Samia", "Mourad", "Imane", "Walid", "Hana",
    "Fares", "Rania", "Anis", "Lamia", "Hakim", "Souad", "Nabil", "Manal", "Sofiane", "Asma",
    "Reda", "Bouchra", "Kamel", "Houda", "Mustapha", "Nawel", "Rafik", "Dalila", "Hichem", "Fella",
    "Lotfi", "Soraya", "Jamal", "Wided", "Abderrahmane", "Leila", "Mounir", "Safia", "Yacine", "Karima",
    "Hamza", "Amel", "Samir", "Faiza", "Djamel", "Khadija", "Mehdi", "Zahra", "Fouad", "Latifa",
    "Bachir", "Siham", "Riad", "Loubna", "Taha", "Zakaria", "Radia", "Youcef", "Aicha",
    "Idris", "Farida", "Majid", "Habiba", "Nassim", "Malika", "Samy", "Dounia", "Faysal", "Sabrina",
    "Boualem", "Nezha", "Rabah", "Samira", "Aya", "Slimane", "Wafa", "Toufik", "Ghania", "Aziz", "Hanane"
};

void ini_memory(int n) {/* this for memory allocation*/
    s = malloc(n * sizeof(struct student));
    av = malloc(n * sizeof(struct moy));
    if (s == NULL || av == NULL) {printf("Memory allocation failed!\n");exit(1);}
}
void free_memory() {free(s);    free(av); }

void ini_students(int n) {
/* this is for random values */srand(time(NULL));
    for (int i = 0; i < MAX_STUDENTS; i++) {
        int d = rand() % 32, m = rand() % 13, y = rand() % 7;
        s[i].id = (rand() % 100000 + 220000000);
        strcpy(s[i].name, student_names[rand() % 100]);
        s[i].birth.day = d + 1;
        s[i].birth.month = m + 1;
        s[i].birth.year = y + 2000;
        s[i].opsys.td = 7 + rand() / (float)RAND_MAX * (20 - 7);
        s[i].opsys.tp = 7 + rand() / (float)RAND_MAX * (20 - 7);
        s[i].opsys.exam = 7 + rand() / (float)RAND_MAX * (20 - 7);
        av[i].id = s[i].id;
        av[i].moy = (s[i].opsys.tp + s[i].opsys.td) * 0.2 + s[i].opsys.exam * 0.6;
    }
}


void merge(struct moy arrm[], int left, int mid, int right) {
	
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    struct moy *Lm = malloc(n1 * sizeof(struct moy));
    struct moy *Rm = malloc(n2 * sizeof(struct moy));
   
    if (Lm == NULL || Rm == NULL) {
        printf("Memory allocation in merge failed!\n");
        exit(1);
    }

    for (i = 0; i < n1; i++)Lm[i] = arrm[left + i];
    for (j = 0; j < n2; j++)Rm[j] = arrm[mid + 1 + j];
    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (Lm[i].moy >= Rm[j].moy) {
            arrm[k] = Lm[i];
            i++;
        } else {
            arrm[k] = Rm[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arrm[k] = Lm[i];
        i++;
        k++;
    }

    while (j < n2) {
        arrm[k] = Rm[j];
        j++;
        k++;
    }

    free(Lm);
    free(Rm);
}

void merge_sort_by_average(struct moy arrm[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_by_average(arrm, left, mid);
        merge_sort_by_average(arrm, mid + 1, right);
        merge(arrm, left, mid, right);
    }
}
void merge_sort_parallel(struct moy arr[], int left, int right);
void* merge_sort_thread(void* arg) {
	__sync_fetch_and_add(&threads, 1); 
    struct ThreadArgs *args = (struct ThreadArgs*)arg;
    merge_sort_parallel(args->arr, args->left, args->right);
    return NULL;
}

void merge_sort_parallel(struct moy arr[], int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    // if threads == MAX_T*2 stop creating threads
    if (MAX_T-1  < threads ) {
        merge_sort_by_average(arr, left, right);
        return;
    }

    struct ThreadArgs left_args = {arr, left, mid};
    struct ThreadArgs right_args = {arr, mid + 1, right};

    pthread_t left_thread, right_thread;
    if (pthread_create(&left_thread, NULL, merge_sort_thread, &left_args) != 0) {
        exit(1);
    }
    if (pthread_create(&right_thread, NULL, merge_sort_thread, &right_args) != 0) {
        exit(1);
    }

    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);
    merge(arr, left, mid, right);
}



void byname(int n){
    int i, j,k, np = 1;
    while (np < n) np *= 2;
    for ( k = 2; k <= np; k *= 2) {
        for ( j = k/2; j > 0; j /= 2) {
            for (i = 0; i < np; i++) {
                int l = i ^ j;
                if (l > i && i < n && l < n) {
                    int dir = (i & k) == 0 ? 1 : -1;
                    int cmp = strcmp(s[i].name, s[l].name);
                    if ((dir == 1 && cmp > 0) || (dir == -1 && cmp < 0)) {
                        ns = s[i];
                        s[i] = s[l];
                        s[l] = ns;
                    }
                }
            }
        }
    }
    
}


void* bitonic_worker(void* arg) {

    uintptr_t tid = (uintptr_t)arg;
    int chunk_size = (global_np + 1) / 2; 
    int start = tid * chunk_size;
    int end = (tid == 1) ? (global_np + 1) / 2 : (tid + 1) * chunk_size;

    for (int idx = start; idx < end; idx++) {
        int i = idx;
        int l = i ^ global_j;

        if (i < l && i < global_n && l < global_n) {
            int dir = (i & global_k) == 0 ? 1 : -1;
            int cmp = strcmp(s[i].name, s[l].name);
            if ((dir == 1 && cmp > 0) || (dir == -1 && cmp < 0)) {
                struct student tmp = s[i];
                s[i] = s[l];
                s[l] = tmp;
            }
        }
    }
    return NULL;
}

void byname_parallel_fast(int n) {
    if (n <= 1) return;

    int np = 1;
    while (np < n) np *= 2;

    global_n = n;
    global_np = np;

    const int NUM_THREADS = 2;
    pthread_t threads[NUM_THREADS];

    for (int k = 2; k <= np; k *= 2) {
        for (int j = k / 2; j > 0; j /= 2) {
            global_k = k;
            global_j = j;

            for (long t = 0; t < NUM_THREADS; t++) {
                if (pthread_create(&threads[t], NULL, bitonic_worker, (void*)(uintptr_t)t) != 0) {
                    exit(1);
                }
            }
            for (int t = 0; t < NUM_THREADS; t++) {
                pthread_join(threads[t], NULL);
            }
        }
    }
}




static struct SearchResult global_result;
static pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
static char target_name[50];

struct SearchThreadArgs {
    int start;
    int end;
};
struct SearchResult {
    int *indices;
    int count;
    int capacity;
};

void* search_thread_func(void* arg) {
	__sync_fetch_and_add(&threads, 1);
    struct SearchThreadArgs *args = (struct SearchThreadArgs*)arg;
    int start = args->start;
    int end = args->end;

    for (int i = start; i < end; i++) {
        if (strcmp(s[i].name, target_name) == 0) {
            pthread_mutex_lock(&result_mutex);
            
            if (global_result.count >= global_result.capacity) {
                global_result.capacity = (global_result.capacity == 0) ? 16 : global_result.capacity * 2;
                global_result.indices = realloc(global_result.indices, global_result.capacity * sizeof(int));
                if (!global_result.indices) {
                    fprintf(stderr, "Memory allocation failed!\n");
                    exit(1);
                }
            }
            
            global_result.indices[global_result.count] = i;
            global_result.count++;
            
            pthread_mutex_unlock(&result_mutex);
        }
    }
    return NULL;
}

struct SearchResult* search_all_by_name_multithread(const char* name, int num_threads) {

    strncpy(target_name, name, sizeof(target_name) - 1);
    target_name[sizeof(target_name) - 1] = '\0';

    global_result.indices = NULL;
    global_result.count = 0;
    global_result.capacity = 0;

    if (num_threads <= 0) num_threads = 1;
    if (num_threads > MAX_STUDENTS) num_threads = MAX_STUDENTS;

    int chunk_size = (MAX_STUDENTS + num_threads - 1) / num_threads;

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    struct SearchThreadArgs *args = malloc(num_threads * sizeof(struct SearchThreadArgs));

    int thread_count = 0;
    for (int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? MAX_STUDENTS : (t + 1) * chunk_size;
        if (start >= MAX_STUDENTS) break;

        args[thread_count].start = start;
        args[thread_count].end = end;

        if (pthread_create(&threads[thread_count], NULL, search_thread_func, &args[thread_count]) != 0) {
            free(threads);
            free(args);
            return NULL;
        }
        thread_count++;
    }

    for (int t = 0; t < thread_count; t++) {
        pthread_join(threads[t], NULL);
    }

    free(threads);
    free(args);

    if (global_result.count > 0) {
        global_result.indices = realloc(global_result.indices, global_result.count * sizeof(int));
    }

    struct SearchResult *result = malloc(sizeof(struct SearchResult));
    if (!result) return NULL;
    *result = global_result;

    return result;
}

void free_search_result(struct SearchResult *res) {
    if (res) {
        free(res->indices);
        free(res);
    }
}





void show_student_by_name(int n){
	if(show_student){
    printf("First %d students :\n",n);
	printf("   ID     |   Name\n");
    for (int i = 0; i < n ; i++)
        printf("%d :    %s\n", s[i].id,s[i].name);
}
}
void show_student_by_average(int n){
	if(show_student){
    printf("First %d students :\n",n);
    printf("   ID     |   Average\n");
    for (int i = 0; i < n ; i++){
	strcpy(zero,"\0"); // display '' (null character) with av>9 
	if(av[i].moy < 10)strcpy(zero,"0");
        printf("%d :    %s%.2f\n", av[i].id,zero,av[i].moy);}
}
}
      
int main() {
    printf("number of students :");scanf("%d",&MAX_STUDENTS);
    printf("number of threads :");scanf("%d",&MAX_T);
    printf("sort by average : 1\n");			
    printf("sort by name : 2\n");		
    printf("search students : 3\n");
	while(BY>3||BY<1){printf("tap just (1,2,3) : ");scanf("%d",&BY);}
	if(BY == 3){
    printf("name for normal search :\n");scanf("%s",&NAME1);
    printf("name for normal search :\n");scanf("%s",&NAME2);
	}

    printf("display students (0/1):");scanf("%d",&show_student);
    if(show_student){
    printf("number of students for display :");scanf("%d",&nst);
}
    printf("Initializing memory for %d students\n", MAX_STUDENTS);
    ini_memory(MAX_STUDENTS);
    clock_t start, end;
/* Initializing student data */  ini_students(MAX_STUDENTS);

switch(BY){
	case 1:printf("*******  by_average  *******\n");
	show_student_by_average(nst);
  	start = clock();
	merge_sort_by_average(av, 0, MAX_STUDENTS - 1);
  	end = clock();
  	printf("--------------------Normal Merge: %.3fs\n", (double)(end - start) / CLOCKS_PER_SEC);
	show_student_by_average(nst);
  	free_memory();ini_memory(MAX_STUDENTS);ini_students(MAX_STUDENTS);
	show_student_by_average(nst);
	start = clock();
    merge_sort_parallel(av, 0, MAX_STUDENTS - 1);
	end = clock();
    printf("--------------------multi threads Merge: %.3fs  ,total threads created : %d\n", (double)(end - start) / CLOCKS_PER_SEC,threads);
	show_student_by_average(nst);
    break;
    case 2:printf("*******  by_name  *******\n");
    show_student_by_name(nst);
 	start = clock();
    byname(MAX_STUDENTS);
	end = clock();
    printf("bitonic name : %.3fs\n", (double)(end - start) / CLOCKS_PER_SEC);
    show_student_by_name(nst);
  	free_memory();ini_memory(MAX_STUDENTS);ini_students(MAX_STUDENTS);
    show_student_by_name(nst);
 	start = clock();
	byname_parallel_fast(MAX_STUDENTS);
	end = clock();
    printf("fast bitonic name : %.3fs\n", (double)(end - start) / CLOCKS_PER_SEC);
    show_student_by_name(nst);
    break;
    case 3:{printf("*******  by_name  *******\n");
	struct SearchResult* res;
	start = clock();
    res = search_all_by_name_multithread(NAME1, 0);
	end = clock();
    printf("--------------------normal Search: %.3fs\n", (double)(end - start) / CLOCKS_PER_SEC);
	printf("First %d students :\n",nst);
    printf("   ID     |   Name\n");
    for (int i = 0; i < res->count && i<nst ; i++)
    printf("%d :    %s\n", s[res->indices[i]].id,s[res->indices[i]].name);
    threads = 0;
	start = clock();
    res = search_all_by_name_multithread(NAME2, MAX_T);
	end = clock();
    printf("--------------------multi threads Search: %.3fs  ,total threads created : %d\n", (double)(end - start) / CLOCKS_PER_SEC,threads);
	printf("First %d students :\n",nst);
    printf("   ID     |   Name\n");
    for (int i = 0; i < res->count && i<nst ; i++)
    printf("%d :    %s\n", s[res->indices[i]].id,s[res->indices[i]].name);
    
    break;}
}
    getchar();	
    getchar();	
    
    free_memory();

    return 0;
}
