#include "main.h"

pthread_mutex_t shops_mutex[SHOPS] = {PTHREAD_MUTEX_INITIALIZER};
int shops[SHOPS];
int requirements[BUYERS];

int main(void){

    for(int i = 0; i < SHOPS; i++){
        shops[i] = 9000 + random() % 2001;;
        printf("Магазин %d: %d товаров.\n", i, shops[i]);
    }
    
    for(int i = 0; i < BUYERS; i++){
        requirements[i] = 90000 + random() % 20001;;
        printf("Покупатель %d: %d потребность.\n", i, requirements[i]);
    }

    pthread_t provider;
    pthread_create(&provider, NULL, supply, (void*)shops);
    
    pthread_t buyers[BUYERS];
    BuyerData buyer_data[BUYERS];
    
    for (int i = 0; i < BUYERS; i++) {
        buyer_data[i].buyer_id = i;
        buyer_data[i].shops = shops;
        buyer_data[i].requirements = requirements;
        pthread_create(&buyers[i], NULL, shopping, (void*)&buyer_data[i]);
    }
    
    for (int i = 0; i < BUYERS; i++) {
        pthread_join(buyers[i], NULL);
    }

    pthread_cancel(provider);
    pthread_join(provider, NULL);

    return 0;
}