#pragma once

#include <sead/prim/seadSafeString.h>

namespace sead {

template <typename T>
s32 replaceStringImpl_(T* dst, s32* length, s32 dst_size, const T* src, s32 src_size,
    const SafeStringBase<T>& old_str, const SafeStringBase<T>& new_str,
    bool* is_buffer_overflow)
{
    s32 ret = 0;
    *is_buffer_overflow = false;
    const s32 dst_max_idx = dst_size - 1;

    const T* old_cstr = old_str.cstr();
    const s32 old_str_len = old_str.calcLength();

    if (old_str_len == 0) {
        if (dst == src)
            return 0;

        *is_buffer_overflow = src_size >= dst_size;
        if (src_size >= dst_size) {
            MemUtil::copy(dst, src, dst_max_idx);
            dst[dst_max_idx] = SafeStringBase<T>::cNullChar;
            if (length)
                *length = dst_max_idx;
        } else {
            MemUtil::copy(dst, src, src_size + 1);
            if (length)
                *length = src_size;
        }
        return 0;
    }

    const T* new_cstr = new_str.cstr();
    const s32 new_str_len = new_str.calcLength();

    // Replace in-place.
    if (dst == src && old_str_len < new_str_len) {
        s32 dst_final_size = 0;
        s32 src_final_size = 0;
        // First, terminate the string and check for buffer overflow.
        while (src_final_size < src_size) {
            const s32 cmp = MemUtil::compare(&dst[src_final_size], old_cstr, old_str_len);
            src_final_size += cmp == 0 ? old_str_len : 1;
            dst_final_size += cmp == 0 ? new_str_len : 1;
            if (dst_final_size >= dst_size) {
                *is_buffer_overflow = true;
                break;
            }
        }

        if (*is_buffer_overflow) {
            dst[dst_max_idx] = SafeStringBase<T>::cNullChar;
            if (length)
                *length = dst_max_idx;
        } else {
            dst[dst_final_size] = SafeStringBase<T>::cNullChar;
            if (length)
                *length = dst_final_size;
        }

        s32 dst_i = dst_final_size - 1;
        s32 src_i = src_final_size - 1;
        while (src_i >= 0) {
            const s32 cmp = MemUtil::compare(&dst[src_i + 1 - old_str_len], old_cstr, old_str_len);
            if (cmp == 0) {
                dst_i -= new_str_len;
                const s32 copy_size = std::min(new_str_len, dst_size - 2 - dst_i);
                if (copy_size > 0) {
                    MemUtil::copy(&dst[dst_i + 1], new_cstr, copy_size);
                    ret += 1;
                }
                src_i -= old_str_len;
            } else {
                if (dst_i < dst_max_idx)
                    dst[dst_i] = dst[src_i];
                if (src_i < 1) {
                    --src_i;
                    --dst_i;
                    break;
                }
            }
        }

        SEAD_ASSERT(dst_i == -1);
        SEAD_ASSERT(src_i == -1);
    }
    // Simpler case.
    else {
        s32 target_i = 0;
        s32 buffer_i = 0;
        while (target_i < src_size) {
            const s32 cmp = MemUtil::compare(&src[target_i], old_cstr, old_str_len);
            // Not old_str, copy one character to the buffer.
            if (cmp != 0) {
                if (buffer_i < dst_max_idx) {
                    dst[buffer_i++] = src[target_i++];
                    continue;
                }
            }
            // Found old_str, copy new_str to the buffer.
            else {
                const s32 copy_size = std::min(new_str_len, dst_max_idx - buffer_i);
                if (copy_size >= 1)
                    MemUtil::copy(&dst[buffer_i], new_cstr, copy_size);
                ret += new_str_len == 0 || copy_size > 0;
                if (copy_size >= new_str_len) {
                    buffer_i += new_str_len;
                    target_i += old_str_len;
                    continue;
                }
            }

            // Buffer overflow.
            *is_buffer_overflow = true;
            dst[dst_max_idx] = SafeStringBase<T>::cNullChar;
            if (length)
                *length = dst_max_idx;
            return ret;
        }

        SEAD_ASSERT(buffer_i <= dst_size);
        SEAD_ASSERT(target_i == src_size);

        dst[buffer_i] = SafeStringBase<T>::cNullChar;
        if (length)
            *length = buffer_i;
    }

    return ret;
}

template s32 replaceStringImpl_<char>(char* buffer, s32* length, s32 buffer_size,
    const char* target_buf, s32 target_len,
    const SafeStringBase<char>& old_str,
    const SafeStringBase<char>& new_str,
    bool* is_buffer_overflow);

template s32 replaceStringImpl_<char16>(char16* buffer, s32* length, s32 buffer_size,
    const char16* target_buf, s32 target_len,
    const SafeStringBase<char16>& old_str,
    const SafeStringBase<char16>& new_str,
    bool* is_buffer_overflow);

} // namespace sead
