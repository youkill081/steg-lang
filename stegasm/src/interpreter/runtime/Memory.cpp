//
// Created by Roumite on 21/02/2026.
//

#include "Memory.h"

#include "Logger.h"
#include "../exceptions.h"
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

// ----- MemoryBlockSet -----

MemoryBlockSet::MemoryBlockSet()
{
    blocks.emplace_back(0u, MAX_VALUE_IN_UINT32, FREE);
}

uint32_t MemoryBlockSet::find_free_block_index(uint32_t size) const
{
    for (uint32_t i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].free == FREE && blocks[i].size >= size)
        {
            return i;
        }
    }
    throw MemoryError("Not enough memory to allocate " + std::to_string(size));
}

bool MemoryBlockSet::is_address_free(uint32_t address) const
{
    return blocks[find_address_block_index(address)].free == FREE;
}

bool MemoryBlockSet::is_address_used(uint32_t address) const
{
    return blocks[find_address_block_index(address)].free == USED;
}

uint32_t MemoryBlockSet::allocate(uint32_t size)
{
    cached_block_index = 0;
    if (size == 0) throw MemoryError("Cannot allocate zero size");
    const uint32_t index = find_free_block_index(size);

    MemoryBlock new_block(blocks[index].start, size, USED);

    blocks[index].start = blocks[index].start + size;
    blocks[index].size -= size;

    blocks.insert(blocks.begin() + index, std::move(new_block));
    return blocks[index].start;
}

void MemoryBlockSet::allocate_at(uint32_t address, uint32_t size)
{
    if (size == 0) return;
    cached_block_index = 0;

    uint64_t end = static_cast<uint64_t>(address) + static_cast<uint64_t>(size);
    if (end - 1 > MAX_VALUE_IN_UINT32) {
        throw MemoryError("Allocation out of bounds");
    }

    uint32_t index = find_address_block_index(address);
    MemoryBlock& block = blocks[index];

    if (block.free == USED) {
        throw MemoryError("Address " + std::to_string(address) + " is already used");
    }

    if (address + size > block.start + block.size) {
        throw MemoryError("Not enough contiguous free space at this address");
    }

    uint32_t space_before = address - block.start;
    uint32_t space_after = (block.start + block.size) - (address + size);

    if (space_before > 0 && space_after > 0) {
        block.size = space_before;

        MemoryBlock used_block(address, size, USED);
        MemoryBlock free_block_after(address + size, space_after, FREE);

        blocks.insert(blocks.begin() + index + 1, std::move(used_block));
        blocks.insert(blocks.begin() + index + 2, std::move(free_block_after));
    }
    else if (space_before > 0 && space_after == 0) {
        block.size = space_before;

        MemoryBlock used_block(address, size, USED);
        blocks.insert(blocks.begin() + index + 1, std::move(used_block));
    }
    else if (space_before == 0 && space_after > 0) {
        block.free = USED;
        block.size = size;
        block.data.resize(size);

        MemoryBlock free_block_after(address + size, space_after, FREE);
        blocks.insert(blocks.begin() + index + 1, std::move(free_block_after));
    }
    else {
        block.free = USED;
        block.data.resize(size);
        block.size = size;
    }
}

void MemoryBlockSet::merge_all_free_block()
{
    cached_block_index = 0;
    if (blocks.empty()) return;

    for (size_t i = 0; i < blocks.size() - 1;)
    {
        if (blocks[i].free == FREE && blocks[i + 1].free == FREE)
        {
            blocks[i].size += blocks[i + 1].size;
            blocks.erase(blocks.begin() + i + 1);
        }
        else
        {
            i++;
        }
    }
}

uint32_t MemoryBlockSet::find_address_block_index(uint32_t address) const
{
    if (cached_block_index < blocks.size())
    {
        const MemoryBlock &cached_block = blocks[cached_block_index];
        if (cached_block.start <= address && address < cached_block.start + cached_block.size)
        {
            return cached_block_index; // Cache hit
        }
    }

    // Return the first block above the address
    auto it = std::upper_bound(blocks.begin(), blocks.end(), address,
        [](uint32_t addr, const MemoryBlock& block) {
            return addr < block.start;
        });

    if (it == blocks.begin()) {
        throw MemoryError("Address " + std::to_string(address) + " is not in memory");
    }
    it -= 1; // Change to the right block
    if (address >= it->start + it->size) {
        throw MemoryError("Address " + std::to_string(address) + " is not in memory");
    }

    cached_block_index = static_cast<uint32_t>(std::distance(blocks.begin(), it));
    return cached_block_index;
}

void MemoryBlockSet::free(uint32_t address)
{
    cached_block_index = 0;
    const uint32_t index = find_address_block_index(address);

    if (blocks[index].free == FREE)
        throw MemoryError("Address " + std::to_string(address) + " is already free");

    blocks[index].free = FREE;
    blocks[index].data.clear();
    blocks[index].data.shrink_to_fit();

    merge_all_free_block();
}

