#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

const int N_cook = 2;
const int N_oven = 15;
const int N_packer = 2;
const int N_deliverer = 10;

const int T_order_low = 1;
const int T_order_high = 3;

const int N_order_low = 1;
const int N_order_high = 5;

const int P_plain = 60;

const int T_payment_low = 1;
const int T_payment_high = 3;

const int P_fail = 10;

const int C_plain = 10;
const int C_special = 12;

const int T_low = 5;
const int T_high = 15;

const int T_prep = 1;

const int T_bake = 10;

const int T_pack = 1;

int no_available_cooks;
int no_available_ovens;
int no_available_packers;
int no_available_deliverers;

double payment_time;
double order_time;
double total_order_time;
double max_order_time;
double cold_time;
double total_cold_time;
double max_cold_time;
unsigned int seed;

int total_plain;
int total_special;
int total_revenue;
int failed_orders;
int successful_orders;

pthread_mutex_t mutex_no_available_cooks = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_available_ovens = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_available_packers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_no_available_deliverers = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_total_order_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_order_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_screen = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_no_available_cooks = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_available_ovens = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_available_packers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_no_available_deliverers = PTHREAD_COND_INITIALIZER;
