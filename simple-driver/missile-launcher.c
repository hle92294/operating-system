#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h> 
#include <linux/mutex.h>            
#include <linux/kobject.h>   
#include <linux/kthread.h>  
#include <linux/delay.h>      

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hao Le, Ramsey Peter, Seana Morgan");
MODULE_DESCRIPTION("missile Launcher Driver for Dream Cheeky");
MODULE_VERSION("0.1");

/*------------------------------------------------
					INITIALIZE   
--------------------------------------------------*/
static DEFINE_MUTEX(fire_lock);
static DEFINE_MUTEX(fire_one_lock);
static DEFINE_MUTEX(fire_all_lock);
static DEFINE_MUTEX(rotate_right_lock);
static DEFINE_MUTEX(rotate_left_lock);
static DEFINE_MUTEX(tilt_up_lock);
static DEFINE_MUTEX(tilt_down_lock);
static DEFINE_MUTEX(count_remain_lock);

static unsigned int gpio_fire 				= 26;   
module_param(gpio_fire,uint,S_IRUGO);
MODULE_PARM_DESC(gpio_fire, "To fire (default=26)"); 

static unsigned int gpio_rotate_right 		= 27;  
module_param(gpio_rotate_right,uint,S_IRUGO);
MODULE_PARM_DESC(gpio_rotate_right, "To turn clockwise (default=27)"); 

static unsigned int gpio_rotate_left 		= 22;
module_param(gpio_rotate_left,uint,S_IRUGO);
MODULE_PARM_DESC(gpio_rotate_left, "To turn counter-clockwise (default=22)"); 

static unsigned int gpio_tilt_up			= 17;
module_param(gpio_tilt_up,uint,S_IRUGO);
MODULE_PARM_DESC(gpio_tilt_up, "To raise the turret(default=17)"); 

static unsigned int gpio_tilt_down 			= 4;
module_param(gpio_tilt_down,uint,S_IRUGO);
MODULE_PARM_DESC(gpio_tilt_down, "To lower turret (default=4)"); 

static unsigned int FIRE_ONE 				= 0;
module_param(FIRE_ONE,uint,0664);
MODULE_PARM_DESC(FIRE_ONE, " Firing One Interface To fire a missile set to 1");     ///< parameter description

static unsigned int FIRE_ALL 				= 0;
module_param(FIRE_ALL,uint,0664);
MODULE_PARM_DESC(FIRE_ALL, " Firing All Interface To fire all missile set to 1");

static unsigned int NR_MISSILES_REMAINING 	= 4;
module_param(NR_MISSILES_REMAINING,uint,0664);
MODULE_PARM_DESC(NR_MISSILES_REMAINING, " Number of missile remaing (max = 4 and min = 0, default=4)");

static unsigned int SET_NR_MISSILES 		= 0;
module_param(SET_NR_MISSILES,uint,0664);
MODULE_PARM_DESC(SET_NR_MISSILES, "Set missile (max = 4 and min = 0)");

static unsigned int NR_UP 					= 0;
module_param(NR_UP,uint,0664);
MODULE_PARM_DESC(NR_UP, "Raise the turret up (max = 50 and min = 0)");

static unsigned int NR_DOWN 				= 0;
module_param(NR_DOWN,uint,0664);
MODULE_PARM_DESC(NR_DOWN, "Lower the turret (max = 50 and min = 0)");

static unsigned int NR_RIGHT 				= 0;
module_param(NR_RIGHT,uint,0664);
MODULE_PARM_DESC(NR_RIGHT, "Turn right (max = 10 and min = 0)");

static unsigned int NR_LEFT 				= 0;
module_param(NR_LEFT,uint,0664);
MODULE_PARM_DESC(NR_LEFT, "Turn lef (max = 10 and min = 0)");

static char module_name[8] = "missile";
static bool isEmpty = 0; 
/*------------------------------------------------
					COMMAND HANDLERS
--------------------------------------------------*/
static ssize_t count_missiles(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"missiles remaining: %d\n", NR_MISSILES_REMAINING);
}

