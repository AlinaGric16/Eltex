#include "main.h"

void* shopping(void *args){
    BuyerData* data = (BuyerData*)args;
    int buyer_id = data->buyer_id;
    int* shops = data->shops;
    int need = data->requirements[buyer_id];

    printf("Покупатель %d вошел в поток.\n\n", buyer_id);
    while(need > 0){
        for(int i = 0; i < SHOPS; i++){
            if(pthread_mutex_trylock(&shops_mutex[i]) == 0){
                if(shops[i] > 0){
                    printf("Покупатель %d вошел в магазин %d.\n\n", buyer_id, i);

                    if (shops[i] >= need) {
                        shops[i] -= need;
                        need = 0;
                    } else {
                        need -= shops[i];
                        shops[i] = 0;
                    }

                    printf("Покупатель %d забрал товары.\nОсталось потребность: %d\nВ магазине %d осталось %d товаров.\n\n", buyer_id, need, i, shops[i]);
                    pthread_mutex_unlock(&shops_mutex[i]);
                    sleep(2); 
                }
                else pthread_mutex_unlock(&shops_mutex[i]);
            }
            if(need == 0){
                printf("Покупатель %d удовлетворил все потребности!\n\n", buyer_id);
                return NULL;
            }
        }
    }
    return NULL;
}

void* supply(void* args){
    int *shops = (int*)args;
    printf("Поставщик начал работу.\n\n");
    while(1){
        for(int i = 0; i < SHOPS; i++){
            if(pthread_mutex_trylock(&shops_mutex[i]) == 0){
                shops[i] += 5000;
                printf("Поставка в магазин %d.\nВ магазине стало %d товаров.\n\n", i, shops[i]);

                pthread_mutex_unlock(&shops_mutex[i]);
                sleep(1);
            }
        }
    }
    return NULL;
}