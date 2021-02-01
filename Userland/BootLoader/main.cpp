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
#include <LibCore/ArgsParser.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

static const size_t sector_vector = 0x1ac;
static const size_t storage_medium = 0x1b0;
static const size_t drive_number = 0x1b4;

static const char* bootsector_error = "Your bootsector may be ruined. run grub-install if you have it.";

template<typename T>
static T ceil(T quotient, T divisor)
{
    return (quotient + divisor - 1) / divisor;
}

static int install(int argc, char** argv)
{
    const char* bootsector = "/boot/BootLoader/BootSector.img";
    const char* stage2 = "/boot/BootLoader/BootLoader.img";
    const char* disk = nullptr;

    auto args_parser = Core::ArgsParser();
    args_parser.add_option(bootsector, "Bootsector to be placed on the first sector of the disk", "bootsector", 's', "image");
    args_parser.add_option(stage2, "Bootloader that the bootsector will load", "bootloader", 'l', "image");
    args_parser.add_positional_argument(disk, "Disk which the bootloader will be installed to", "disk");

    args_parser.parse(argc, argv);

    FILE* disk_file = fopen(disk, "w");
    if (disk_file == nullptr) {
        perror("fopen(disk)");
        return 1;
    }
    FILE* bootsector_file = fopen(bootsector, "r");
    if (bootsector_file == nullptr) {
        perror("fopen(bootsector)");
        return 1;
    }

    // Get the premade bootsector.
    char buffer[512];
    fread(buffer, sizeof(char), sizeof(buffer), bootsector_file);
    if (ferror(bootsector_file)) {
        perror("fread(bootsector)");
        return 1;
    }
    fclose(bootsector_file);

    int stage2fd = open(stage2, O_RDONLY);
    if (stage2fd < 0) {
        perror("open(stage2)");
        return 1;
    }

    struct stat stat_data;
    if (fstat(stage2fd, &stat_data) < 0) {
        perror("fstat");
        return 1;
    }

    size_t block_count = ceil<size_t>(stat_data.st_size, stat_data.st_blksize);
    size_t sectors_per_block = stat_data.st_blksize / 512;

    for (size_t i = 0; i < block_count; ++i) {
        int block = i;
        if (ioctl(stage2fd, FIBMAP, &block) < 0) {
            perror("ioctl(FIBMAP)");
            return 1;
        }
        for (size_t j = 0; j < sectors_per_block; ++j) {
            u32 sector = block * sectors_per_block + j;
            size_t offset = sector_vector - 4 * (i * sectors_per_block + j);

            *reinterpret_cast<u32*>(buffer + offset) = sector;
        }
    }

    // FIXME: Select drive and drive type

    close(stage2fd);

    // This is the point of no return.
    int rc = fwrite(buffer, sizeof(char), sizeof(buffer), disk_file);
    if (ferror(disk_file) || rc != sizeof(buffer)) {
        perror("fwrite(disk)");
        warnln(bootsector_error);
        return 3;
    }

    fclose(disk_file);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        warnln("usage: {} [install] [options]", argv[0]);
        return 1;
    }

    if (String("install") == argv[1]) {
        return install(argc - 1, argv + 1);
    } else {
        warnln("usage: {} [install] [options]", argv[0]);
        return 1;
    }
}
