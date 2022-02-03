#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

spinlock_t counter_lock;

struct my_list_head {
	struct my_list_head  *next_n, *prev_n;
};

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_for_each(pos, head) \
	for (pos = (head)->next_i; pos != NULL; pos = pos->next_i)

#define list_for_each_entry_index(pos, head, member)				\
	for (pos = list_first_entry_index(head, typeof(*pos), member);	\
	     &pos->member != NULL;					\
	     pos = list_next_entry_index(pos, member))
	     
#define list_first_entry_index(ptr, type, member) \
	list_entry((ptr)->next_i, type, member)
	
#define list_next_entry_index(pos, member) \
	list_entry((pos)->member.next_i, typeof(*(pos)), member)

#define list_first_entry_left(ptr, type, member) \
	list_entry((ptr)->prev_n, type, member)
	
#define list_first_entry_right(ptr, type, member) \
	list_entry((ptr)->next_n, type, member)
	
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next_n, typeof(*(pos)), member)
	
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev_n, typeof(*(pos)), member)
	
#define list_for_each_entry_left(pos, head, member)				\
	for (pos = list_first_entry_left(head, typeof(*pos), member);	\
	     &pos->member != NULL;					\
	     pos = list_prev_entry(pos, member))
	    
#define list_for_each_entry_right(pos, head, member)				\
	for (pos = list_first_entry_right(head, typeof(*pos), member);	\
	     &pos->member != NULL;					\
	     pos = list_next_entry(pos, member))

//improved version
struct my_list {
	int num;
	struct my_list_head list;
};

static inline void INIT_MY_LIST_HEAD(struct my_list_head *list)
{
	list->next_n = list;
	list->prev_n = list;
	
}



static inline void list_insert_by_value(struct my_list *pos, struct my_list_head *head, int value, int new_value) {
	struct my_list *new_node = kmalloc(sizeof(struct my_list), GFP_KERNEL);
	new_node->num = new_value;
	struct my_list_head *new = &(new_node->list);
	if (new_value%2 == 1 && head->prev_n==head) {
		
		head->prev_n = new;
		new->next_n = head;
		new->prev_n = NULL;
		return 0;
	}
	if (new_value%2 == 0 && head->next_n==head){
		
		head->next_n = new;
		new->prev_n = head;
		new->next_n = NULL;
		return 0;
	}
	if (value%2 == 1) {
		list_for_each_entry_left(pos, head, list){
			if (pos->num == value) {
				//just put in
				if(new_value%2==1){
					head->prev_n->next_n = new;
					new->prev_n = head->prev_n;
					
					head->prev_n = new;
					new->next_n = head;
				}
				
				if(new_value%2==0){
					head->next_n->prev_n = new;
					new->next_n = head->next_n;
					
					head->next_n = new;
					new->prev_n = head;
				
				}
				
				
				
				break;
			}
		}
	}
	else {
		list_for_each_entry_right(pos, head, list){
			if (pos->num == value) {
				if(new_value%2==1){
					head->prev_n->next_n = new;
					new->prev_n = head->prev_n;
					
					head->prev_n = new;
					new->next_n = head;
				}
				
				if(new_value%2==0){
					head->next_n->prev_n = new;
					new->next_n = head->next_n;
					
					head->next_n = new;
					new->prev_n = head;
				
				}
				
				
				
				break;
			}
		}
	}
}

static inline void list_delete_by_value(struct my_list *pos, struct my_list_head *head, int value) {
	if (value%2 == 1) {
		list_for_each_entry_left(pos, head, list){
			if (pos->num == value) {
				//delete from n
				struct my_list_head *pos_original_next_n = pos->list.next_n;
				if (pos->list.prev_n != NULL){
					pos->list.prev_n->next_n = pos_original_next_n;
				}
				if (pos_original_next_n !=NULL) {
					pos_original_next_n->prev_n = pos->list.prev_n;
				}
				
					
				kfree(pos);
				break;
			}
		}
	}
	else {
		list_for_each_entry_right(pos, head, list){
			if (pos->num == value) {
				struct my_list_head *pos_original_next_n = pos->list.next_n;
				if (pos->list.prev_n != NULL){
					pos->list.prev_n->next_n = pos_original_next_n;
				}
				if (pos_original_next_n !=NULL) {
					pos_original_next_n->prev_n = pos->list.prev_n;
				}
				
					
				kfree(pos);
				break;
			}
		}
	}
}

static inline void list_search_by_value(struct my_list *pos, struct my_list_head *head, int value) {
	if (value%2==1) {
		list_for_each_entry_left(pos,head,list){
			if (pos->num == value){
				break;
			}
		}
	}
	else {
		list_for_each_entry_right(pos,head,list){
			if (pos->num == value){
				break;
			}
		}
	
	}
}



void test(void){
	
	ktime_t start, end;
	s64 time_i, time_s, time_d;
	//start=ktime_get();
	//end=ktime_get();
	//time_new=ktime_to_ns(ktime_sub(end, start));
	//printk("New Insert : %lld ns\n", (long long)time_new);
	//printk("Original Insert : %lld ns\n", (long long)time_original);
	
	struct my_list_head head;
	struct my_list *mylist;
	INIT_MY_LIST_HEAD(&head);
	spin_lock(&counter_lock);
	start=ktime_get();
	int i = 0;
	for (i=2; i<100000; i++){
		list_insert_by_value(mylist, &head, i, i+1);
	}	
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_i=ktime_to_ns(ktime_sub(end, start));
	printk("Insert Time: %lld ns\n", (long long)time_i);
	
	
	/*printk("Check Insert Result\n");
	struct my_list *test1;
	list_for_each_entry_index(test1, &head, list){
		printk("traverse by index : %d\n", test1->num);
	}*/
	
	spin_lock(&counter_lock);
	start=ktime_get();
	
	list_search_by_value(mylist, &head, 50000);
	
	
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_s=ktime_to_ns(ktime_sub(end, start));
	printk("Search Time: %lld ns\n", (long long)time_s);
	
	
	
	spin_lock(&counter_lock);
	start=ktime_get();
	int j = 0;
	for (j=3; j<100001; j++ ){
		list_delete_by_value(mylist, &head, j);
	}
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_d=ktime_to_ns(ktime_sub(end, start));
	printk("Delete Time: %lld ns\n", (long long)time_d);
	
	
	
	
	
	/*printk("Delete Insert Result\n");
	struct my_list *test2;
	list_for_each_entry_index(test2, &head, list){
		printk("traverse by index : %d\n", test2->num);
	}*/
	
	/*list_for_each_entry_left(mylist, &head, list){
		printk("left num : %d\n", mylist->num);
	}
	list_for_each_entry_right(mylist, &head, list){
		printk("right num : %d\n", mylist->num);
	}*/

}

int __init project_module_init(void)
{
	spin_lock_init(&counter_lock);
	printk("Improved Linked List!\n");
	test();
	return 0;
}

void __exit project_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(project_module_init);
module_exit(project_module_cleanup);
MODULE_LICENSE("GPL");

/*#define search_by_value_odd_even(pos, head, member, value, num_name) (\
	if (value%2 == 0) {, \
		(pos, head, member) {, \
				return *pos->member, \
			}, \
		}, \
	else {, \
		list_for_each_entry_right(pos, head, member) {, \
			if(pos->name == value) {, \
				return *pos->member, \
			}, \
		}, \
	), \
	*/





