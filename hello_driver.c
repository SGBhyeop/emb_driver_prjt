#include <linux/init.h>     // 커널 초기화 관련 헤더
#include <linux/module.h>   // 커널 모듈 관련 필수 헤더
#include <linux/kernel.h>   // 커널 로그(printk)를 위한 헤더
#include <linux/fs.h>
#include <linux/cdev.h>    // 문자 장치(cdev) 구조체 관련
#include <linux/uaccess.h> // 유저 영역과 데이터 주고받기 (copy_to_user 등)

#define DEVICE_NAME "samsung_dev"

static dev_t dev_num;
static struct cdev my_cdev;

// 1. 응용 프로그램이 파일을 열 때 실행
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Samsung 드라이버: 누군가 나를 열었습니다!\n");
    return 0;
}

// 2. 응용 프로그램이 파일을 닫을 때 실행
static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Samsung 드라이버: 누군가 나를 닫았습니다.\n");
    return 0;
}

// 3. 응용 프로그램이 드라이버로부터 데이터를 읽어갈 때 실행
static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    char *msg = "삼성 공기청정기 상태: 정상";
    int msg_len = strlen(msg);
    
    // 유저 영역(앱)으로 데이터를 안전하게 복사 (매우 중요)
    if (copy_to_user(buffer, msg, msg_len)) {
        return -EFAULT;
    }
    
    printk(KERN_INFO "Samsung 드라이버: 앱에게 상태를 보고했습니다.\n");
    return 0; // 읽기 완료
}

// 4. 응용 프로그램이 드라이버에게 명령을 쓸 때 실행
static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    char input_data[10];
    
    // 앱이 보낸 데이터를 커널 메모리로 안전하게 가져옴
    if (copy_from_user(input_data, buffer, len)) {
        return -EFAULT;
    }
    
    input_data[len] = '\0';
    printk(KERN_INFO "Samsung 드라이버: 앱으로부터 명령 수신 -> %s\n", input_data);
    
    return len;
}

// 5. 드라이버 함수 지도 (File Operations)
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init hello_init(void) {
    // 장치 번호 할당
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    
    // cdev 구조체 초기화 및 커널 등록
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);
    
    printk(KERN_INFO "Samsung 드라이버: 준비 완료! Major:%d\n", MAJOR(dev_num));
    return 0;
}

static void __exit hello_exit(void) {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Samsung 드라이버: 제거되었습니다.\n");
}

MODULE_LICENSE("GPL"); // 이 드라이버의 라이선스 (필수)
MODULE_DESCRIPTION("System SW Study Driver");
