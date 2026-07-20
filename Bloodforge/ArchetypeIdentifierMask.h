#pragma once
#include <vector>
#include <algorithm>

namespace Bloodforge
{
    struct ArchetypeIdentifierMask
    {
    public:
        std::vector<uint64_t> Bits;

        const std::vector<int>& GetComponentIndices()
        {
            if (!isSorted)
            {
                std::sort(componentIndices.begin(), componentIndices.end());
                isSorted = true;
            }
            return componentIndices;
        }

        bool operator==(ArchetypeIdentifierMask const& other) const noexcept
        {
            return Bits == other.Bits;
        }

        void AddComponent(int componentId)
        {
            int word = componentId >> 6;
            if (word >= Bits.size()) Bits.resize(word + 1);
            Bits[word] |= (1ULL << (componentId & 63));

            isSorted = false;
            componentIndices.emplace_back(componentId);
        }

        void RemoveComponent(int componentId)
        {
            int word = componentId >> 6;
            if (word < Bits.size())
            {
                Bits[word] &= ~(1ULL << (componentId & 63));
            }

            componentIndices.erase(std::remove(componentIndices.begin(), componentIndices.end(), componentId), componentIndices.end());
        }

        bool HasComponent(int componentId) const
        {
            int word = componentId >> 6;
            return word < Bits.size() && (Bits[word] & (1ULL << (componentId & 63)));
        }
    private:
        std::vector<int> componentIndices;
        bool isSorted = true;
    };

    struct ArchetypeIdentifierMaskHash
    {
        size_t operator()(ArchetypeIdentifierMask const& mask) const noexcept
        {
            size_t hash = 14695981039346656037ULL;
            for (uint64_t word : mask.Bits)
            {
                hash ^= word;
                hash *= 1099511628211ULL;
            }
            return hash;
        }
    };
}