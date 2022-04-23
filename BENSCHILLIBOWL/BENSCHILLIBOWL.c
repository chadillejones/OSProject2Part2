#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int rand_num = ((rand()%BENSCHILLIBOWLMenuLength)+1);//has to be in MenuItem
    return BENSCHILLIBOWLMenu[rand_num];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    
    BENSCHILLIBOWL *rest = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));  
    rest->current_size = 0;
    rest->max_size  = max_size;
    rest->next_order_number  = 0;
    rest->orders_handled  = 0;
    rest->expected_num_orders  = expected_num_orders;
    rest->orders = NULL;
    pthread_mutex_init(&(rest->mutex), NULL);
    pthread_cond_init(&(rest->can_add_orders),NULL);
    pthread_cond_init(&(rest->can_get_orders), NULL);
    printf("Restaurant is open!\n");

    return NULL;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  if(bcb->orders_handled == bcb->expected_num_orders)
  {
      free(bcb);//deallocate
      pthread_mutex_destroy(&(bcb->mutex));
    printf("All orders are complete\n");
    
  }
  else{
  printf("Restaurant is closed!\n"); 
  }
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
     pthread_mutex_lock((&bcb->mutex));
    while(IsFull(bcb) == true){
      pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }
    order->order_number = bcb->next_order_number;
    AddOrderToBack(&(bcb->orders),order);
    return order->order_number;    
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
  
    while(IsEmpty(bcb) == true){
      if(bcb->orders_handled >= bcb->expected_num_orders){
        pthread_mutex_unlock(&(bcb->mutex));
        return NULL;
      }
     pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }
  Order *order = bcb->orders;
  bcb->orders = bcb->orders->next;
  
  pthread_cond_broadcast(&(bcb->can_add_orders));
  pthread_mutex_unlock(&(bcb->mutex));
  
  bcb->current_size--;
  bcb->orders_handled++;
  return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    if(bcb->current_size == 0)
  {
    return true;
  }
  return false;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  if(bcb->current_size == bcb->max_size)
  {
    return true;
  }
  return false;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
  Order * current = *orders;
  while (current->next != NULL) {
      current = current->next;
    }

    /* now we can add a new variable */

    current->next = (Order *) malloc(sizeof(order));
    current->next->menu_item = order->menu_item;
    current->next->customer_id = order->customer_id;
    current->next->order_number = order->order_number;
    current->next->next = NULL;
  }


