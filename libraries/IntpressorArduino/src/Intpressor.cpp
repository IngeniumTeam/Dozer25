#include "Intpressor.h"

namespace Intpressor
{
    int compress(const int *values, const int *sizes, int numValues, char *bytes)
    {
        int totalSize = 0;
        for (int i = 0; i < numValues; i++)
        {
            totalSize += sizes[i];
        }
        int bitIndex = 0;
        int byteIndex = 0;
        bool bits[MAX_VALUES];
        memset(bits, 0, sizeof(bits));
        for (int valueIndex = 0; valueIndex < numValues; valueIndex++)
        {
            int size = sizes[valueIndex];
            int value = values[valueIndex];
            for (int i = 0; i < size; i++)
            {
                bits[bitIndex + i] = ((value & (1 << (size - i - 1))) != 0);
            }
            bitIndex += size;
        }
        int numBytes = 0;
        for (int i = 0; i < totalSize; i += 8)
        {
            unsigned char byte = 0;
            for (int j = 0; j < 8; ++j)
            {
                byte |= (bits[i + j] << (7 - j));
            }
            bytes[i / 8] = byte;
            numBytes++;
        }
        return numBytes;
    }

    void extract(const char *bytes, const int *sizes, int numValues, int *values)
    {
        int bitIndex = 0;
        int byteIndex = 0;
        for (int valueIndex = 0; valueIndex < numValues; valueIndex++)
        {
            int size = sizes[valueIndex];
            int value = 0;
            for (int i = 0; i < size; i++)
            {
                value = (value << 1) | static_cast<int>((bytes[byteIndex] & (1 << (7 - bitIndex))) != 0);
                bitIndex++;

                if (bitIndex >= 8)
                {
                    bitIndex = 0;
                    byteIndex++;
                }
            }
            values[valueIndex] = value;
        }
    }
}