static ssize_t missiles_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&count_remain_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	if ((missiles>=0)&&(missiles <=4)){
		NR_MISSILES_REMAINING = missiles;
	} 
	mutex_unlock(&count_remain_lock);
	return count;
}

static ssize_t fire_one_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"single shot: %d\n", FIRE_ONE);
}

static ssize_t fire_one_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&fire_one_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	if (missiles == 1){
		FIRE_ONE = missiles;
	} 
	mutex_unlock(&fire_one_lock);
	return count;
}

static ssize_t fire_all_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"multiple shot: %d\n", FIRE_ALL);
}

static ssize_t fire_all_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&fire_all_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	if (missiles == 1){
		FIRE_ALL = missiles;
	} 
	mutex_unlock(&fire_all_lock);
	return count;
}

static ssize_t rotate_right_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"rotating right: %d\n", NR_RIGHT);
}

static ssize_t rotate_right_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&rotate_right_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	NR_RIGHT = missiles; 
	mutex_unlock(&rotate_right_lock);
	return count;
}

static ssize_t rotate_left_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"rotating left: %d\n", NR_LEFT);
}

static ssize_t rotate_left_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&rotate_left_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	NR_LEFT = missiles; 
	mutex_unlock(&rotate_left_lock);
	return count;
}

static ssize_t tilt_up_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"tilting up: %d\n", NR_UP);
}

static ssize_t tilt_up_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&tilt_up_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	NR_UP = missiles; 
	mutex_unlock(&tilt_up_lock);
	return count;
}

static ssize_t tilt_down_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf,"tilting down: %d\n", NR_DOWN);
}

static ssize_t tilt_down_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	mutex_lock(&tilt_down_lock);
	unsigned int missiles;
	sscanf(buf,"%du",&missiles);
	NR_DOWN = missiles; 
	mutex_unlock(&tilt_down_lock);
	return count;
}

/*------------------------------------------------
					SETTING UP  
--------------------------------------------------*/

static struct kobj_attribute count_remain_attr 	= __ATTR(NR_MISSILES_REMAINING,(S_IWUSR|S_IRUGO),count_missiles, missiles_store);
static struct kobj_attribute fire_one_attr 		= __ATTR(FIRE_ONE,(S_IWUSR|S_IRUGO),fire_one_show, fire_one_store);
static struct kobj_attribute fire_all_attr 		= __ATTR(FIRE_ALL,(S_IWUSR|S_IRUGO),fire_all_show, fire_all_store);
static struct kobj_attribute rotate_right_attr 	= __ATTR(NR_RIGHT,(S_IWUSR|S_IRUGO),rotate_right_show, rotate_right_store);
static struct kobj_attribute rotate_left_attr 	= __ATTR(NR_LEFT,(S_IWUSR|S_IRUGO),rotate_left_show, rotate_left_store);
static struct kobj_attribute tilt_up_attr		= __ATTR(NR_UP,(S_IWUSR|S_IRUGO),tilt_up_show, tilt_up_store);
static struct kobj_attribute tilt_down_attr		= __ATTR(NR_DOWN,(S_IWUSR|S_IRUGO),tilt_down_show, tilt_down_store);

static struct attribute *module_attrs[] = {
	&count_remain_attr.attr,
	&fire_one_attr.attr,
	&fire_all_attr.attr,
	&rotate_right_attr.attr,
	&rotate_left_attr.attr,
	&tilt_up_attr.attr,
	&tilt_down_attr.attr,
	NULL,  
};

static struct attribute_group attr_group = {
	.name = module_name,
	.attrs = module_attrs,
};

static struct kobject *module_kobj;
static struct task_struct *task_fire;
static struct task_struct *task_rotate_left;
static struct task_struct *task_rotate_right;
static struct task_struct *task_tilt_up;
static struct task_struct *task_tilt_down;

