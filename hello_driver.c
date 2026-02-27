#include <linux/init.h>     // 커널 초기화 관련 헤더
#include <linux/module.h>   // 커널 모듈 관련 필수 헤더
#include <linux/kernel.h>   // 커널 로그(printk)를 위한 헤더
#include <linux/fs.h>

static dev_t dev_num;
// 1. 드라이버가 로드될 때 실행되는 함수
static int __init hello_init(void) {
    int ret = alloc_chrdev_region(&dev_num, 0, 1, "device");

    if(ret<0){
        printk(KERN_ALERT "번호 할당 실패\n");
        return ret;
    }
    printk(KERN_INFO "major number: %d, minor number: %d\n", MAJOR(dev_num), MINOR(dev_num));

    return 0;
}

// 2. 드라이버가 제거될 때 실행되는 함수
static void __exit hello_exit(void) {
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Goodbye Samsung: 드라이버가 제거되었습니다.\n");
}

// 3. 커널에 이 함수들을 등록합니다.
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL"); // 이 드라이버의 라이선스 (필수)
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Samsung System SW Study Driver");