MemoryBlock& MemoryBlockSet::get_block_for_address(uint32_t address)
{
    const uint32_t idx = find_address_block_index(address);
    return blocks[idx];
}

const MemoryBlock& MemoryBlockSet::get_block_for_address(uint32_t address) const
{
    const uint32_t idx = find_address_block_index(address);
    return blocks[idx];
}

// ----- Memory -----

MemoryBlock& Memory::get_block(uint32_t address)
{
    return _blocks.get_block_for_address(address);
}

uint8_t Memory::read_uint8(uint32_t address) const
{
    if (_blocks.is_address_free(address))
        throw MemoryError("[SEGFAULT] Try to read at FREE address " + std::to_string(address));

    const MemoryBlock& block = _blocks.get_block_for_address(address);
    uint32_t offset = address - block.start;

    if (offset >= block.data.size())
        throw MemoryError("Internal error: offset out of range in read");

    return block.data[offset];
}

uint16_t Memory::read_uint16(uint32_t address) const
{
    if (_blocks.is_address_free(address))
        throw MemoryError("[SEGFAULT] Try to read uint16 at FREE address " + std::to_string(address));

    const MemoryBlock& block = _blocks.get_block_for_address(address);
    uint32_t offset = address - block.start;

    if (offset + 1 >= block.data.size())
        throw MemoryError("Read uint16 out of block bounds");

    return (static_cast<uint16_t>(block.data[offset]) << 8) |
           (static_cast<uint16_t>(block.data[offset + 1]));
}

uint32_t Memory::read_uint32(uint32_t address) const
{
    const MemoryBlock& block = _blocks.get_block_for_address(address);
    uint32_t offset = address - block.start;

    if (offset + 3 >= block.data.size())
        throw MemoryError("Read uint32 out of block bounds");

    return (static_cast<uint32_t>(block.data[offset]) << 24) |
           (static_cast<uint32_t>(block.data[offset + 1]) << 16) |
           (static_cast<uint32_t>(block.data[offset + 2]) << 8) |
           (static_cast<uint32_t>(block.data[offset + 3]));
}

void Memory::write_uint8(uint32_t address, uint8_t value)
{
    MemoryBlock &block = _blocks.get_block_for_address(address);
    if (block.free == FREE)
        throw MemoryError("[SEGFAULT] Try to write at FREE address " + std::to_string(address));

    uint32_t offset = address - block.start;
    if (offset >= block.data.size())
        throw MemoryError("Internal error: offset out of range in write");

    block.data[offset] = value;
}

void Memory::write_uint16(uint32_t address, uint16_t value)
{
    MemoryBlock &block = _blocks.get_block_for_address(address);
    if (block.free == FREE)
        throw MemoryError("[SEGFAULT] Try to write uint16 at FREE address " + std::to_string(address));

    uint32_t offset = address - block.start;
    if (offset + 1 >= block.data.size())
        throw MemoryError("Write uint16 out of block bounds");

    block.data[offset] = static_cast<uint8_t>((value >> 8) & 0xFF);
    block.data[offset + 1] = static_cast<uint8_t>(value & 0xFF);
}

void Memory::write_uint32(uint32_t address, uint32_t value)
{
    MemoryBlock &block = _blocks.get_block_for_address(address);
    if (block.free == FREE)
        throw MemoryError("[SEGFAULT] Try to write uint32 at FREE address " + std::to_string(address));

    uint32_t offset = address - block.start;
    if (offset + 3 >= block.data.size())
        throw MemoryError("Write uint32 out of block bounds");

    block.data[offset] = static_cast<uint8_t>((value >> 24) & 0xFF);
    block.data[offset + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    block.data[offset + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    block.data[offset + 3] = static_cast<uint8_t>(value & 0xFF);
}

uint32_t Memory::allocate(uint32_t size)
{
    return _blocks.allocate(size);
}

void Memory::allocate_at(uint32_t address, uint32_t size)
{
    _blocks.allocate_at(address, size);
}

void Memory::free(uint32_t address)
{
    _blocks.free(address);
}

void Memory::display() const
{
    Logger::log("Memory display", "Memory");
    for (const auto &block : this->_blocks.get_blocks())
    {
        std::stringstream stream;
        stream << "Block " << block.start << " - " << (block.start + block.size) << " : ";
        if (block.free == FREE) stream << "FREE";
        else stream << "USED";
        Logger::log(stream.str(), "Memory");

        if (block.free == USED)
        {
            std::stringstream data_stream;
            auto count = block.data.size();
            for (uint32_t i = 0; i < count; ++i)
                data_stream << " " << static_cast<int>(block.data[i]);
            Logger::log(data_stream.str(), "Memory");
        }
    }
}