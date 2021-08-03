#include "p3170162-p3170165-res1.h"
#define TRUE 0

void * purchase_seat(void *thread_id){
    int return_code;
    int *tid = (int *)thread_id;
    int transaction_cost;
    int ** current_zone ;
    int current_rows = 0;
    int *current_empty_seats;
    int current_c_cost = 0;
    char zone;
    struct timespec start,tel_stop,no_seats_wait;

    return_code = clock_gettime(CLOCK_REALTIME,&start);
    if(return_code==-1){
      printf("ERROR: Failure getting real time.");
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_lock(&tel_mutex);
  	if (return_code != 0) {
  		printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
  		pthread_exit(&return_code);
  	}

    //check if we have available tel line
    if(tel_counter==n_tel){
      while (TRUE) {
    		return_code = pthread_cond_wait(&tel_cond, &tel_mutex);
    		if (return_code != 0) {
    			printf("ERROR: return code from pthread_cond_wait() is %d\n", return_code);
    			pthread_exit(&return_code);
    		}
    	}
    }

    return_code = clock_gettime(CLOCK_REALTIME,&tel_stop);
    if(return_code==-1){
      printf("ERROR: Failure getting real time.");
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_lock(&tel_wait_mutex);
  	if (return_code != 0) {
  		printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
  		pthread_exit(&return_code);
  	}

    tel_wait_total += (tel_stop.tv_nsec - start.tv_nsec);

    return_code = pthread_mutex_unlock(&tel_wait_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    //increase tel_counter
    tel_counter++;

    return_code = pthread_mutex_unlock(&tel_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    /*dimiourgia tyxaiwn arithmwn*/
    int n_tickets = (rand_r(&seed) % (n_seathigh-n_seatlow+1)) + n_seatlow ;
    int t_wait = (rand_r(&seed) % (t_seathigh-t_seatlow+1)) + t_seatlow ;

    //Gia thn zonh
    double rand_num_for_zone = (double)rand() / (double)RAND_MAX ;
    if(rand_num_for_zone<=p_zone_A){
      current_zone = seats_zone_A;
      current_rows = n_zone_A;
      current_empty_seats = empty_seats_A;
      current_c_cost = c_zone_A;
      zone = 'A';
    }
    else if(rand_num_for_zone<=p_zone_A+p_zone_B){
      current_zone = seats_zone_B;
      current_rows = n_zone_B;
      current_empty_seats = empty_seats_B;
      current_c_cost = c_zone_B;
      zone = 'B';
    }
    else{
      current_zone = seats_zone_C;
      current_rows = n_zone_C;
      current_empty_seats = empty_seats_C;
      current_c_cost = c_zone_C;
      zone = 'C';
    }


    //wait to check for available seats
    //sleep(t_wait);


    return_code = pthread_mutex_lock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    //check if zone is full
    int full_rows = 0;
    for(int i = 0;i<current_rows;i++){
      if(current_empty_seats[i]==0){
        full_rows++;
      }
    }

    return_code = pthread_mutex_unlock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    if(full_rows == current_rows){

      return_code = pthread_mutex_lock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = clock_gettime(CLOCK_REALTIME,&no_seats_wait);
      if(return_code==-1){
        printf("ERROR: Failure getting real time.");
        pthread_exit(&return_code);
      }

      time_to_complete_reservations += ( no_seats_wait.tv_sec - start.tv_sec );

      return_code = pthread_mutex_unlock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_lock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      printf("%d : Your reservation was canceled, because the zone is fully booked.\n",*tid);

      return_code = pthread_mutex_unlock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_lock(&tel_mutex);
    	if (return_code != 0) {
    		printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
    		pthread_exit(&return_code);
    	}

      tel_counter --;

      return_code = pthread_cond_broadcast(&tel_cond);
    	if (return_code != 0) {
    		printf("ERROR: return code from pthread_cond_broadcast() is %d\n", return_code);
    		pthread_exit(&return_code);
    	}

      return_code = pthread_mutex_unlock(&tel_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      pthread_exit(thread_id);
    }

    //check if we have available seats in the chosen zone for the number of tickets the customer wants and if not print error and exit thread


    return_code = pthread_mutex_lock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }


    int not_available_rows = 0;
    for(int j = 0;j<current_rows;j++){
      if(current_empty_seats[j] <= n_tickets){
        not_available_rows++;
      }
    }

    return_code = pthread_mutex_unlock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }


    if(not_available_rows==current_rows){

      return_code = pthread_mutex_lock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = clock_gettime(CLOCK_REALTIME,&no_seats_wait);
      if(return_code==-1){
        printf("ERROR: Failure getting real time.");
        pthread_exit(&return_code);
      }

      time_to_complete_reservations += ( no_seats_wait.tv_sec - start.tv_sec );

      return_code = pthread_mutex_unlock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_lock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      printf("%d : Your reservation was canceled because there were no more seats available in this zone.\n",*tid);

      return_code = pthread_mutex_unlock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_lock(&tel_mutex);
    	if (return_code != 0) {
    		printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
    		pthread_exit(&return_code);
    	}

      tel_counter --;

      return_code = pthread_cond_broadcast(&tel_cond);
    	if (return_code != 0) {
    		printf("ERROR: return code from pthread_cond_broadcast() is %d\n", return_code);
    		pthread_exit(&return_code);
    	}

      return_code = pthread_mutex_unlock(&tel_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      pthread_exit(thread_id);
    }

    return_code = pthread_mutex_lock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    //bind the seats
    int row_to_bind;
    for(row_to_bind = 0;row_to_bind<current_rows;row_to_bind++){
      if((current_empty_seats[row_to_bind]) >= n_tickets){
        for(int j = 0;j<n_tickets;j++){
          current_zone[row_to_bind][n_seat_per_row-(current_empty_seats[row_to_bind])] = *tid;
          (current_empty_seats[row_to_bind])--;
        }
        break;
      }
    }
    return_code = pthread_mutex_unlock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }



    /*
    for (int i=n_seat-available_tickets; i<n_seat-available_tickets+n_tickets; i++) {
      seats[i]= *tid;
      seat_numbers[seats_to_reserve++]=i;
    }

    available_tickets -= n_tickets;
    */

    return_code = pthread_mutex_lock(&tel_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    tel_counter --;

    return_code = pthread_cond_broadcast(&tel_cond);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_cond_broadcast() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_unlock(&tel_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    //Cashier
    return_code = pthread_mutex_lock(&cashier_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    if(cas_counter==n_cash){
      while (TRUE) {
    		return_code = pthread_cond_wait(&cashier_cond, &cashier_mutex);
    		if (return_code != 0) {
    			printf("ERROR: return code from pthread_cond_wait() is %d\n", return_code);
    			pthread_exit(&return_code);
    		}
    	}
    }

    cas_counter++;

    return_code = pthread_mutex_unlock(&cashier_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }


    int c_wait = (rand_r(&seed) % (t_cashhigh-t_cashlow+1)) + t_cashlow ;
    int card_accepted =  rand() <  p_cardsucces * ((double)RAND_MAX + 1.0); //find if the card is accepted

    //sleep(c_wait);

    //if it isn't unbind the seats
    if(card_accepted != 1){
      return_code = pthread_mutex_lock(&seat_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      for(int j = 0;j<n_tickets;j++){
        current_zone[row_to_bind][n_seat_per_row-(current_empty_seats[row_to_bind])-1] = 0;
        (current_empty_seats[row_to_bind])++;
      }

      return_code = pthread_mutex_unlock(&seat_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }



      return_code = pthread_mutex_lock(&cashier_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      cas_counter --;

      return_code = pthread_cond_broadcast(&cashier_cond);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_cond_broadcast() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_unlock(&cashier_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

    }


    //if the card is not accepted print error and exit the thread
    if(card_accepted!=1){
      return_code = pthread_mutex_lock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = clock_gettime(CLOCK_REALTIME,&no_seats_wait);
      if(return_code==-1){
        printf("ERROR: Failure getting real time.");
        pthread_exit(&return_code);
      }

      time_to_complete_reservations += ( no_seats_wait.tv_sec - start.tv_sec );

      return_code = pthread_mutex_unlock(&time_to_complete_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      return_code = pthread_mutex_lock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      printf("\n%d : The reservation was canceled because the transaction with your card was declined\n",*tid);

      return_code = pthread_mutex_unlock(&screen_lock_mutex);
      if (return_code != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
        pthread_exit(&return_code);
      }

      pthread_exit(thread_id);
    }

    //calculate the price fore the tickets
    transaction_cost = n_tickets * (current_c_cost);

    //lock transaction mutex
    return_code = pthread_mutex_lock(&transaction_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    transactions++; //increases the number of transactions
    account_summary+=transaction_cost; //adds the transaction cost to theatre's bank account

    //unlock transaction mutex
    return_code = pthread_mutex_unlock(&transaction_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_lock(&time_to_complete_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = clock_gettime(CLOCK_REALTIME,&no_seats_wait);
    if(return_code==-1){
      printf("ERROR: Failure getting real time.");
      pthread_exit(&return_code);
    }

    time_to_complete_reservations += ( no_seats_wait.tv_sec - start.tv_sec );

    return_code = pthread_mutex_unlock(&time_to_complete_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_lock(&screen_lock_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    printf("\n%d : The reservation was successful. The transaction id is %d, your seats are :\n Zone %c Row %d : \n",*tid,*tid,zone,row_to_bind+1);
    /*
    for(int z=0;z<n_tickets-1;z++){
      printf("%d,",(current_zone[row_to_bind][n_seat_per_row - (current_empty_seats[row_to_bind]) - z]));
    }
    */

    return_code = pthread_mutex_lock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }


    for(int z=n_tickets;z>1;z--){
      printf("%d,",(n_seat_per_row-current_empty_seats[row_to_bind]-z+1));
    }
    printf("%d\nand the cost of the transaction is %d.\n",n_seat_per_row-current_empty_seats[row_to_bind],transaction_cost);

    return_code = pthread_mutex_unlock(&seat_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_unlock(&screen_lock_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_lock(&cashier_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_lock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    cas_counter --;

    return_code = pthread_cond_broadcast(&cashier_cond);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_cond_broadcast() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    return_code = pthread_mutex_unlock(&cashier_mutex);
    if (return_code != 0) {
      printf("ERROR: return code from pthread_mutex_unlock() is %d\n", return_code);
      pthread_exit(&return_code);
    }

    pthread_exit(thread_id);
}

int main(int argc,char **argv){

    if(n_seatlow>n_seat_per_row){
      printf("Error: The lowest number of seats per customer cannot be more than the number of seats per row.\n");
      exit(0);
    }

    if(argc!=3){
        printf("Error: Please pass 2 aruments only\n");
        exit(0);
    }

    n_cust = atoi(argv[1]); //pairnei string kai to kanei int
    seed = strtoul(argv[2],0L,10);

     if (n_cust < 0) {
        printf("ERROR: the number of customers to run should be a positive number. Current number given %d.\n", n_cust);
        exit(-1);
     }

    pthread_t *threads;
    tel_counter = 0;
    cas_counter = 0;

    threads =malloc(n_cust * sizeof(pthread_t));
    if (threads == NULL) {
        printf("Not enough memory.\n");
        return -1;
    }



    seats_zone_A =malloc(n_zone_A * sizeof(int *));
    for (int i=0; i<n_zone_A; i++)
         seats_zone_A[i] = malloc(n_seat_per_row * sizeof(int));
    if (seats_zone_A == NULL) {
        printf("Not enough memory.\n");
        return -1;
    }

    for(int i=0;i<n_zone_A;i++){
      for(int j=0;j<n_seat_per_row;j++){
        seats_zone_A[i][j] = 0;
      }
    }

    seats_zone_B = malloc(n_zone_B * sizeof(int *));
    for (int i=0; i<n_zone_B; i++)
         seats_zone_B[i] = malloc(n_seat_per_row * sizeof(int));
    if (seats_zone_B == NULL) {
        printf("Not enough memory.\n");
        return -1;
    }

    for(int i=0;i<n_zone_B;i++){
      for(int j=0;j<n_seat_per_row;j++){
        seats_zone_B[i][j] = 0;
      }
    }

    seats_zone_C = malloc(n_zone_C * sizeof(int *));
    for (int i=0; i<n_zone_C; i++)
         seats_zone_C[i] = malloc(n_seat_per_row * sizeof(int));
    if (seats_zone_C == NULL) {
        printf("Not enough memory.\n");
        return -1;
    }

    for(int i=0;i<n_zone_C;i++){
      for(int j=0;j<n_seat_per_row;j++){
        seats_zone_C[i][j] = 0;
      }
    }

    for(int i = 0;i<n_zone_A;i++){
      empty_seats_A[i] = n_seat_per_row;
    }
    for(int i = 0;i<n_zone_B;i++){
      empty_seats_B[i] = n_seat_per_row;
    }
    for(int i = 0;i<n_zone_C;i++){
      empty_seats_C[i] = n_seat_per_row;
    }

    int return_code;
    int thread_count;
    int t_counter = 0;
    int counter[n_cust];
    for(thread_count = 0; thread_count < n_cust; thread_count++) {
        counter[thread_count] = (++t_counter) + 1;
        return_code = pthread_create(&threads[thread_count], NULL, purchase_seat, &counter[thread_count]);
        if (return_code != 0) {
            printf("ERROR: return code from pthread_create() is %d\n", return_code);
            exit(-1);
        }
    }

    void *status;
    for (thread_count= 0; thread_count < n_cust; thread_count++) {
        return_code = pthread_join(threads[thread_count], &status);

        if (return_code != 0) {
            printf("ERROR: return code from pthread_join() is %d\n", return_code);
            exit(-1);
        }
    }

    //print seat plan

    //Zone A
    printf("Seats plan : \nZone A:\n");
    for(int i=0;i<n_zone_A;i++){
      printf("Row %d : \n",i);
      for(int j=0;j<n_seat_per_row;j++){
        if(seats_zone_A[i][j]!=0)
          printf("Seat %d / Client %d\n",j+1,seats_zone_A[i][j]);
        else
          printf("Seat %d / Empty\n",j+1);
      }
    }

    //Zone B
    printf("\nZone B:\n");
    for(int i=0;i<n_zone_B;i++){
      printf("Row %d : \n",i);
      for(int j=0;j<n_seat_per_row;j++){
        if(seats_zone_B[i][j]!=0)
          printf("Seat %d / Client %d\n",j+1,seats_zone_B[i][j]);
        else
          printf("Seat %d / Empty\n",j+1);
      }
    }

    //Zone C
    printf("\nZone C:\n");
    for(int i=0;i<n_zone_B;i++){
      printf("Row %d : \n",i);
      for(int j=0;j<n_seat_per_row;j++){
        if(seats_zone_C[i][j]!=0)
          printf("Seat %d / Client %d\n",j+1,seats_zone_C[i][j]);
        else
          printf("Seat %d / Empty\n",j+1);
      }
    }

    //print the total revenue of theatre's bank account
    printf("\nThe total revenue is: %d.\n",account_summary);
    //printf("The average wait time to speak to the telephone is %.2f nsec\n",(tel_wait_total/n_cust));
    //printf("The average wait time to complete (with or without failure) the reservation is %.2f secs\n",(time_to_complete_reservations/n_cust));


    for (int i=0; i<n_zone_A; ++i){
      free(seats_zone_A[i]);
    }
    free(seats_zone_A);

    for (int i=0; i<n_zone_B; ++i){
      free(seats_zone_B[i]);
    }
    free(seats_zone_B);

    for (int i=0; i<n_zone_C; ++i){
      free(seats_zone_C[i]);
    }
    free(seats_zone_C);

    free(threads);
    return 1;
}
