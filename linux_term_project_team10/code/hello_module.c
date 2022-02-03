#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

spinlock_t counter_lock;

struct my_node {
	struct list_head list;
	int data;
};

struct list_head my_list;
struct my_node *current_node;

void original(void)
{
	
	ktime_t start, end;
	s64 time_i, time_s, time_d;
	INIT_LIST_HEAD(&my_list);
	spin_lock(&counter_lock);
	start=ktime_get();
	struct my_node *temp = kmalloc(sizeof(struct my_node), GFP_KERNEL);
	temp->data = 1;
	list_add(&temp->list, &my_list);
	int i;
	for (i=2; i<100000; i++){
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_for_each_entry(current_node, &my_list, list){
			if (current_node->data == i-1) {
				list_add(&new->list, &my_list);
				break;
			}
			
		}
	}
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_i=ktime_to_ns(ktime_sub(end, start));
	printk("Original Insert : %lld ns\n", (long long)time_i);
	
	struct my_node *current_node;
	spin_lock(&counter_lock);
	start=ktime_get();
	
	list_for_each_entry(current_node, &my_list, list){

		if (current_node->data == 50000){
			break;
		}
	}
	
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_s=ktime_to_ns(ktime_sub(end, start));
	printk("Original Search : %lld ns\n", (long long)time_s);
	
	
	
	
	struct my_node *current_node3;
	spin_lock(&counter_lock);
	start=ktime_get();
	struct my_node *tmp;
	int j=0;
	for (j=3; j<100000; j++){
		list_for_each_entry_safe(current_node3, tmp, &my_list, list){
			if (current_node3->data == j){
				list_del(&current_node3->list);
				kfree(current_node3);
				break;
			}
		}
	}
	end=ktime_get();
	spin_unlock(&counter_lock);
	time_s=ktime_to_ns(ktime_sub(end, start));
	printk("Original Delete : %lld ns\n", (long long)time_s);
	
	
	
}

int __init project_module_init(void)
{
	spin_lock_init(&counter_lock);
	printk("Original Linked List!\n");
	original();
	return 0;
}

void __exit project_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(project_module_init);
module_exit(project_module_cleanup);
MODULE_LICENSE("GPL");
	

