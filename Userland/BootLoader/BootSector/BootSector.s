/*
 * Copyright (c) 2021, Peter Elliott <pelliott@ualberta.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
    .code16

    .global _start
_start:
    cli
    xor %ax, %ax
    /* Zero out segment registers. */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    /* %ebx is our sector counter */
    xor %ebx, %ebx

load_loop:
    /* Copy the current sector number into the disk packet. */
    mov %ebx, %ecx
    neg %ecx
    mov sector_vector(,%ecx, 4), %ecx
    cmp $0, %ecx
    jz end_load
    //mov %ecx, disk_packet_sector

    /* Load the buffer address into the disk packet. */
    mov %ebx, %ecx
    shl $9, %ecx /* %ecx * 512 */
    add 0x00007E00(%ecx), %ecx
    mov %ecx, disk_packet_buffer

    /* Set up the arguments to int 0x13. */
    mov $0x42, %ah
    mov drive_number, %dl
    mov disk_packet, %si

    /* Request the sector */
    int $0x13

    jmp .
    /* We could check for errors, but there's not much we could do about it. */

    jmp load_loop
end_load:
    /* Out 2nd stage bootloader is now loaded at 0x0x00007E00 */

    /* Set up an ~30KiB stack, over our bootloader */
    mov $0x00007E00, %esp
    jmp 0x00007E00

    .align 8
disk_packet:
disk_packet_size:
    .byte 0x10, 0
disk_packet_nblocks:
    .2byte 1
disk_packet_buffer:
    .4byte 0
disk_packet_sector:
    .8byte 0

    .fill 0x1ac - (. - _start), 1, 0
sector_vector:
    .4byte 0
storage_medium:
    .byte 0
drive_number:
    .byte 0x80

    /*  FIXME: Some kind of partition data goes here. */

    .fill 0x1fe - (. - _start), 1, 0
    .word 0xaa55
