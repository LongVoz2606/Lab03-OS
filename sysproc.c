#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int sys_pgaccess(void) {
    uint64 start_addr;
    int num_pages;
    uint64 user_buffer;

    argaddr(0, &start_addr);
    argint(1, &num_pages);
    argaddr(2, &user_buffer);

    uint8 *kernel_buffer = (uint8 *)kalloc();
    if (kernel_buffer == 0) {
        return -1;
    }
    struct proc *p = myproc();
    memset(kernel_buffer, 0, num_pages);  // Khởi tạo bộ đệm
    for (int i = 0; i < num_pages; i++) {
        uint64 addr = start_addr + i * PGSIZE;  // Tính địa chỉ của trang hiện tại
        pte_t *pte = walk(p->pagetable, addr, 0);  // Lấy PTE của trang
        if (pte && (*pte & PTE_A)) {
            kernel_buffer[i / 8] |= (1 << (i % 8));  // Đặt bit tương ứng trong bộ đệm
            *pte &= ~PTE_A;  // Xóa bit PTE_A để tránh lưu thông tin quá lâu
        }
    }

    // Sao chép bộ đệm kết quả về không gian người dùng
    if (copyout(myproc()->pagetable, user_buffer, (char *)kernel_buffer, num_pages) < 0) {
        kfree(kernel_buffer);  // Giải phóng bộ đệm nếu có lỗi
        return -1;
    }

    kfree(kernel_buffer);  // Giải phóng bộ đệm sau khi sao chép xong
    return 0;  // Thành công
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif


uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
