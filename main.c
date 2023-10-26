#include "main.h"

void *order(void *order_id){
    struct timespec order_start;
    struct timespec order_bake;
    struct timespec order_packed;
    struct timespec order_deliver;
    int rc;
    int *oid;
    oid = (int *)order_id;
    unsigned int seedp = seed^(*oid);

    int no_pizzas = rand_r(&seedp) % N_order_high + N_order_low;
    int current_plain = 0;
    int current_special = 0;

    srand(getpid() + time(NULL));

    for (int i = 0; i < no_pizzas; i++)
    {
        int random_number = rand() % 100;
        if(random_number <= P_plain){
            current_plain++;
        }
        else{
            current_special++;
        }
    }
    payment_time = rand_r(&seedp) % T_payment_high + T_payment_low;
    sleep(payment_time);

    srand(getpid() + time(NULL));
    int fail = rand() % 100;

    if(fail <= P_fail){
        rc = pthread_mutex_lock(&mutex_lock_screen);
        if(rc != 0){
            printf("Locking screen error during order %i\n", rc);
            pthread_exit(&rc);
        }
        printf("Order %i was failed.\n", *oid);
        rc = pthread_mutex_unlock(&mutex_lock_screen);
        if(rc != 0){
            printf("Unlocking screen error during order %i\n", rc);
            pthread_exit(&rc);
        }
        ++failed_orders;
        pthread_exit(&rc);
    }
    else{
        rc = pthread_mutex_lock(&mutex_lock_screen);
        if(rc != 0){
            printf("Locking screen error during order %i\n", rc);
            pthread_exit(&rc);
        }
        printf("Order %i was successfully placed.\n", *oid);
        rc = pthread_mutex_unlock(&mutex_lock_screen);
        if(rc != 0){
            printf("Unlocking screen error during order %i\n", rc);
            pthread_exit(&rc);
        }
        total_special += current_special;
        total_plain += current_plain;
        total_revenue += current_plain*C_plain;
        total_revenue += current_special*C_special;
        ++successful_orders;
    }
    clock_gettime(CLOCK_REALTIME, &order_start);

    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Taking order %i.\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }


    rc = pthread_mutex_lock(&mutex_no_available_cooks);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    while(no_available_cooks<=0){
        rc = pthread_cond_wait(&cond_no_available_cooks, &mutex_no_available_cooks);
        if(rc != 0){
            printf("Error condition_wait() -> %i\n", rc);
            pthread_exit(&rc);
        }
    }
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Cook was found for order %i.\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    --no_available_cooks;
    rc = pthread_mutex_unlock(&mutex_no_available_cooks);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    sleep(no_pizzas*T_prep);
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %d\n", rc);
        pthread_exit(&rc);
    }
    printf("%i pizzas prepared for order %i.\n", no_pizzas, *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_no_available_ovens);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    while(no_available_ovens<no_pizzas){
        rc = pthread_cond_wait(&cond_no_available_ovens, &mutex_no_available_ovens);
        if(rc != 0){
            printf("Error condition_wait() -> %i\n", rc);
            pthread_exit(&rc);
        }
    }
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Oven found for order %i\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    --no_available_ovens;
    rc = pthread_mutex_unlock(&mutex_no_available_ovens);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_no_available_cooks);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    ++no_available_cooks;
    pthread_cond_signal(&cond_no_available_cooks);
    rc = pthread_mutex_unlock(&mutex_no_available_cooks);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    sleep(T_bake);
    clock_gettime(CLOCK_REALTIME, &order_bake);
    rc = pthread_mutex_lock(&mutex_no_available_packers);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    while(no_available_packers<=0){
        rc = pthread_cond_wait(&cond_no_available_packers, &mutex_no_available_packers);
        if(rc != 0){
            printf("Error condition_wait() -> %i\n", rc);
            pthread_exit(&rc);
        }
    }
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Packer found for order %i\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    --no_available_packers;
    rc = pthread_mutex_unlock(&mutex_no_available_packers);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    } 
    rc = pthread_mutex_lock(&mutex_no_available_ovens);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    ++no_available_ovens;
    pthread_cond_signal(&cond_no_available_ovens);
    rc = pthread_mutex_unlock(&mutex_no_available_ovens);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }
    sleep(T_pack*no_pizzas);
    clock_gettime(CLOCK_REALTIME, &order_packed);
    double time_ready = order_packed.tv_sec-order_start.tv_sec;
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Order for %i ready in %f!\n", *oid, time_ready);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_no_available_packers);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    ++no_available_packers;
    pthread_cond_signal(&cond_no_available_packers);
    rc = pthread_mutex_unlock(&mutex_no_available_packers);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);

    }
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Pizzas for order %i are ready, waiting for deliverer!\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_no_available_deliverers);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    while(no_available_deliverers<=0){
        rc = pthread_cond_wait(&cond_no_available_deliverers, &mutex_no_available_deliverers);
        if(rc != 0){
            printf("Error condition_wait() -> %i\n", rc);
            pthread_exit(&rc);
        }
    }
    --no_available_deliverers;
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    printf("Deliverer found for order %i\n", *oid);
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&mutex_no_available_deliverers);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }
    double T_delivery = rand_r(&seedp)%T_high + T_low;
    sleep(T_delivery);
    clock_gettime(CLOCK_REALTIME, &order_deliver);
    sleep(T_delivery);
    rc = pthread_mutex_lock(&mutex_no_available_deliverers);
    if(rc != 0){
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    ++no_available_deliverers;
    pthread_cond_signal(&cond_no_available_deliverers);
    rc = pthread_mutex_unlock(&mutex_no_available_deliverers);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    rc = pthread_mutex_lock(&mutex_total_cold_time);
    if(rc != 0) {
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    cold_time = order_deliver.tv_sec - order_bake.tv_sec;
    total_cold_time += cold_time;
    rc = pthread_mutex_unlock(&mutex_total_cold_time);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    rc = pthread_mutex_lock(&mutex_max_cold_time);
    if(rc != 0) {
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    if(cold_time>max_cold_time){
        max_cold_time = cold_time;
    }
    rc = pthread_mutex_unlock(&mutex_max_cold_time);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    rc = pthread_mutex_lock(&mutex_total_order_time);
    if(rc != 0) {
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_lock_screen);
    if(rc != 0){
        printf("Locking screen error %i\n", rc);
        pthread_exit(&rc);
    }

    order_time = order_deliver.tv_sec-order_start.tv_sec;
    total_order_time += order_time;
    printf("Order %i delivered in %f minutes.\n", *oid, order_time);
    printf("Order %i was freezing for %f minutes.\n", *oid, cold_time);
    
    rc = pthread_mutex_unlock(&mutex_lock_screen);
    if(rc != 0){
        printf("Unlocking screen error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&mutex_total_order_time);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }
    rc = pthread_mutex_lock(&mutex_max_order_time);
    if(rc != 0) {
        printf("Locking error %i\n", rc);
        pthread_exit(&rc);
    }
    if(order_time>max_order_time){
        max_order_time = order_time;
    }
    rc = pthread_mutex_unlock(&mutex_max_order_time);
    if(rc != 0){
        printf("Unlocking error %i\n", rc);
        pthread_exit(&rc);
    }

    pthread_exit(oid);
}

int main(int argc, char *argv[]) {

    if(argc != 3) {
        printf("Parameter error.\n");
        exit(-1);
    }
    if(N_cust <0) {
        printf("Customer number error.\n");
        exit(-1);
    }
    int N_cust = atoi(argv[1]);
    seed = atoi(argv[2]);

    no_available_cooks = N_cook;
    no_available_ovens = N_oven;
    no_available_packers = N_packer;
    no_available_deliverers = N_deliverer;
    order_time = 0;
    total_order_time = 0;
    max_order_time = 0;
    total_cold_time = 0;
    max_cold_time = 0;

    int order_id[N_cust];
    for(int i=0;i<N_cust;++i)
    {
        order_id[i] = i+1;
    }

    pthread_t *threads;
    threads = malloc(N_cust*sizeof(pthread_t));
    if(threads==NULL){
        printf("Error: Memory Allocation.\n");
        exit(-1);
    }

    int rc;
    for(int i=0; i<N_cust; ++i)
    {
        rc = pthread_create(&threads[i], NULL, &order, &order_id[i]);
        double wait = rand_r(&seed)%T_order_high+T_order_low;
        sleep(wait);
        if (rc != 0) {
            printf("Error: pthread_create() -> %i\n", rc);
            exit(-1);
        }
    }

    void *status;
    for(int i=0; i<N_cust; i++)
    {
        rc = pthread_join(threads[i], &status);
        if (rc != 0) {
            printf("Error: pthread_join() -> %i on thread %i\n", rc, order_id[i]);
            exit(-1);
        }
    }

    printf("\n\n\n\nResults:\n\n");
    printf("Average order time is %f\n", total_order_time/successful_orders);
    printf("Max order time is %f\n", max_order_time);
    printf("Average freezing time is %f\n", total_cold_time/successful_orders);
    printf("Max freezing time is %f\n\n", max_cold_time);
    printf("Total revenue is %i\n", total_revenue);
    printf("Total successful orders: %i\n", successful_orders);
    printf("Total failed orders: %i\n", failed_orders);
    printf("Total plain pizzas : %i\n", total_plain);
    printf("Total special pizzas : %i\n", total_special);
    free(threads);
    return 1;
}