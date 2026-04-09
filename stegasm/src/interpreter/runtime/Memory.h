//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <vector>
#include <cstdint>

#define MAX_VALUE_IN_UINT32 4294967295
#define MAX_VALUE_IN_UINT16 65536

enum BlockType
{
    FREE,
    USED
};

struct MemoryBlock
{
    uint32_t start;
    uint32_t size;
    BlockType free;
    std::vector<uint8_t> data;

    explicit MemoryBlock(uint32_t s = 0, uint32_t sz = 0, BlockType f = FREE)
        : start(s), size(sz), free(f)
    {
        if (f == USED && sz > 0)
            data.resize(sz);
    }
};

class MemoryBlockSet
{
private:
    static constexpr uint32_t CACHE_SIZE = 512;
    static constexpr uint32_t CACHE_MASK = CACHE_SIZE - 1;

    std::vector<MemoryBlock> blocks;
    uint32_t _hash_cache[CACHE_SIZE];

    void invalidate_cache();
    void merge_all_free_block();
    [[nodiscard]] uint32_t find_address_block_index(uint32_t address) const;
    [[nodiscard]] uint32_t find_free_block_index(uint32_t size) const;
public:
    mutable uint32_t cached_block_index = 0; // Last block index used, used to avoid searching for free block when linear writing

    MemoryBlockSet();

    [[nodiscard]] const std::vector<MemoryBlock>& get_blocks() const { return blocks; }

    [[nodiscard]] bool is_address_free(uint32_t address) const;
    [[nodiscard]] bool is_address_used(uint32_t address) const;

    uint32_t allocate(uint32_t size);
    void allocate_at(uint32_t address, uint32_t size);

    void free(uint32_t address);

    MemoryBlock& get_block_for_address(uint32_t address);
    const MemoryBlock& get_block_for_address(uint32_t address) const;
};

class Memory
{
private:
    MemoryBlockSet _blocks;
public:
    Memory() = default;

    [[nodiscard]] MemoryBlock &get_block(uint32_t address);
    [[nodiscard]] uint8_t read_uint8(uint32_t address) const;
    [[nodiscard]] uint16_t read_uint16(uint32_t address) const;
    [[nodiscard]] uint32_t read_uint32(uint32_t address) const;
    void write_uint8(uint32_t address, uint8_t value);
    void write_uint16(uint32_t address, uint16_t value);
    void write_uint32(uint32_t address, uint32_t value);

    uint32_t allocate(uint32_t size);
    void allocate_at(uint32_t address, uint32_t size);
    void free(uint32_t address);

    void display() const;
};