/*------------------------------------------------
					CORE-FUNCTION
--------------------------------------------------*/
static int fire(void *arg) {
	printk(KERN_INFO "Thread #fire: started\n");
	while(!kthread_should_stop()){
		if(NR_MISSILES_REMAINING == 0){
			FIRE_ONE = 0;
			FIRE_ALL = 0;
			msleep(500);
			continue;
		}
		mutex_lock(&fire_lock);
		set_current_state(TASK_RUNNING);
		if ((FIRE_ONE == 1) && (NR_MISSILES_REMAINING > 0)) {
			gpio_set_value(gpio_fire, FIRE_ONE);
			msleep(4800);
			mutex_lock(&count_remain_lock);
			NR_MISSILES_REMAINING--;
			mutex_unlock(&count_remain_lock);
			mutex_lock(&fire_one_lock);
			FIRE_ONE = 0;
			mutex_unlock(&fire_one_lock);
			gpio_set_value(gpio_fire, FIRE_ONE);
		}
		if ((FIRE_ALL == 1) &&( NR_MISSILES_REMAINING > 0)) {
			gpio_set_value(gpio_fire, FIRE_ALL);
			msleep(4800*NR_MISSILES_REMAINING);
			mutex_lock(&count_remain_lock);
			NR_MISSILES_REMAINING = 0;
			mutex_unlock(&count_remain_lock);
			mutex_lock(&fire_all_lock);
			FIRE_ALL = 0;
			mutex_unlock(&fire_all_lock);
			gpio_set_value(gpio_fire, FIRE_ALL);
		}
		mutex_unlock(&fire_lock);
		set_current_state(TASK_INTERRUPTIBLE);
	}
	printk(KERN_INFO "Thread #fire: completed\n");
	return 0;

}
static int rotate_left(void *arg) {
	printk(KERN_INFO "Thread #rotation left: started\n");
	while(!kthread_should_stop()){
		mutex_lock(&fire_lock);
		set_current_state(TASK_RUNNING);
		if (NR_LEFT > 0) {
			gpio_set_value(gpio_rotate_left,true);
			msleep(gpio_rotate_left * 10);
			gpio_set_value(gpio_rotate_left,false);
			mutex_lock(&rotate_left_lock);
			NR_LEFT = 0;
			mutex_unlock(&rotate_left_lock);
		}
		mutex_unlock(&fire_lock);
		set_current_state(TASK_INTERRUPTIBLE);
	}
	printk(KERN_INFO "Thread #rotation left: completed\n");
	return 0;
}

static int rotate_right(void *arg) {
	printk(KERN_INFO "Thread #rotation right: started\n");
	while(!kthread_should_stop()){
		mutex_lock(&fire_lock);
		set_current_state(TASK_RUNNING);
		if (NR_RIGHT > 0) {
			gpio_set_value(gpio_rotate_right,true);
			msleep(gpio_rotate_right * 10);
			gpio_set_value(gpio_rotate_right,false);
			mutex_lock(&rotate_right_lock);
			NR_RIGHT = 0;
			mutex_unlock(&rotate_right_lock);
		}
		mutex_unlock(&fire_lock);
		set_current_state(TASK_INTERRUPTIBLE);
	}
	printk(KERN_INFO "Thread #rotation right: completed\n");
	return 0;
}

static int tilt_up(void *arg) {
	printk(KERN_INFO "Thread #tilt up: started\n");
	while(!kthread_should_stop()){
		mutex_lock(&fire_lock);
		set_current_state(TASK_RUNNING);
		if (NR_UP > 0) {
			gpio_set_value(gpio_tilt_up,true);
			msleep(gpio_tilt_up * 50);
			gpio_set_value(gpio_tilt_up,false);
			mutex_lock(&tilt_up_lock);
			NR_UP = 0;
			mutex_unlock(&tilt_up_lock);
		}
		mutex_unlock(&fire_lock);
		set_current_state(TASK_INTERRUPTIBLE);
	}
	printk(KERN_INFO "Thread #tilt up: completed\n");
	return 0;
	
}
static int tilt_down(void *arg) {
	printk(KERN_INFO "Thread #tilt down: started\n");
	while(!kthread_should_stop()){
		mutex_lock(&fire_lock);
		set_current_state(TASK_RUNNING);
		if (NR_DOWN > 0) {
			gpio_set_value(gpio_tilt_down,true);
			msleep(gpio_tilt_down * 50);
			gpio_set_value(gpio_tilt_down,false);
			mutex_lock(&tilt_down_lock);
			NR_DOWN = 0;
			mutex_unlock(&tilt_down_lock);
		}
		mutex_unlock(&fire_lock);
		set_current_state(TASK_INTERRUPTIBLE);
	}
	printk(KERN_INFO "Thread #tilt down: completed\n");
	return 0;
	
}

