#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define n_tel 8
#define n_cash 4
#define n_seat_per_row 10
#define n_zone_A 5 //rows
#define n_zone_B 10 //rows
#define n_zone_C 10 //rows
#define n_seatlow 1
#define n_seathigh 5
#define t_seatlow 5
#define t_seathigh 10
#define t_cashlow 2
#define t_cashhigh 4
#define p_cardsucces 0.9
#define p_zone_A 0.2
#define p_zone_B 0.4
#define p_zone_C 0.4
#define c_zone_A 30
#define c_zone_B 25
#define c_zone_C 20


int account_summary = 0;
int transactions = 0; //arithmos sinallagwn
int n_cust;
unsigned int seed;
int **seats_zone_A;
int empty_seats_A[n_zone_A]; //number of empty seats in zone A per row
int **seats_zone_B;
int empty_seats_B[n_zone_B]; //number of empty seats in zone B per row
int **seats_zone_C;
int empty_seats_C[n_zone_C]; //number of empty seats in zone C per row
int tel_counter,cas_counter;
double tel_wait_total = 0;
double time_to_complete_reservations = 0;
pthread_mutex_t tel_mutex,account_mutex,transaction_mutex,screen_lock_mutex,seat_mutex,tickets_mutex,tel_wait_mutex,time_to_complete_mutex,cashier_mutex;
pthread_cond_t tel_cond,cust_cond,cashier_cond;
