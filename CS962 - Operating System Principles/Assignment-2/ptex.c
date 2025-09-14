#include <types.h>
#include <mmap.h>
#include <fork.h>
#include <v2p.h>
#include <page.h>
#include <lib.h>
#include <memory.h>
#include <context.h>

//##################################
/*
 * System call handler for VMA print
 */
//##################################

int print_pmaps(struct exec_context *ctx) {
   
  // Return error if no process context
    if (!ctx) return -1;
  // Return success
    if (!ctx->mms) return 0;
  
  // Extract memory segment
    struct mm_segment *mms = ctx->mms;
    char r, w, x;

    // CODE segment
    r = (mms[0].access_flags & MM_RD) ? 'R' : '-';
    w = (mms[0].access_flags & MM_WR) ? 'W' : '-';
    x = (mms[0].access_flags & MM_EX) ? 'X' : '-';
    printk("%x %x %c%c%c CODE\n", mms[0].start, mms[0].next_free, r, w, x);

    // RODATA segment
    if (mms[1].start != mms[1].next_free) {
        r = (mms[1].access_flags & MM_RD) ? 'R' : '-';
        w = (mms[1].access_flags & MM_WR) ? 'W' : '-';
        x = (mms[1].access_flags & MM_EX) ? 'X' : '-';
        printk("%x %x %c%c%c RODATA\n", mms[1].start, mms[1].next_free, r, w, x);
    }

    // DATA segment
    r = (mms[2].access_flags & MM_RD) ? 'R' : '-';
    w = (mms[2].access_flags & MM_WR) ? 'W' : '-';
    x = (mms[2].access_flags & MM_EX) ? 'X' : '-';
    printk("%x %x %c%c%c DATA\n", mms[2].start, mms[2].next_free, r, w, x);

    // MMAP areas
    struct vm_area *curr = ctx->vm_area;
    while (curr) {
        r = (curr->access_flags & MM_RD) ? 'R' : '-';
        w = (curr->access_flags & MM_WR) ? 'W' : '-';
        x = (curr->access_flags & MM_EX) ? 'X' : '-';
        printk("%x %x %c%c%c MMAP\n", curr->vm_start, curr->vm_end, r, w, x);
        curr = curr->vm_next;
    }

    // STACK segment
    r = (mms[3].access_flags & MM_RD) ? 'R' : '-';
    w = (mms[3].access_flags & MM_WR) ? 'W' : '-';
    x = (mms[3].access_flags & MM_EX) ? 'X' : '-';
    printk("%x %x %c%c%c STACK\n", mms[3].next_free, mms[3].end, r, w, x);

    return 0;  //return success
}


//################################
/*
 * System call handler for PT walk
 */
//################################

long do_walk_pt(struct exec_context *ctx, unsigned long addr) {
 // When Process execution context is NULL, and could not proceed 
    if (!ctx) return -1;

 // Page Global Directory capture 
 // osmap() converts PFN into kernel virtual address to help kernel access the table 
    u64 *pgd = (u64 *)osmap(ctx->pgd);

 // Page table uses 9 bits for each level
 // L1 - PGD (bits 39 to 47), L2 - PUD (bits 30 to 38), L3 - PMD (bits 21 to 29)
 // L4 - PTE (bits 12 to 20), Page offset (bits 0 to 11)
    int indices[4];
    indices[0] = (addr >> 39) & 0x1FF; // L1 index
    indices[1] = (addr >> 30) & 0x1FF; // L2 index
    indices[2] = (addr >> 21) & 0x1FF; // L3 index
    indices[3] = (addr >> 12) & 0x1FF; // L4 index

 // Retrives the virtual address of entry
    u64 *table = pgd;
    for (int level = 0; level < 4; level++) {
        u64 entry = table[indices[level]];
        u64 entry_va = (u64)&table[indices[level]];

 // LSB bit (0x1) is present bit, if not set, mapping does not exist
        if (!(entry & 0x1)) {
            printk("No L%d entry\n", level + 1);
            for (int l = level + 1; l < 4; l++)
                printk("No L%d entry\n", l + 1);
            return 0;
        }

 // Extraction of Physical address and flags
 // Physical Frame Number contains in bits 12 ~ 51
 // From 0 ~ 11 bit, contains various flags (Present, RW, Accessed, Dirty, etc.)
        u64 phys = (entry >> 12) & 0xFFFFFFFFF;
        u64 flags = entry & 0xFFF;

 // Printing of i) Virtual address of page table entry (in kernel space)
 // ii) Raw entry contents, iii) Physical address, iv) Flags
        printk("L%d-entry addr: %x, L%d-entry contents: %x, Physical addr: %x, Flags: %x\n",
               level + 1, entry_va, level + 1, entry, phys, flags);

 // Conversion of PFN of Next level table into Kernel virtual address
        table = (u64 *)osmap(phys);
    }

    return 0; // Return success
}


//##############################
/*
 * System call handler for unmap
 */
//##############################

long vm_area_unmap(struct exec_context *ctx, u64 addr, int length)
{
 // Validates the length to unmap is positive
    if (!ctx || length <= 0)
        return -1;

 // Each process has a linked list of Virtual Memory Areas
 // Plan to iterate through each VMA to find one that contains given range (addr ~ addr+len)
    struct vm_area *vma = ctx->vm_area;
    struct vm_area *prev = NULL;

    while (vma) {

 // Ensuring requested range is completely within current VMA
        if (addr >= vma->vm_start && (addr + length) <= vma->vm_end) {
            // Handle full removal
            if (addr == vma->vm_start && (addr + length) == vma->vm_end) {
                if (prev)
                    prev->vm_next = vma->vm_next;
                else
                    ctx->vm_area = vma->vm_next;

                 os_free(vma, sizeof(struct vm_area));
            }

            // Handle partial unmap (requires splitting)
            else {
                // Adjust vma start or end as needed
                if (addr == vma->vm_start) {
                    vma->vm_start = addr + length;
                } else if ((addr + length) == vma->vm_end) {
                    vma->vm_end = addr;
                } else {
                    // Split into two VMAs
                    // Whie unmapped range is in the middle of VMA, split it into two:
                    // i) Original VMA ends at addr, ii) New VMA starts at addr+Len
                    struct vm_area *new_vma = os_alloc(sizeof(struct vm_area));
                    new_vma->vm_start = addr + length;
                    new_vma->vm_end = vma->vm_end;
                    new_vma->access_flags = vma->access_flags;
                    new_vma->vm_next = vma->vm_next;

                    vma->vm_end = addr;
                    vma->vm_next = new_vma;
                }
            }

            return 0; // Unmapping successful
        }
        prev = vma;
        vma = vma->vm_next;
    }

    return -1; // Address range not found
}