/*------------------------------------------------
						INIT
--------------------------------------------------*/
// To load the module
static int __init missile_init(void)
{
	int error;
	module_kobj = kobject_create_and_add("dream-cheeky", kernel_kobj->parent);

	if (!module_kobj){
	printk(KERN_ALERT "Failed to create kobject\n");
	kobject_put(module_kobj);
	return -ENOMEM;
	}

	error = sysfs_create_group(module_kobj, &attr_group);
	if (error){
	printk(KERN_ALERT "Failed to create sysfs group\n");
	kobject_put(module_kobj);
	return error;
	}

	gpio_request(gpio_fire,"sysfs");
	gpio_direction_output(gpio_fire,false);
	gpio_export(gpio_fire,false);

	gpio_request(gpio_rotate_right,"sysfs");
	gpio_direction_output(gpio_rotate_right,false);
	gpio_export(gpio_rotate_right,false);

	gpio_request(gpio_rotate_left,"sysfs");
	gpio_direction_output(gpio_rotate_left,false);
	gpio_export(gpio_rotate_left,false);

	gpio_request(gpio_tilt_up,"sysfs");
	gpio_direction_output(gpio_tilt_up,false);
	gpio_export(gpio_tilt_up,false);

	gpio_request(gpio_tilt_down,"sysfs");
	gpio_direction_output(gpio_tilt_down,false);
	gpio_export(gpio_tilt_down,false);

	task_fire 			= kthread_run(fire, NULL, "FIRING");
	task_rotate_left 	= kthread_run(rotate_left, NULL, "ROTATING LEFT");
	task_rotate_right 	= kthread_run(rotate_right, NULL, "ROTATING RIGHT");
	task_tilt_up 		= kthread_run(tilt_up, NULL, "TILTING UP");
	task_tilt_down		= kthread_run(tilt_down, NULL, "TILTING DOWN");

	if (IS_ERR((task_fire)||(task_rotate_left)||(task_rotate_right)||(task_tilt_up)||(task_tilt_down))) {
	printk(KERN_ALERT "Failed to create tasks\n");
	return PTR_ERR((task_fire)||(task_rotate_left)||(task_rotate_right)||(task_tilt_up)||(task_tilt_down));
	}

	printk("Load module completed\n");
	return error;
}

// To unload
static void __exit missile_exit(void)
{
	// Thread clean up 
	kthread_stop(task_fire);
	kthread_stop(task_rotate_left);
	kthread_stop(task_rotate_right);
	kthread_stop(task_tilt_up);
	kthread_stop(task_tilt_down);
	
	// gpio clean up 
	kobject_put(module_kobj);
	gpio_set_value(gpio_fire,0);
	gpio_set_value(gpio_rotate_right,0);
	gpio_set_value(gpio_rotate_left,0);
	gpio_set_value(gpio_tilt_up,0);
	gpio_set_value(gpio_tilt_down,0);
	
    gpio_unexport(gpio_fire);
	gpio_unexport(gpio_rotate_right);
	gpio_unexport(gpio_rotate_left);
	gpio_unexport(gpio_tilt_up);
	gpio_unexport(gpio_tilt_down);
	
    gpio_free(gpio_fire);
	gpio_free(gpio_rotate_right);
	gpio_free(gpio_rotate_left);
	gpio_free(gpio_tilt_up);
	gpio_free(gpio_tilt_down);

	// locks clean up 
	mutex_destroy(&fire_lock);
    mutex_destroy(&fire_one_lock);
    mutex_destroy(&fire_all_lock);
    mutex_destroy(&rotate_right_lock);
    mutex_destroy(&rotate_left_lock);
    mutex_destroy(&tilt_up_lock);
    mutex_destroy(&tilt_down_lock);
    mutex_destroy(&count_remain_lock);

    printk(KERN_INFO "Unload module completed\n");
	return;
}

module_init(missile_init);
module_exit(missile_exit);



